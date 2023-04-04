Freelancer XML Project : Release 2.2 by Jason Hood
			 Release 1.1 by Sir Lancelot

The Freelancer XML Project consists of two utilities for converting the
Freelancer Universal Tree Format (UTF) files into XML files, and vice-versa.
The purpose of these utilities is to provide a better tool set for the
Freelancer modding community for creating and editing UTF files.  These
utilities are intended as a supplement to UTF editors.

The UTFXML utility converts from the Freelancer UTF format files into XML format
to allow you to examine (and modify) the contents of the UTF file with any text
editor or (if you prefer) any XML editor.  During the conversion, the utility
will extract data files, such as .WAV, .TGA, and .DDS files.  The companion
XMLUTF utility converts the XML files back to Freelancer UTF format.

There are three primary advantages to using these utilities in lieu of a UTF
editor.  First, it is much easier to edit an XML file than keying in raw data.
Second, the XML utilities understand the data format for virtually all
Freelancer data types.	Finally, these utilities can be used in batch mode
with recursive processing of folders, rather than working with a single UTF file
at a time.

Version 2 of XMLUTF is NOT compatible with version 1 of UTFXML (in particular,
the order of the values in the VMeshRef bounding box has changed).  Count
attributes are ignored - XMLUTF will process as many entries as given,
automatically setting the count.  (This does not apply to the hex dump's size
attribute.)  Similarly, the "used" and "span" values of VWireData are set
automatically.	To assist with creating new audio entries, the hash attribute
can be used - it generates the hash of its value, using that as the node name.
For example:

	<msg hash="gcs_refer_fc_new_short" .../>

is equivalent to

	<_xBC3D4807 name="0xBC3D4807" .../>

UTFXML will only create timestamps (previously known as unk4 & 5 in the header
and unk234 in the nodes) if it differs from its parent.  Ignoring timestamps
altogether will cause XMLUTF to use the current local time.


COMMAND LINE USAGE

UTFXML [-agitdrcC] [-s strings] [-o path] [file]

	-a	extract audio files
	-g	extract texture files
	-i	extract include files
	-t	include timestamps
	-d	convert radians to degrees
	-r	convert matrix to rotation, quaternion to angle/axis
	-c	convert float RGB (0.0 to 1.0) to integer (0 to 255)
	-C	convert float RGB to hex (#RRGGBB)
	-s	strings file
	-o	path of output file(s)
	file	UTF file to convert (only one allowed)

If FILE is absent, the dialog will start as usual; otherwise it will be quietly
converted (to the current directory if PATH is not given) and the program will
exit.

XMLUTF [-o|O path] [file]

	-o	path of output file, creating subfolders
	-O	path of output file
	file	XML file to convert (only one allowed)

If FILE is absent, the dialog will start as usual; otherwise it will be quietly
converted (within Freelancer's DATA directory if PATH is not given) and the
program will exit.


Release 2.2 fixes converting integer RGB ALE values in XMLUTF.	Prompt for the
path to Freelancer, if the registry key is not found.  Create the entire dest-
ination path.  The logs are written to the temporary directory, adding the
source and destination paths and the summary.  The Alchemy type values have been
given a name (e.g. type="bool" instead of type="0x001").  Animated Pris parts
no longer use degrees and quaternions use angle (in degrees) plus axis (if the
Rotation option is in effect).	Handle command line options.  Do not include
"DATA\" in the path attribute.

Release 2.1 fixes problems with tabs and multi-line comments in XMLUTF.  The RGB
option of UTFXML can generate integer values; the label indicates the method
employed.  Remove the embedded path from VMeshData file names; remove the
timestamp from VWireData names; remove the duplicated timestamp and extension
from some texture files.  Ignore data size when determining node type.

Release 2.0 makes many improvements.  Extracted files are written to a
subdirectory (based on the UTF file name), eliminating the need for an appended
CRC.  Audio and material names can be extracted from the ini and UTF files,
allowing the hash or CRC code to be replaced with its name.  Multi-line comments
are allowed.  UTFXML has options to use degrees instead of radians, rotation
instead of orientation and RGB (#RRGGBB) instead of floats; XMLUTF can read each
type, so they can be freely mixed.

Release 1.1 fixes two problems: missing heads/hands, and extra commas in the
attributes.  The utilities include source code which was built using Microsoft
Visual C++ 6.0.

Please send any suggestions for improvements to me via email.

Enjoy!
Sir Lancelot <elvviis@hotmail.com>
Jason Hood <jadoxa@yahoo.com.au>
