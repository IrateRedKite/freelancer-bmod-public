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

Write-Output "Downloading latest update"
#git checkout main
$git =  "https://github.com/TheStarport/freelancer-bmod.git"
git pull $git

#echo "Starting the game"
Start-Process -FilePath "EXE/Freelancer.exe" -ArgumentList "-s162.248.93.98:2302 -w"

Remove-Variable freelancer
Remove-Variable flserver
