A list of tracked changes made to Freelancer.exe, content.dll and and common.dll

# Freelancer.exe

| Address | Original  | Adjusted | New Value (if applicable) | Reason                             |
|---------|-----------|----------|-----------------------|------------------------------------|
| 0D5936  | 0F85      | 90E9     |                       | Unlock visual cap on cruise speed  |
| 0DAE0B  | 4260653C  | 00000000 |                       | Disable shield batteries           |
| 0DAE87  | 96430B3D  | 00000000 |                       | Disable shield batteries           |
| 1D86E0  | 69        | 00       |                       | Disable shield batteries           |
| 0DAE0B  | 4260653C  | 00000000 |                       | Disable nanobots                   |
| 0DAE87  | 96430B3D  | 00000000 |                       | Disable nanobots                   |
| 1D86E0  | 69        | 00       |                       | Disable nanobots                   |
| 1D7964  |           |          | f0.25                 | Update contact list refresh rate   |
| 1D8484  |           |          | f0.25                 | Update weapon panel refresh rate   |
| 02477A  | CA 02     | 00 00    |                       | Removes window borders when Freelancer is running in a window   |
| 02490D  | CA 02     | 00 00    |                       | Removes window borders when Freelancer is running in a window   |


# Common.dll

| Address | Original | Adjusted | New Value (if applicable) | Reason                             |
|---------|----------|----------|-----------------------|------------------------------------|
| 08E86A  | 7E31     | EB39     |                       | Disable Act_PlayerEnemyClamp       |
| 13E52C  | 04       | 00       |                       | Enable NPC countermeasure usage    |
| 03A2B3  | 06       | 09       |                       | Enable mine droppers during cruise |
| 038590  | 83EC345356 | B001C20400 |                       | CEGun::CanSeeTargetObject fix for NPCs |
| 08A185  |          |          | f0.349065989255905151 | Radians value of NPC muzzle cone angle |
| 08AE95  |          |          | f0.349065989255905151 | Radians value of NPC muzzle cone angle |
| 0D670F  | C644243001 | E947FFFFFF |  | Makes suns honour the visit flag (For Houston Glow)

# Content.dll

| Address | Original | Adjusted | New Value (if applicable) | Reason                             |
|---------|----------|----------|-----------------------|------------------------------------|
| 04EE3A  | 422A     | A26A     |                       | Force newplayer.fl to m13.ini      |
