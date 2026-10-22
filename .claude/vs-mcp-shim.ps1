# vs:auto-managed - the Claude Code VS extension (re)writes this file on Launch/connect so fixes ship automatically. Remove THIS LINE to take ownership and the extension will leave this file alone.
# stdio MCP proxy shim (auto-installed by the Claude Code VS extension; registered in .mcp.json).
#
# The CLI launches this as an ordinary stdio MCP server. The actual debug tools (vs_debug_state,
# vs_list_breakpoints, vs_get_frame_locals, vs_evaluate) live IN the VS extension, reachable over the
# bridge's authenticated localhost HTTP endpoint (POST /mcp). This script is a dumb pipe: it forwards
# each JSON-RPC message from stdin to that endpoint and writes the reply to stdout. Keeping the logic
# in C# means we don't reimplement EnvDTE access here, and the .mcp.json entry stays static.
#
# Why a shim at all (vs. a plain type:http MCP entry)? The bridge port is dynamic per VS instance. The
# shim discovers the LIVE instance at connect time using the same most-specific + listening lockfile
# selection the hooks use - so it Just Works across multiple VS windows and survives zombie lockfiles,
# without rewriting .mcp.json on every launch.
#
# Contract: stdout carries ONLY framed JSON-RPC (newline-delimited, one message per line, UTF-8, no BOM).
# Anything else on stdout corrupts the MCP stream, so diagnostics must go to stderr only. Fail-soft:
# if the bridge is unreachable we answer requests with a JSON-RPC error (never hang the CLI) and drop
# notifications.
#
# -Route selects which bridge MCP endpoint this shim proxies to, so ONE script backs both MCP servers:
#   /mcp           -> vs-debug    (runtime/debugger tools)
#   /mcp-semantic  -> vs-semantic (Roslyn code-navigation tools)
# The .mcp.json entry for each server passes the matching -Route; default keeps the original vs-debug behavior.

param([string]$Route = '/mcp')

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'   # suppress Invoke-WebRequest's progress bar (it can hit stderr)

$script:Port = $null
$script:Token = $null

function Test-Port([int]$pt) {
    try {
        $c = New-Object System.Net.Sockets.TcpClient
        $live = $c.BeginConnect('127.0.0.1', $pt, $null, $null).AsyncWaitHandle.WaitOne(300) -and $c.Connected
        $c.Close(); return $live
    } catch { return $false }
}

# Find the live Visual Studio bridge: the MOST-SPECIFIC workspace match (longest workspaceFolders prefix
# of this process's cwd - the CLI launches us in its project dir) whose port is actually listening.
# Mirrors the hook discovery so multi-window / zombie-lockfile behavior is identical. Sets $script:Port
# and $script:Token; returns $true on success.
function Resolve-Bridge {
    $cwd = (Get-Location).Path
    $ideDir = Join-Path $env:USERPROFILE '.claude\ide'
    $cands = @()
    foreach ($f in Get-ChildItem $ideDir -Filter *.lock -ErrorAction SilentlyContinue) {
        try {
            $j = Get-Content -Raw $f.FullName | ConvertFrom-Json
            if ($j.ideName -ne 'Visual Studio') { continue }
            $ws = if ($j.workspaceFolders) { [string]$j.workspaceFolders[0] } else { '' }
            # Separator-aware, containment BOTH ways, ranked (see vs-permission-hook.ps1 for the full
            # rationale): exact > session inside workspace > workspace inside session > unrelated. The
            # old bare '$ws + "*"' prefix also matched a sibling like 'C:\work\app-service'.
            $wsN = ($ws -replace '/', '\').TrimEnd('\'); $cwdN = ([string]$cwd -replace '/', '\').TrimEnd('\')
            $rank = 0
            if ($wsN -and $cwdN) {
                if     ($cwdN -eq $wsN)             { $rank = 3 }
                elseif ($cwdN -like ($wsN + '\*'))  { $rank = 2 }
                elseif ($wsN -like ($cwdN + '\*'))  { $rank = 1 }
            }
            $cands += [pscustomobject]@{ Port = [int]$f.BaseName; Token = $j.authToken; Score = ($rank * 1000000 + $ws.Length) }
        } catch { }
    }
    foreach ($cand in ($cands | Sort-Object Score -Descending)) {
        if (Test-Port $cand.Port) { $script:Port = $cand.Port; $script:Token = $cand.Token; return $true }
    }
    return $false
}

# Forward one JSON-RPC message to the bridge; return the (possibly empty) reply body. Throws on transport
# failure so the caller can re-discover and retry. Decode the reply from RAW BYTES as UTF-8 ourselves:
# PS 5.1's $resp.Content pre-decodes with the response charset, or Latin-1 when none is declared, which
# mangles any non-ASCII in tool output (e.g. a window title's Unicode) into mojibake.
function Send-ToBridge([string]$json) {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($json)
    $resp = Invoke-WebRequest -Uri "http://127.0.0.1:$($script:Port)$Route" -Method Post `
        -ContentType 'application/json; charset=utf-8' `
        -Headers @{ 'x-claude-code-ide-authorization' = $script:Token } `
        -Body $bytes -TimeoutSec 60 -UseBasicParsing
    if ($resp.RawContentStream) {
        return [System.Text.Encoding]::UTF8.GetString($resp.RawContentStream.ToArray())
    }
    return [string]$resp.Content
}

# UTF-8 stdio, no BOM, autoflush so the CLI sees each reply immediately.
$stdin = New-Object System.IO.StreamReader([Console]::OpenStandardInput(), [System.Text.Encoding]::UTF8)
$stdout = New-Object System.IO.StreamWriter([Console]::OpenStandardOutput(), (New-Object System.Text.UTF8Encoding($false)))
$stdout.AutoFlush = $true

Resolve-Bridge | Out-Null   # best-effort warm-up; tools/call re-resolves if the bridge moved

while ($null -ne ($line = $stdin.ReadLine())) {
    $line = $line.Trim()
    if (-not $line) { continue }

    # Determine whether a reply is owed (requests have an id; notifications don't) so we can answer with
    # an error if the bridge is down without hanging the CLI on a request.
    $hasId = $false; $id = $null
    try {
        $msg = $line | ConvertFrom-Json
        if ($msg.PSObject.Properties.Name -contains 'id') { $hasId = $true; $id = $msg.id }
    } catch { }

    $response = $null; $ok = $false
    for ($attempt = 0; $attempt -lt 2 -and -not $ok; $attempt++) {
        if (-not $script:Port) { if (-not (Resolve-Bridge)) { break } }
        try { $response = Send-ToBridge $line; $ok = $true }
        catch { $script:Port = $null }   # bridge moved/restarted -> re-resolve on the next attempt
    }

    if (-not $ok) {
        if ($hasId) {
            $err = @{ jsonrpc = '2.0'; id = $id; error = @{ code = -32000; message = 'Visual Studio bridge unavailable' } } |
                ConvertTo-Json -Compress -Depth 6
            $stdout.Write($err); $stdout.Write("`n")
        }
        continue
    }

    if ($response) { $stdout.Write($response); $stdout.Write("`n") }
}
