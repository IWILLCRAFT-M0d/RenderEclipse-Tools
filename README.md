# RenderEclipse Tools
## RenderEclipse Extractor
This tool is for extracting data from *.arc files from Climax's Silent Hill games, this covers both Climax LA's Silent Hill: Origins (the infamous RE4-esque version of the game), Climax UK's Silent Hill: Origins (final release) and Silent Hill: Shattered Memories in all platforms and in Shattered Memories case with almost the entirety of the files names (SHSM uses hashed names so there is no way to recover them from the file unlike SHO).

For it usage just open a file and right click on them and you will get the option to extract an specific file, if you right click on the root you can extract all files in one extract. Sorry if the program lag this is my first time doing something like this...

Oh and the GUI is made with [wxWidgets](https://github.com/wxWidgets/wxWidgets)
## tex_SilentHillClimax.py
This file is a plugin for [Noesis](https://www.richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) this plugin is for reading and exporting textures from all Climax's SH Games (PSP, PS2 and Wii)

For it usage you will need this library (https://github.com/Zheneq/Noesis-Plugins/blob/master/lib_zq_nintendo_tex.py), and for extracting data you will need to create a folder named "textures" where you have the *.txd file you are trying to open.

For PS2 and Wii textures I recommend using [Magic.TXD](https://gtaforums.com/topic/851436-relopensrc-magictxd/)
## FBXFtest and DataSplitter
FBXFtest is a barebones reader of the encryption method of XML/XAML files from Silent Hill: Shattered Memories. DataSplitter splits RenderWare files. Just drop the proper file over the exe and it will start working. Anyways this are testing stuff.
## File formats documentation
[Click here to go to a little list of a few poorly documented file formats I did.](FileFormats.md)