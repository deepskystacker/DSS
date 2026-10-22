# vs:auto-managed - the Claude Code VS extension (re)writes this file on Launch/connect so fixes ship automatically. Remove THIS LINE to take ownership and the extension will leave this file alone.
# Stop hook (auto-installed by the Claude Code VS extension). Reports the conversation transcript path
# to the VS bridge's /usage endpoint so the dockable panel can show token/cost stats. Observe-only:
# always exits 0 so the CLI's turn-end is never blocked.
$ErrorActionPreference = 'Stop'
try {
    # Read stdin as UTF-8 (default console input encoding garbles non-ASCII).
    $stdin = New-Object System.IO.StreamReader([Console]::OpenStandardInput(), [System.Text.Encoding]::UTF8)
    $p = $stdin.ReadToEnd() | ConvertFrom-Json
    $transcript = $p.transcript_path
    if (-not $transcript) { exit 0 }

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
    if (-not $port) { exit 0 }

    # cwd rides along so the bridge can ignore a session that belongs to a different workspace
    # (the zero-match fallback above can land on the wrong VS instance - PR #28). permissionMode rides
    # along so the panel's run-wild checkbox tracks a shift+tab mode change at the next turn end, rather
    # than staying stuck until the session happens to make an edit.
    # pid + entrypoint identify WHICH session this is - see vs-permission-hook.ps1 (issue #42).
    $body = @{ transcript_path = $transcript; cwd = [string]$p.cwd; permissionMode = [string]$p.permission_mode; pid = $PID; entrypoint = [string]$env:CLAUDE_CODE_ENTRYPOINT } | ConvertTo-Json -Compress
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($body)
    Invoke-RestMethod -Uri "http://127.0.0.1:$port/usage" -Method Post `
        -ContentType 'application/json; charset=utf-8' `
        -Headers @{ 'x-claude-code-ide-authorization' = $token } `
        -Body $bytes -TimeoutSec 5 | Out-Null
} catch { }
exit 0
