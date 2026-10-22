# vs:auto-managed - the Claude Code VS extension (re)writes this file on Launch/connect so fixes ship automatically. Remove THIS LINE to take ownership and the extension will leave this file alone.
# Single-gate PreToolUse hook for Edit/Write/MultiEdit (auto-installed by the Claude Code VS extension).
# Reconstructs the proposed file content, posts it to the VS bridge's /permission endpoint, and lets
# the native VS diff (Accept/Reject) be the SOLE gate. Fail-open: any error -> allow (never block the CLI).
# Output contract: exit 0 + one JSON line on stdout with the PreToolUse permission decision.

$ErrorActionPreference = 'Stop'

function Emit([string]$decision, [string]$reason) {
    @{ hookSpecificOutput = @{ hookEventName = 'PreToolUse'; permissionDecision = $decision; permissionDecisionReason = $reason } } |
        ConvertTo-Json -Compress -Depth 8
    exit 0
}

function ApplyEdit([string]$content, [string]$old, [string]$new, [bool]$all) {
    if ([string]::IsNullOrEmpty($old)) { return $content }
    # Match the file's newline convention so the diff isn't a sea of line-ending changes.
    if ($content -match "`r`n") {
        $old = $old -replace "`n", "`r`n"
        $new = $new -replace "`n", "`r`n"
    }
    if ($all) { return $content.Replace($old, $new) }
    $idx = $content.IndexOf($old)
    if ($idx -lt 0) { return $content } # not found (e.g. newline mismatch) -> leave unchanged
    return $content.Substring(0, $idx) + $new + $content.Substring($idx + $old.Length)
}

try {
    # Read stdin as UTF-8: the default console input encoding garbles non-ASCII (em-dashes, smart
    # quotes) in the hook payload, which then fails to match the file content during reconstruction.
    $stdin = New-Object System.IO.StreamReader([Console]::OpenStandardInput(), [System.Text.Encoding]::UTF8)
    $payload = $stdin.ReadToEnd()
    $p = $payload | ConvertFrom-Json
    $tool = $p.tool_name
    $ti = $p.tool_input
    $file = $ti.file_path

    # Read as UTF-8 (PS 5.1 Get-Content defaults to the ANSI codepage, which garbles em-dashes etc.).
    $cur = if ($file -and (Test-Path -LiteralPath $file)) { Get-Content -Raw -LiteralPath $file -Encoding UTF8 } else { '' }
    switch ($tool) {
        'Write'     { $new = [string]$ti.content }
        'Edit'      { $new = ApplyEdit $cur $ti.old_string $ti.new_string ([bool]$ti.replace_all) }
        'MultiEdit' { $new = $cur; foreach ($e in $ti.edits) { $new = ApplyEdit $new $e.old_string $e.new_string ([bool]$e.replace_all) } }
        default     { Emit 'allow' "unhandled tool $tool" }
    }

    # Find the Visual Studio bridge: the MOST-SPECIFIC workspace match (longest workspaceFolders prefix
    # of this cwd) whose port is actually listening. Avoids two failure modes: a parent-folder instance
    # (e.g. the repo root) shadowing a subfolder, and a stale "zombie" lockfile (dead instance, recycled
    # PID) whose port no longer answers.
    function Test-Port([int]$pt) {
        try {
            $c = New-Object System.Net.Sockets.TcpClient
            $live = $c.BeginConnect('127.0.0.1', $pt, $null, $null).AsyncWaitHandle.WaitOne(300) -and $c.Connected
            $c.Close(); return $live
        } catch { return $false }
    }
    $ideDir = Join-Path $env:USERPROFILE '.claude\ide'
    $cands = @()
    foreach ($f in Get-ChildItem $ideDir -Filter *.lock -ErrorAction SilentlyContinue) {
        try {
            $j = Get-Content -Raw $f.FullName | ConvertFrom-Json
            if ($j.ideName -ne 'Visual Studio') { continue }
            $ws = if ($j.workspaceFolders) { [string]$j.workspaceFolders[0] } else { '' }
            # Separator-aware match (case-insensitive, / and \ equivalent): 'C:\work\app' must NOT match a
            # session in 'C:\work\app-service'. Containment counts BOTH ways, ranked: an exact match beats a
            # session inside the workspace, which beats a workspace inside the session (open a .sln from a
            # subfolder, run claude from the repo root - still this bridge's work). Ties break toward the
            # more specific workspace, so a nested VS instance wins over its parent.
            $wsN = ($ws -replace '/', '\').TrimEnd('\'); $cwdN = ([string]$p.cwd -replace '/', '\').TrimEnd('\')
            $rank = 0
            if ($wsN -and $cwdN) {
                if     ($cwdN -eq $wsN)             { $rank = 3 }
                elseif ($cwdN -like ($wsN + '\*'))  { $rank = 2 }
                elseif ($wsN -like ($cwdN + '\*'))  { $rank = 1 }
            }
            $cands += [pscustomobject]@{ Port = [int]$f.BaseName; Token = $j.authToken; Score = ($rank * 1000000 + $ws.Length) }
        } catch { }
    }
    $port = $null; $token = $null
    foreach ($cand in ($cands | Sort-Object Score -Descending)) {
        if (Test-Port $cand.Port) { $port = $cand.Port; $token = $cand.Token; break }
    }
    if (-not $port) { Emit 'allow' 'no Visual Studio bridge lockfile found' }

    # Pass the CLI's own permission mode through: when the user chose a pre-approving mode
    # for the session, the bridge allows without opening the diff (our gate must never be stricter
    # than the user's explicit CLI-level choice). Absent on older CLIs -> bridge gates as always.
    # cwd rides along so the bridge can refuse to gate a session that belongs to a DIFFERENT workspace
    # (the zero-match fallback above can land on the wrong VS instance - PR #28). The bridge answers
    # ask=true for those, handing the decision back to the CLI's own permission prompt.
    # pid + entrypoint are how the bridge tells WHICH session this is (issue #42). This hook process is a
    # descendant of its own CLI, so the bridge can check whether the CLI connected to it is one of our
    # ancestors; folder paths cannot answer that, because a second Claude session in the same tree - one
    # running under VS Code - matches them just as well and would otherwise steal the diff into VS.
    $body = @{ filePath = $file; newContents = $new; transcript_path = $p.transcript_path; permissionMode = [string]$p.permission_mode; cwd = [string]$p.cwd; pid = $PID; entrypoint = [string]$env:CLAUDE_CODE_ENTRYPOINT } | ConvertTo-Json -Compress -Depth 8
    # Send the body as explicit UTF-8 bytes; Invoke-RestMethod's default string encoding mangles
    # non-ASCII content (em-dashes, smart quotes) into invalid JSON.
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($body)
    $resp = Invoke-RestMethod -Uri "http://127.0.0.1:$port/permission" -Method Post `
        -ContentType 'application/json; charset=utf-8' `
        -Headers @{ 'x-claude-code-ide-authorization' = $token } `
        -Body $bytes -TimeoutSec 86400

    if ($resp.ask) {
        # The bridge declined to gate: this POST is not from the session connected to that Visual Studio.
        # Exit with NO decision rather than 'ask', so the CLI simply runs its normal permission flow -
        # which, if this session belongs to another IDE, means that IDE shows its own diff. Emitting 'ask'
        # here would instead FORCE a prompt, overriding a user who had chosen acceptEdits (issue #42).
        exit 0
    }
    elseif ($resp.allow) {
        Emit 'allow' 'Accepted in Visual Studio diff'
    }
    else {
        $why = if ($resp.reason) { [string]$resp.reason } else { 'Rejected in Visual Studio diff' }
        Emit 'deny' $why
    }
}
catch {
    Emit 'allow' ("hook error (allowing): " + $_.Exception.Message)
}
