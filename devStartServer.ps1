<#
 .DESCRIPTION: Run/logging script for flserver.exe
 .SYNOPSIS: This script checks for running instances of flserver.exe, kills them, and then runs an instance of flserver.exe from the pre-defined BMOD_COPY_PATH directory and pipes server spew into the console with log highlighting for warnings and errors until the application is terminated. If the application terminates unexpectedly, it will fetch the application crash event log from the Windows Event Logs for quick debugging.
 .AUTHOR: IrateRedKite, Lazrius 
 .REVISION HISTORY: 
 v1.0 2023-10-12: Initial release
#>
Param ($noLaunch)
$init = { function Get-LogColor {
        Param([Parameter(Position = 0)]
            [String]$logEntry)
    
        process {
            if ($logEntry.Contains("DEBUG")) { Return "Green" }
            elseif ($logEntry.Contains("WARNING")) { Return "Yellow" }
            elseif ($logEntry.Contains("couldnt find")) { Return "Cyan" }
            elseif ($logEntry.Contains("ERROR")) { Return "Red" }
            elseif ($logEntry.Contains("Server running, ready for log in.")) { Return "Green" }
            elseif ($logEntry.Contains("CShutdown")) { Return "Red" }
            elseif ($logEntry.Contains("Login")) { Return "Blue" }
            elseif ($logEntry.Contains("Logout")) { Return "Blue" }
            else { Return "White" }
        }
    }
}
function TailFileUntilProcessStops {
    Param ($processID, $filePath)
    $loopBlock = {
        Param($filePath) Get-Content $filePath -Wait | ForEach-Object { Write-Host -ForegroundColor (Get-LogColor $_) $_ }
    }
    $tailLoopJob = start-job -scriptBlock $loopBlock -ArgumentList $filePath -InitializationScript $init
    try {
        do {
            $tailLoopJob | Receive-Job
            try {
                Wait-Process -id $processID -ErrorAction Stop -Timeout 1
                $waitMore = $false
            }
            catch {
                $waitMore = $true
            }
        } while ($waitMore)
    }
    finally {
        Stop-Job $tailLoopJob
        Remove-Job $tailLoopJob
    }
}
Write-Host "Looking for current instances of flserver.exe" -ForegroundColor Blue
$flServer = Get-Process flserver -ErrorAction SilentlyContinue
if ($flServer) {
    $flServerName = $flServer.Path
    $flServerId = $flServer.Id
    Write-Host "Found $flServerName ($flServerId)" -ForegroundColor Yellow 
    Write-Host "Stopping $flServerName ($flServerId)" -ForegroundColor Yellow
    Get-Process "flserver" | Stop-Process
    $flServer.WaitForExit()
}
if (!$env:BMOD_COPY_PATH) {
    $env:BMOD_COPY_PATH = Read-Host -Prompt "Enter the full path for the EXE folder of the Freelancer Server instance you wish to run"
    Write-Host "BMOD_COPY_PATH has been set to $env:BMOD_COPY_PATH" -ForegroundColor Blue
}
if ($env:BMOD_COPY_PATH) {
        $startTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"
        $spewLocation = "$env:BMOD_COPY_PATH\serverspew.log"
        $flServerJob = Start-Process -WorkingDirectory $env:BMOD_COPY_PATH -PassThru -FilePath "$env:BMOD_COPY_PATH\flserver.exe" -ArgumentList "-lserverspew.log", "-c"
        $flServerJobId = $flServerJob.Id
        Write-Host "Starting Freelancer Server instance at $startTime with PID $flServerJobId" -ForegroundColor Green
        Write-Host "Logging $spewLocation to console" -ForegroundColor Blue
        TailFileUntilProcessStops -processID $flServerJob.id -filepath $spewLocation
        $endTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"  
        $hexJobId = ('0x{0:x}' -f $flServerJobId)
        $err = Get-WinEvent -FilterHashtable @{LogName = 'Application'; ID = 1000 } -MaxEvents 1 | Where-Object -Property Message -Match 'Faulting application name: flserver.exe' | Where-Object -Property Message -Match "$hexJobId"
        if ($err) {
            Write-Host "Freelancer Server instance with PID $flServerJobId has terminated unexpectedly at $endTime. Fetching crash event data from Application logs... " -ForegroundColor Red
            Write-Host $err.Message
            Write-Host "For debugging, please reference the Fault offset and Faulting module name against https://wiki.the-starport.net/wiki/fl-binaries/crash-offsets" -ForegroundColor Red
        }
        else {
            Write-Host "Freelancer Server instance with PID $flServerJobId has terminated at $endTime" -ForegroundColor Green
        }
}
else {
    Write-Host "The BMOD_COPY_PATH environment variable is not set! Aborting..." -ForegroundColor Red
    exit
}