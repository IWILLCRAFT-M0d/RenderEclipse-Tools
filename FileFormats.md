# File Formats of Climax's Silent Hill games
This page covers some of the file formats from Silent Hill games Develop by Climax (Silent Hill: Origins [Both Climax Los Angeles {CLA} Prototypes and Climax United Kingdom {CUK} versions] and Silent Hill: Shattered Memories.)

The main focus of this research are Climax's Silent Hill games however some of the findings can be applied for the 2007's Ghost Rider game, Overlord: Dark Legend and the cancelled PSP version of The Elders Scrolls IV: Oblivion.

This file format covered in this page:
* <a href="#arc-file-container">\*.ARC (File Containers)</a>
* <a href="#xamlxml-ui---shattered-memories">\*.XAML/\*.XML Obfuscation (UI - Silent Hill: Shattered Memories)</a>
* <a href="#renderware-stream-files">RenderWare Stream Files (File Containers)</a>
* <a href="#sndmus-audio---silent-hill-shattered-memories">\*.SND/\*.MUS (Audio - Silent Hill: Shattered Memories)</a>
* <a href="#string-table">String Files (Text)</a>
* <a href="#videos-file-format">Video File Format</a>

## *.ARC (File container)
Inside files with this extension are the files used by the games. Each game has little variations of the format. Only SHO (CUK) seems to change between PS2 and PSP by a slight detail. All games compression uses ZLib.

In both SHO (CUK & CLA) and SHSM file data has a special space reserved for them, in SHO cases I wasn't able to figure out which could be the minimun space reserved, likely it is between 16 or 32 bytes, but in Shattered Memories the space is 2048 bytes (2 Megabytes), if the file or sector of the file is bigger than that the size of the space reserved is enlarged by 2 MBs more until it fully covers the file.
### *.ARC - Silent Hill: Origins (CLA)
```
(Header)
- unsigned long fileCount
- unsigned long startDataPos
- unsigned long namesPos
- unsigned long namesSize

(File information)
read next four values the many files detected in fileCount
- unsigned long fileNamePos + namesPos
- unsigned long fileDataPos + startDataPos
- unsigned long fileSize
- unsigned long fileSizeUncompressed (if this value is zero then it not compressed)
```
### *.ARC - Silent Hill: Origins (CUK)
Credits to Rheini from XeNTaX forums for documenting file format.
```
(Header)
- char[4] fileSignature "A2.0"
- unsigned long fileCount
- unsigned long startDataPos
- unsigned long namesPos
- unsigned long namesSize

(File information)
read next four values the many files detected in fileCount
- unsigned long fileNamePos + namesPos
- unsigned long fileDataPos
- unsigned long fileSize
- unsigned long fileSizeUncompressed (if this value is zero then it not compressed)

After reading all file data information in PS2 *.arc, two 4-bytes which are unknown their reason.
```
### *.ARC - Silent Hill Shattered: Memories
Credits to AlphaTwentyThree and TPU from XeNTaX forums for documenting file format and hashing method.

For some reason this is the only game that the file information table is not used, but instead it uses a file information table embed inside the main game executable.
```
(Header)
- 4 bytes fileSignature 0x10FA0000
- unsigned long fileCount
- unsigned long startDataPos
- unsigned long empty (unknown)

(File information)
read next four values the many files detected in fileCount
- unsigned long fileName (hashed)
- unsigned long fileDataPos
- unsigned long fileSize
- unsigned long fileSizeUncompressed (if this value is zero then it not compressed)
After reading all file data information, two 4-bytes which are unknown their reason.

The hashing method with example (written in Python):
"FontJAP" -> "fontjap"
def calc_arc_hash("fontjap"):
    hash = 0
    for i in range(len(in_str)):
        hash = ((hash * 33) ^ ord(in_str[i]))
    return hex(hash & 0xFFFFFFFF)
The result will be 4131116712 (0xf63bd6a8)
```
## \.*XAML/\.*XML (UI) - Shattered Memories
As per the official Microsoft documentation: ["XAML is a declarative markup language. As applied to the .NET programming model, XAML simplifies creating a UI for a .NET app."](https://learn.microsoft.com/en-us/dotnet/desktop/wpf/xaml/?view=netdesktop-8.0)

Both CLA and CUK Origins uses \*.XML for some of the interfaces, while Shattered Memories uses \*.XAML and some files uses \*.XML, but what makes it deserve a whole section is the obfuscation method. A further investigation will require to read game functionality, but that is far from my (IWILLCRAFT) knownledge.

Side note: UI_Bootmenu.xml is a leftover from a debug level selector, this is know as this file is also present on Silent Hill: Origins (CUK) and thanks to a [gameplay of a prototype made by Borman (Past To Present Online/PtoPOnline) we can see it in action.](https://youtu.be/HMM1_-eVQhA?si=EgYEu4-muZHWUH8D)
```
(Header)
- unsigned long unknown0
- unsigned long startStringsPos
- char[4] fileSignature "FBXF"
- unsigned long fileSize
```
After this the files doesn't follow an exact pattern, but it contains some recognizable patterns.
The first value is the position of the first tag, if after that value there is any value of the listed below then that means the tag contains an attribute. The next value (unsigned long) will be the position of the string of the name of the attribute, and the next value (unsigned long) will be the position of the string of the value of the attribute, in some cases the attribute redirect to a 0x00 value meaning that the attribute it is empty.
```
0x06800100 (98310)
0x1E800100 (98334)
0x0E800100 (98318)
0x1E000000 (30)
0x0E000000 (14)
```
After this normally reading values is impossible, but there seems to be a pattern where the next 4-byte value is some kind of identifier, and the next value is the position of the value that normally are the tags.
## RenderWare Stream Files
As previously mention, Climax used RenderWare version 3.7.0.2 and it seem that many of the file formats that comes with base RenderWare are used. However most RenderWare files and some extra data are stored inside files, like a file container. [Leeao made a QuickBMS script for extracting exclusively RenderWare files](https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2_Unpack_Resource.bms) which works with all games in both PlayStation systems, but not in Wii (SHSM) because data has to be read as Big Endian while PS systems are fully read in Little Endian. [In this repository an adjusted version of the script can be found that works with Wii (SHSM).](https://github.com/IWILLCRAFT-M0d/RenderEclipse-Tools-Priv/blob/main/Extra%20tools%20and%20scripts/ClimaxSH_Unpack_Resource.bms) What it do is read the first value which contains the size of the file information header, normally the value is not bigger than 600 bytes and as reading Big Endian values as Little Endian normally gives gigantic values I expect it will never be bigger than 1024 bytes (1 Megabyte). Also a little program coded in C++ (code available in the repository) can be found and it fully extracts all data, although it is not recommended to use with the May 2006 Silent Hill: Origins prototype as it may not work due all files having in the start a 192 byte sector full of undetectable giberish data which may cause the tool to not properly run causing either the tool to crash or a memory saturation.
## \*.snd/\*.mus (Audio) - Silent Hill: Shattered Memories
Shattered Memories uses FMOD as per the official page describe it ["FMOD is an end-to-end solution for adding sound and music to any game. Build adaptive audio using the FMOD Studio and play it in-game using the FMOD Engine."](https://www.fmod.com/studio). Shattered Memories uses version 4. Wii is the console that takes more advantage from it as it uses reverb and has dynamic music which files can be found in data.arc labeled with the extension \*.mus, they are simply \*.xm files with a different file extension and a extra 72 bytes long header. In PSP and PS2 they only use static audios, most of them can be found in igc.arc with the extension \*.snd.

The \*.snd can be extracted with both [FSBExt](https://github.com/gdawg/fsbext) and [vgsmtream](https://github.com/vgmstream/vgmstream). I reccomend use FSBExt as it fully extract all audio files with the incovenience that it extract them as \*.genh or \*.ss2 files, they can be reproduced from the page of vgsmtream and also coverted to \*.wav. However extract it requires to remove the header which is everything that comes before "FSB4" and rename the file to \*.fsb.

The \*.mus files can be open with most players that supports \*.xm files (a convenient one being VLC), but the first 72 bytes of the file has to be deleted in other to play them.

Side note: in case of extracting files with the Climax's Silent Hill Split RenderWare Resource BMS script, a file with the extension \*.fdp could get extracted. This files are actually \*.snd files, but with the extension of a project file for FMOD's tools.
## String Table
Credits to [Bartlomiej Duda.](https://github.com/bartlomiejduda/Tools/blob/master/NEW%20Tools/Silent%20Hill%20Shattered%20Memories/SUB%20file%20format.txt)
```
(Header)
- char[4] fileSignature 0x02000000
- unsigned long stringsNumber

(String table information)
read next two values the many strings detected in stringsNumber
- unsigned long stringName (hashed)
- unsigned long stringPosition (stringPosition + string table information size + header size)

After reading the string table information data starts. Strings are UTF-16 encoded.
```

Similar to Shattered Memories having the filenames hashed in the \*.ARC all Climax's SH games has string tables uses hashed IDs from strings. The hashing method variates from in each game (CLA and CUK Origins, and SHSM). For now the only one having been able to fully recreated has been the one used on SHSM.

```
def calc_sub_hash(in_str):
    hash = 5381
    for i in range(len(in_str)):
        hash = (hash * 0x21) + (ord(in_str[i]) & 0xdf)
    return hex(hash & 0xFFFFFFFF)


print(calc_sub_hash("igc02_doctork_01a"))
The result will be 484287472 (0xf0a3dd1c)
```

## Videos File Format
This page won't cover in detail the file formats as they are Third-Party based and has been extensively covered and even made compatible with some video players like VLC so here it will only list the format and versions.

PSP games uses "PlayStation Portable Movie Format" also known as "PSMF" made by Sony. Silent Hill: Origins uses version 14 while Shattered Memories uses version 15.

PS2 games uses "PSS" a PS2 video and audio file format made by Sony. All games uses it.

Wii Shattered Memories uses "THP" file format made by Nintendo. The version used by the game is the 0x00011000.