# RenderEclipse Extractor
This tool is for extracting data from *.ARC files from Climax's Silent Hill games, this covers both Climax LA's Silent Hill: Origins (the infamous RE4-esque version of the game), Climax UK's Silent Hill: Origins (final release) and Silent Hill: Shattered Memories in all platforms.

In Shattered Memories case it contains almost the entirety of the files names unhashed has it uses hashed names and this process makes impossible to directly recover file names, unlike Origins where at the end of the file it contains all the file names.

For it usage just open a file and right click on them and you will get the option to extract an specific file, if you right click on the root you can extract all files in one extract. Sorry if the program lag this is my first time doing something like this...

The GUI is made with [wxWidgets](wxWidgets/wxWidgets)
# tex_SilentHillClimax.py
This file is a plugin for [Noesis](https://www.richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) this plugin read and extract textures from all Climax's SH Games from all platforms, however, this tool doesn't work properly under some textures from PS2 versions and Wii Shattered Memories.

For it usage you will need this library (https://github.com/Zheneq/Noesis-Plugins/blob/master/lib_zq_nintendo_tex.py), and for extracting data you will need to create a folder named "textures" in the same folder you have the *.txd file you are trying to open.

Fortunately for PS2 and Wii textures we have [Magic.TXD](https://gtaforums.com/topic/851436-relopensrc-magictxd/) which also works for modify them.
# FBXFtest, DataSplitter and RETH
FBXFtest is a barebones reader of the encryption method of XML/XAML files from Silent Hill: Shattered Memories.

DataSplitter splits RenderWare files. Just drop the proper file over the exe and it will start working. It's recommended using the QuickBMS script until this get implemented into the RenderEclipse Extractor as this may cause a memory saturation crashing the user PC or it will just not work

RETH is a tool made for adding values to the Shattered Memories hashing table. For it usage, make two txt files in the same folder where the executable it is named "Filenames.txt" and "ARCS.txt", in the first one put all names you want to try to check if they could or not be the name of the missing unhashed files and in the second txt file put the directory of your ARC file.
# File formats documentation
[Click here to go to a little list of a few poorly documented file formats I did.](FileFormats.md)
# Cam Hacks
The user "Moonless Formless" made a Cam Hack for the American PS2 versions of Silent Hill: Origins and Shattered Memories

Link: https://github.com/moonlessformless/ps2_cam_acolyte