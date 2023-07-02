$freelancer = Get-Process freelancer -ErrorAction SilentlyContinue
if ($freelancer){
Write-Output "Stopping any instances of Freelancer.exe"
Get-Process "freelancer" | Stop-Process
}
$flserver = Get-Process flserver -ErrorAction SilentlyContinue
if ($flserver){
Write-Output "Stopping any instances of flserver.exe"
Get-Process "flserver" | Stop-Process
}

Write-Output "Installing git"
WinGet install Git.Git

Write-Output "Installing VC Redist dependencies"
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://vcredist.com/install.ps1'))

Write-Output "Downloading the mod"
$appdata = $env:APPDATA
Set-Location $appdata 
git clone https://github.com/TheStarport/freelancer-bmod.git

Write-Output "Installing fonts"
$fonts = (New-Object -ComObject Shell.Application).Namespace(0x14)
foreach ($file in Get-ChildItem *.TTF)
{
    $fileName = $file.Name
    if (-not(Test-Path -Path "C:\Windows\fonts\$fileName" )) {
        Write-Output $fileName
        Get-ChildItem $file | ForEach-Object{ $fonts.CopyHere($_.fullname) }
    }
}
Copy-Item *.TTF c:\windows\fonts\

Write-Output "Creating shortcut on the Desktop"

Copy-Item "freelancer-bmod\Freelancer BMOD.lnk" "$Home\Desktop\Freelancer BMOD.lnk"

Remove-Variable freelancer
Remove-Variable flserver