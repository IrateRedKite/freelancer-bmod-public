@echo off
@echo "Restarting the server at %time:~0,2%:%time:~3,2%:%time:~6,2% on %date:~-10,2%-%date:~-7,2%-%date:~-4,4%."  

::@echo "Regenerating DSAM's database at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
::"C:\Freelancer Server Management Tools\DSAccountManager1.4.1\bin\DSAccountManager.exe" -autoclean

@echo "Preserving the server console log at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
copy "C:\freelancer-escalation\EXE\flserver.log" "C:\Users\Administrator\Desktop\Server Logs\console-logs\flserverconsole_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::@echo "Backing up chat and event logs at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\chats-and-events" private-chats.log /mov
::ren "C:\Freelancer Server Management Tools\fl-server-logs\chats-and-events\private-chats.log" "private-chats_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\chats-and-events" events.log /mov
::ren "C:\Freelancer Server Management Tools\fl-server-logs\chats-and-events\events.log" "events_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\chats-and-events" chats.log /mov
::ren "C:\Freelancer Server Management Tools\fl-server-logs/chats.log" "chats_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::@echo "Copying FLHook logs at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs" FLHook.log
::ren "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs\FLHook.log" "FLHook_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs" flhook_socketcmds.log
::ren "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs\flhook_socketcmds.log" "flhook_socketcmds_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

::ROBOCOPY C:\freelancer-escalation\EXE\flhook_logs "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs" flhook_admincmds.log
::ren "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs\flhook_admincmds.log" "flhook_admincmds_%time:~0,2%-%time:~3,2%-%time:~6,2%_%date:~-10,2%-%date:~-7,2%-%date:~-4,4%.log"

@echo "Checking for updates at %time:~0,2%:%time:~3,2%:%time:~6,2%."
cd C:\freelancer-escalation
git pull
@echo "Finished updating at %time:~0,2%:%time:~3,2%:%time:~6,2%."

@echo "Removing console logs older than 1 week at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
ROBOCOPY "C:\Users\Administrator\Desktop\Server Logs\console-logs\console-logs" C:\logs_old /mov /minage:7
::@echo "Removing FLHook logs older than 1 week at %time:~0,2%:%time:~3,2%:%time:~6,2%."  
::ROBOCOPY "C:\Freelancer Server Management Tools\fl-server-logs\flhook-logs" C:\logs_old /mov /minage:7
del C:\logs_old /q

@echo "Finished at at %time:~0,2%:%time:~3,2%:%time:~6,2% on %date:~-10,2%-%date:~-7,2%-%date:~-4,4%."  