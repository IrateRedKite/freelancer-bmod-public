echo "Stopping Freelancer and FLServer"
Get-Process "freelancer" | Stop-Process
Get-Process "flserver" | Stop-Process

echo "Downloading latest update"
#git checkout main
$git =  "https://github.com/TheStarport/freelancer-bmod.git"
git pull $git


#echo "Starting the game"
Start-Process -FilePath "EXE/Freelancer.exe" -ArgumentList "-s162.248.93.98:2302 -w"

Read-Host -Prompt "Press Enter to close this console window"