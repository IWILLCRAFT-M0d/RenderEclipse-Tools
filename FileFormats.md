# File Formats of Climax's Silent Hill games
This page covers Silent Hill: Origins (Climax Los Angeles [CLA] and Climax United Kingdom [CUK]) and Silent Hill: Shattered Memories. Most file format seems to be third party software, the few propietary file formats that I (IWILLCRAFT) was did found are *.ARC and \*.XAML/\*.XML, second one it's not propetary, but in Silent Hill: Shattered Memories they encrypted it. All their game uses RenderWare version 3.7.0.2 and fortunately I think didn't change or modified the RenderWare file formats.
## *.ARC (File container)
Inside files with this extension are the files used by the games. Each game has little variations of the format. Only SHO (CUK) seems to change between PS2 and PSP by a slight detail. All games compression uses ZLib.

In both SHO (CUK) and SHSM file data has a special space reserved for them, in SHO I wasn't able to now which could be the minimun space reserved, but in Shattered Memories the space is 2048 bytes (2 Megabytes), if the file or sector of the file is bigger than that the size of the space reserved is enlarged by 2 MBs more until it fully covers the file.
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
## *XAML/XML (UI) - Shattered Memories
As per the official Microsoft documentation: ["XAML is a declarative markup language. As applied to the .NET programming model, XAML simplifies creating a UI for a .NET app."](https://learn.microsoft.com/en-us/dotnet/desktop/wpf/xaml/?view=netdesktop-8.0)

Both CLA and CUK Origins uses XML for the interface, while Shattered Memories uses XAML (although a leftover of a level selector, and a file related to subtitles uses XML), but what makes it deserve a whole section is their encryption method which not documented, but is somewhat readable. The investigation was made comparing a non encrypted file and one encrypted, to be specific the file "UI_Bootmenu.xml" which in both Wii and PS2 is not encryped, but in PSP it is encrypted. A further investigation will require read game memory in how the files are unencrypted, but that is far from my knownledge.

Side note: UI_Bootmenu.xml is a leftover from a debug level selector, this is know as this file is also present on Silent Hill: Origins (CUK) and thanks to a [gameplay of a prototype made by Borman (Past To Present Online/PtoPOnline) we can see it in action.](https://youtu.be/HMM1_-eVQhA?si=EgYEu4-muZHWUH8D)
```
(Header)
- unsigned long unknown0
- unsigned long startStringsPos
- char[4] fileSignature "FBXF"
- unsigned long fileSize
```
After this the files doesn't follow an exact pattern, but it contains some recognizable patterns.
The first value is the position of the first tag, if after that value there is any value of the listed below then that means the tag contains an attribute. The next value (unsigned long) will be the position of the string of the name of the attribute, and the next value (unsigned long) will be the position of the string of the value of the attribute, in some cases the attribute redirect to a 0x00 value so its means its empty.
```
0x06800100 (98310)
0x1E800100 (98334)
0x0E800100 (98318)
0x1E000000 (30)
0x0E000000 (14)
```
After this normally reading values is impossible, but there seems to be a pattern where the next 4-byte value is some kind of identifier, and the next value is the position of the value that normally are the tags.
## RenderWare Data
As previously mention, Climax used RenderWare version 3.7.0.2 and it's seem many file formats that comes with RenderWare are used. However most RenderWare files and some extra data are stored inside files, like a file container. [Leeao made a QuickBMS script for extracting exclusively RenderWare files](https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2_Unpack_Resource.bms) which works with all games in both PlayStation systems, but not in Wii (SHSM) because data has to be read as Big Endian while PS Systems are fully read in Little Endian. In this repository an adjusted version of the script can be found that works with Wii (SHSM). What it do is read the first value which contains the size of the file information header, normally the value is not bigger than 600 bytes and as reading Big Endian values as Little Endian normally gives gigantic values I expect it will never be bigger than 1024 bytes (1 Megabyte). Also a little program coded in C++ (code available in the repository) can be found and it fully extracts all data, although somethings it could bug out and cause a memory saturation so I recommend only use it if you want to extract everything from the file.
## \*.snd/\*.mus (Audio) - Silent Hill: Shattered Memories
Shattered Memories uses FMOD as per the official page describe it ["FMOD is an end-to-end solution for adding sound and music to any game. Build adaptive audio using the FMOD Studio and play it in-game using the FMOD Engine."](https://www.fmod.com/studio). Shattered Memories uses version 4. Wii is the console that takes more advantage from it as it uses reverb and has dynamic music which files can be found in data.arc labeled with the extension *.mus, they seem to be just *.xm files with a different file extension. In PSP and PS2 they just use static audio that can be found in igc.arc with the extension *.snd although through many other files more of this audio files can be found.

The *.snd can be extracted with both [FSBExt](https://github.com/gdawg/fsbext) and [vgsmtream](https://github.com/vgmstream/vgmstream). I reccomend use FSBExt as it fully extract all data with the incovenience that it extract them as *.genh files, they can be reproduced from the page of vgsmtream and also coverted to *.wav. However extract it you need to remove the header which is everything that comes before "FSB4" and rename the file to *.fsb.

Side note: In case of extracting files with the Climax's Silent Hill Split RenderWare Resource script, a file with the extension \*.fdp could get extracted. This files are actually \*.snd files, but with the extension of a project file for FMOD's tools.
## Subtitules
Credits to [Bartlomiej Duda.](https://github.com/bartlomiejduda/Tools/blob/master/NEW%20Tools/Silent%20Hill%20Shattered%20Memories/SUB%20file%20format.txt)
```
(Header)
- char[4] fileSignature 0x02000000
- unsigned long stringsNumber

(String information)
read next two values the many strings detected in stringsNumber
- unsigned long fileName (hashed)
- unsigned long stringSize

now read next strings (as UTF-16) the many strings detected in stringsNumber
```
## Extra note: Leeao DFF viewer and why it doesn't work on Wii and PSP
Leeao also made a [model viewer for PS2 (CUK) Silent Hill: Origins](https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2.py), the reason why it doesn't work on PSP and Wii is because of the [Native Data PLG](https://gtamods.com/wiki/Native_Data_PLG_(RW_Section)) which is a section that is customized based on the plaform, unfortunately there is no other tool (to my knowledge) that works PSP and Wii models.

However would be interesting to check out progress on other Leeao project where he covers a [PSP game using RenderWare](https://github.com/leeao/MortalKombat) and a [Blender script for importing models from Iron Heavy Studio games](https://github.com/Psycrow101/DragonFF/tree/native-plg) (Developers of many licensed games like SpongeBob SquarePants: Battle for Bikini Bottom [2003]) made by the user Psycrow101

Must mention: Wii and GameCube shares the same ID so probably a tool made for a GameCube games should support Wii too
## Extra note: Viewing and extracting textures
The PS2 SHO (CUK) script made by Leeao also works with also works with some PS2 SHO prototypes and PS2 Shattered Memories, Leeao also made a [script for general RenderWare PSP game](https://github.com/leeao/Noesis-Plugins/blob/master/Textures/tex_rw_psp_txd.py) which is unfinished and sometimes it doesn't work, and also the user Zhenёq made a script for [Wii Shattered Memories](https://github.com/Zheneq/Noesis-Plugins/blob/master/fmt_silenthill_wii_tx.py) which doesn't work sometimes too, but is really rough so I mixed all of them in one script which support and extract textures from PSP, PS2 and Wii games.

For extracting data you will need to create a folder named "textures" where you have the *.txd file you are trying to open and the script requires this library: https://github.com/Zheneq/Noesis-Plugins/blob/master/lib_zq_nintendo_tex.py.

The script still having issues when reading small textures from PS2 and Wii.

Must mention: the tool [Magic.TXD](https://gtaforums.com/topic/851436-relopensrc-magictxd/) works with PS2 and Wii textures.