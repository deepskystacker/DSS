# vs:auto-managed - the Claude Code VS extension (re)writes this file on Launch/connect so fixes ship automatically. Remove THIS LINE to take ownership and the extension will leave this file alone.
# UserPromptSubmit hook (auto-installed by the Claude Code VS extension). When Visual Studio is paused
# at a breakpoint, this injects the live debug state - stop location, call stack, locals/arguments -
# into Claude's context for this turn, so Claude can reason about RUNTIME values without calling a tool
# or making an edit. This is the only hook event that can push context in at prompt-submit time.
#
# No-op when not in break mode (emits nothing -> injects nothing, so non-debugging turns stay clean).
# Fail-open: any error exits 0 with no output. Output contract: exit 0 + a UserPromptSubmit JSON line
# carrying hookSpecificOutput.additionalContext.
$ErrorActionPreference = 'Stop'
try {
    # Read stdin as UTF-8 (default console input encoding garbles non-ASCII).
    $stdin = New-Object System.IO.StreamReader([Console]::OpenStandardInput(), [System.Text.Encoding]::UTF8)
    $p = $stdin.ReadToEnd() | ConvertFrom-Json

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
            # Separator-aware prefix match (case-insensitive, / and \ equivalent): 'C:\work\app' must
            # NOT match a session in 'C:\work\app-service'.
            $wsN = ($ws -replace '/', '\').TrimEnd('\'); $cwdN = ([string]$p.cwd -replace '/', '\').TrimEnd('\')
            # Containment counts BOTH ways, ranked (see vs-permission-hook.ps1 for the full rationale):
            # exact > session inside workspace > workspace inside session > unrelated.
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
    if (-not $port) { exit 0 } # no VS bridge -> inject nothing

    # permissionMode rides along so the panel's run-wild checkbox tracks a shift+tab mode change at the
    # next prompt, rather than staying stuck until the session happens to make an edit.
    # pid + entrypoint identify WHICH session this is - see vs-permission-hook.ps1 (issue #42).
    $body = @{ cwd = $p.cwd; permissionMode = [string]$p.permission_mode; pid = $PID; entrypoint = [string]$env:CLAUDE_CODE_ENTRYPOINT } | ConvertTo-Json -Compress
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($body)
    # Short timeout on purpose: the bridge itself caps the UI-thread read at ~2s and answers fast (real
    # state when paused, "unknown" when the UI thread is busy). We only need headroom over that. Keeping
    # it well under the 10s hook budget means a slow/busy VS never gets this hook killed - it just injects
    # nothing this turn (fail-open via the outer catch).
    $resp = Invoke-RestMethod -Uri "http://127.0.0.1:$port/debug-context" -Method Post `
        -ContentType 'application/json; charset=utf-8' `
        -Headers @{ 'x-claude-code-ide-authorization' = $token } `
        -Body $bytes -TimeoutSec 4

    # Only inject while actually stopped at a breakpoint.
    if (-not $resp -or $resp.mode -ne 'break') { exit 0 }

    # Render a compact, readable context block from the snapshot.
    $sb = New-Object System.Text.StringBuilder
    [void]$sb.AppendLine('The Visual Studio debugger is paused at a breakpoint. Current runtime state:')
    if ($resp.stoppedAt) {
        [void]$sb.AppendLine(("- Stopped at {0}:{1} in {2}()" -f $resp.stoppedAt.file, $resp.stoppedAt.line, $resp.stoppedAt.function))
    }
    if ($resp.callStack -and $resp.callStack.Count -gt 0) {
        [void]$sb.AppendLine('- Call stack (innermost first):')
        foreach ($fr in $resp.callStack) { [void]$sb.AppendLine(("    {0}" -f $fr.function)) }
    }
    if ($resp.args -and $resp.args.Count -gt 0) {
        [void]$sb.AppendLine('- Arguments (current frame):')
        foreach ($a in $resp.args) { [void]$sb.AppendLine(("    {0} ({1}) = {2}" -f $a.name, $a.type, $a.value)) }
    }
    if ($resp.locals -and $resp.locals.Count -gt 0) {
        [void]$sb.AppendLine('- Locals (current frame):')
        foreach ($l in $resp.locals) { [void]$sb.AppendLine(("    {0} ({1}) = {2}" -f $l.name, $l.type, $l.value)) }
    }

    # Structured UserPromptSubmit output (confirmed contract): hookEventName must be exact, and
    # additionalContext is what the model sees alongside the prompt.
    @{ hookSpecificOutput = @{ hookEventName = 'UserPromptSubmit'; additionalContext = $sb.ToString() } } |
        ConvertTo-Json -Compress -Depth 8
}
catch { } # fail-open: inject nothing on any error
exit 0
