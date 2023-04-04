CRCTool v1.00
=============

What the hell is it?
--------------------

CRCTool scans your Frelancer directory for ini, cmp, 3db and mat files and
creates a searchable database of names and corresponding CRC and hash codes.


Why do I want do do that?
-------------------------

CRCs are used internally by Freelancer to reference things.  Up till now CRC was
a one way process name -> hash and no going back.  Building on the string table
idea in Colin Sanby's SUR Importer code this program was born.


Fixes/Updates since Beta 3 r4 release
-------------------------------------

Added "name" (for fuses) and "path_label" (in zones) to the INI Search Strings
list.  Include "Root" for cmp files.  Use Data Base File when creating the DB.
Ignore case in file names.  Leave the search results at the start of the list.

A *lot* quicker (no need for the ini progress display or search progress bar).


Fixes/Updates since Beta 3 r3 release
-------------------------------------

Option to generate a case-sensitive CRC (needed by the Alchemy .ale files).

Update ini progress every 1000 lines, rather than every 100.


Fixes/Updates since Beta 3 r2 release
-------------------------------------

Added a signature to the start of the database, so a rescan will be required.
Scan .vms files, too.  Fix reading ini names that should not have matched
(nickname_plurality) and values that contain a space.

Search loads the database into memory - the first search is slow, subsequent
searches are near instant.  The results window will only show the name once.
The search and generator fields will only use lower case.

Generate the CRC and hash as the name is entered.  Include the signed values.

About box via right-click menu item.


Fixes/Updates since Beta 2 release
----------------------------------

This version by Jason Hood.

New database format - create it afresh!  Reduced the size of the database by
storing the data directory at the beginning, with all other file names relative
to it.	The binary database only stores each file name once.

Fixes a bug generating the hash for names in UTF files.  Removes duplicated
names within a file.

Generate codes for hardpoints (starting with "Hp") and damage points ("Dp") in
UTF files, as well as all "Material library" names.  Generate additional codes
by removing the extension (for example: "bw_elite_lod1020911031436.3db" -->
"bw_elite_lod1020911031436"), timestamp ("bw_elite_lod1") and LOD suffix
("bw_elite").  Of course, this results in a lot of duplicated names, but a more
complete database.

Provide a progress display for ini files; improved its speed a bit.

Allow the scan to be cancelled.

Combined the Dec CRC & Hex CRC searches into one, as well as allowing Enter to
do the search.	Search results indicate whether it is a CRC or Hash code.
Added a progress bar.

The hex generator output will prefix with "0x" and use eight digits.

Built with the trial version of C++Builder 2010.


Fixes/Updates since Beta 1 release
----------------------------------

The database now searches FL hash (ioncross type) and UTF id CRCs (model file
internals).  To my shame I'd missed the difference :o. Thanks to Alan 'STrail'
Fowler and John 'Louva-Deus' Turner for getting me past this stupidity :D

Ability to save DB as a plain text file.

Multiline results output particularly suited to searching for audio references.

Results show which files the references were found in.

The names of utf file internals are now added properly (typo corrected).

This is now the tool I wanted it to be :)


Credits
-------

* Mario 'HCl' Brito - UTF file structures
* Colin Sanby - SUR Importer source code that provided the crc gen class.
* Argh (Greg Wolfe) - Destruct testing everything :D
* Alan 'STrail' Fowler & John 'Louva-Deus' Turner for showing me the hideous
  mistake I made.


License
-------

This program is free and open source (yes I appreciate the irony that I made it
on a non-free compiler).  Do what the hell you please with it, but don't call
it your own work.  Anyone trying to make profit from this software will be
tutted at by an elderly aunt.

I take no responibility for any loss, damage, ill-health, injury, death or
stampeding goats that occur during use of this program.


Problems/Bug Reports/Whatever
-----------------------------

mail : hitchhiker54@yahoo.com
TLR forums : Anton

mail : jadoxa@yahoo.com.au
TLR & The Starport forums: adoxa

Cheers, Tony & Jason
