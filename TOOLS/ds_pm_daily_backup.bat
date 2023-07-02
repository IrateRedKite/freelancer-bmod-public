@echo off
@echo "Starting backup job at %time:~0,2%:%time:~3,2%:%time:~6,2% on %date:~-10,2%-%date:~-7,2%-%date:~-4,4%."  
@echo "Backing up files to archive."  
"C:\Program Files (x86)\7-Zip\7z.exe" a "C:\Freelancer Server Management Tools\fl-server-backups\flserverbackup_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.7z" "C:\Users\Administrator\Documents\My Games\Freelancer\Accts\MultiPlayer"

@echo "Removing backups older than 1 month."  
ROBOCOPY "C:\Freelancer Server Management Tools\fl-server-backups" C:\backups_old /mov /minage:30
del C:\backups_old /q

@echo "Finished backup job at %time:~0,2%:%time:~3,2%:%time:~6,2% on %date:~-10,2%-%date:~-7,2%-%date:~-4,4%."  