<#
 .DESCRIPTION: Asset Copy and Run Script for Freelancer BMOD
 .SYNOPSIS: This script checks for running instances of Freelancer.exe, kills them, and then runs an instance of Freelancer.exe from the pre-defined BMOD_COPY_PATH directory and pipe FlSpew.txt into the console with log highlighting for warnings and errors until the application is terminated. If the application terminates unexpectedly, it will fetch the application crash event log from the Windows Event Logs for quick debugging.
 .AUTHOR: IrateRedKite, Lazrius 
 .REVISION HISTORY: 
 v1.0 2023-10-12: Initial release
 v1.1 2023-10-12: Adjusted for BMOD debugging
#>
Param ($noLaunch)
$init = { function Get-LogColor {
        Param([Parameter(Position = 0)]
            [String]$logEntry)
    
        process {
            if ($logEntry.Contains("DEBUG")) { Return "Green" }
            elseif ($logEntry.Contains("WARNING")) { Return "Yellow" }
            elseif ($logEntry.Contains("fail")) { Return "Cyan" }
            elseif ($logEntry.Contains("couldnt find")) { Return "Cyan" }
            elseif ($logEntry.Contains("ERROR")) { Return "Red" }
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
Write-Host "Looking for current instances of Freelancer.exe" -ForegroundColor Blue
$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer) {
    $freelancerName = $freelancer.Path
    $freelancerId = $freelancer.Id
    Write-Host "Found $freelancerName ($freelancerId)" -ForegroundColor Yellow 
    Write-Host "Stopping $freelancerName ($freelancerId)" -ForegroundColor Yellow
    Get-Process "freelancer" | Stop-Process
    $freelancer.WaitForExit()
}
if (!$env:BMOD_COPY_PATH) {
    $env:BMOD_COPY_PATH = Read-Host -Prompt "Enter the full path for the EXE folder of the Freelancer instance you wish to run"
    Write-Host "BMOD_COPY_PATH has been set to $env:BMOD_COPY_PATH" -ForegroundColor Blue
}
if ($env:BMOD_COPY_PATH) {
    if (!$noLaunch) {
        $startTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"
        $spewLocation = "$env:LOCALAPPDATA\Freelancer\FLSpew.txt"
        $freelancerJob = Start-Process -PassThru -FilePath "$env:BMOD_COPY_PATH\Freelancer.exe" -ArgumentList "-w"
        $freelancerJobId = $freelancerJob.Id
        Write-Host "Starting Freelancer BMOD in windowed mode at $startTime with PID $freelancerJobId" -ForegroundColor Green
        Write-Host "Logging $spewLocation to console" -ForegroundColor Blue
        TailFileUntilProcessStops -processID $freelancerJob.id -filepath $spewLocation
        $endTime = Get-Date -Format "yyyy-MM-dd-HH:mm:ss"  
        $hexJobId = ('0x{0:x}' -f $freelancerJobId)
        $err = Get-WinEvent -FilterHashtable @{LogName = 'Application'; ID = 1000 } -MaxEvents 1 | Where-Object -Property Message -Match 'Faulting application name: Freelancer.exe' | Where-Object -Property Message -Match "$hexJobId"
        if ($err) {
            Write-Host "Freelancer BMOD Process with PID $freelancerJobId has terminated unexpectedly at $endTime. Fetching crash event data from Application logs... " -ForegroundColor Red
            Write-Host $err.Message
            Write-Host "For debugging, please reference the Fault offset and Faulting module name against https://wiki.the-starport.net/wiki/fl-binaries/crash-offsets" -ForegroundColor Red
        }
        else {
            Write-Host "BMOD Process with PID $freelancerJobId has terminated at $endTime" -ForegroundColor Green
        }
    }
}
else {
    Write-Host "The BMOD_COPY_PATH environment variable is not set! Aborting..." -ForegroundColor Red
    exit
}