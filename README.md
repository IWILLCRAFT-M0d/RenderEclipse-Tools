<h1 align="center">
  <a>RenderEclipse Tools</a>
  <img src="./Images/sprog.ico" width="45" align="top">
</h1>

<h4 align="center">A toolset and documentation for reverse-engineer Climax's Silent Hill Games</h4>

<p align="center">
  <a href="#rendereclipse-tools">RenderEclipse Tools</a> •
  <a href="#external-tools">External Tools</a> •
  <a href="https://github.com/IWILLCRAFT-M0d/RenderEclipse-Tools-Priv/tree/archive">Archive</a> •
  <a href="./FileFormats.md">File Format Documentation</a>
</p>

<p align="center">
  <img src='./Images/sh0dogotravis.gif'>
</p>

# Introduction
This repository contains tools and documentation specifically made for Climax's Silent Hill games (Silent Hill: Origins [both Climax Los Angeles and Climax U.K versions] and Silent Hill: Shattered Memories) alongside tools that works with the games.

## <a href="https://github.com/IWILLCRAFT-M0d/RenderEclipse-Tools-Priv/releases">RenderEclipse Tools</a>
This tool is for extracting data from \*.ARC files from all Climax's Silent Hill games.

Silent Hill: Shattered Memories hash the filenames (unlike Silent Hill: Origins), the tool features an extensive list with most of unhashed filenames.

For it usage open an \*.ARC file through the menubar. For now it only extract files, for extract a singular file left click on one of the files and extract it, if you want to extract all files left click on the root element of the list and select extract all files.

Sorry if the program lag this is my first time doing something like this...

## <a href="./Extra tools and scripts/tex_SilentHillClimax.py">tex_SilentHillClimax.py</a>
This plugin for [Noesis](https://www.richwhitehouse.com/index.php?content=inc_projects.php&showproject=91) previews and extract textures from Climax's SH Games from all platforms, however this tool does not work properly under some textures that are less than 32 pixel of height and/or width.

For it usage it is required to download this library (https://github.com/Zheneq/Noesis-Plugins/blob/master/lib_zq_nintendo_tex.py), and for extracting data you will need to create a folder named "textures" in the same folder you have the \*.txd file you are trying to open.

## <a href="./Extra tools and scripts/DataSplitter.exe">DataSplitter</a>
DataSplitter.exe is a CLI tool that splits RenderWare Stream files. For it usage drag and drop the proper file over the *.exe and it will start working.

## <a href="./Extra tools and scripts/FBXFtest.exe">FBXFtest</a>
FBXFtest.exe is a CLI barebones reader of the obfusctation method of XML/XAML files from Silent Hill: Shattered Memories. For it usage drag and drop the proper file over the *.exe and it will start working.

## <a href="./Extra tools and scripts/RETH.exe">RETH.exe</a>
RETH.exe is a CLI tool made for detecting and adding values to a \*.RETH file (Silent Hill: Shattered Memories hashing table for RenderEclipse Tool).

For it usage, make two txt files in the same folder where the executable is name as "Filenames.txt" and "ARCS.txt". In "Filenames.txt" put all names you want to check if they could or not be the name of the missing unhashed files. In "ARCS.txt" put the directory of your ARC file, in case of not creating this file it will add any name put in "Filenames.txt".

## <a href="./Extra tools and scripts/ClimaxSH_Unpack_Resource.bms">Fixed RenderWare Stream Quick BMS Script</a>
This script is a adjusted version from [this script](https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2_Unpack_Resource.bms), it comes with the benefit of properly extract Wii's Silent Hill: Shattered Memories RenderWare Stream files.

# External tools and scripts
Tools and scripts made by people unrelated to this repository.
* <a href="https://gtaforums.com/topic/851436-relopensrc-magictxd/">Magic.TXD</a> a tool for extracting and replacing RenderWare textures that fully works with PS2 and Wii textures.
* QuickBMS scripts
  * \*.ARC Scripts
    * [Silent Hill: Origins (Climax U.K)]()
    * [Silent Hill: Shattered Memories](https://aluigi.altervista.org/bms/silent_hill_origins.bms)
  * [RenderWare Stream Extractor (PS2 & PSP games only)](https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2_Unpack_Resource.bms)
* <a href="https://github.com/moonlessformless/ps2_cam_acolyte">ps2_cam_acolyte (PS2 Origins + Shattered Memories Camhack)</a> is a program that let the user freely move the camera from any PS2 game of the supported list. It support the American versions of Silent Hill: Origins and Shattered Memories.
* <a href="https://github.com/XeNTaXTools/XeNTaXTools-Legacy/blob/main/archive/xntx_3177_SHSM_ARCUnpacker_1.0.0.rar">SHSM ARCUnpacker</a> a tool for extract files from Silent Hill: Shattered Memories and the first tool that is able to detect unhashed files.
  * [An updated file list can be downloaded from here.](https://github.com/XeNTaXBackup/XeNTaXBackup.github.io/blob/main/file/6218_SHSM_Projects_0.2.rar)
* <a href="https://github.com/leeao/SilentHillOriginsPS2/blob/main/fmt_SilentHillOrigins_PS2.py">Texture, Model and Animation viewer for PS2 Silent Hill: Origins.</a>
  * It works with Silent Hill: Shattered Memories, however, models may not be previewed all the time and animations could get bugged.
* <a href="https://github.com/leeao/Noesis-Plugins/blob/master/Textures/tex_rw_psp_txd.py">A partially working RenderWare Textures (PSP) viewer.</a>
* <a href="https://github.com/zealottormunds/SH0Modding">Silent Hill: Origins Modding Repository</a> is a GitHub repository about extracting and modding Silent Hill: Origins (Climax U.K). The repository featuring a custom tool (similar to RenderEclipse Tool) that can extract and import files from Silent Hill: Origins (Climax U.K) and can modify RenderWare Stream files from any Silent Hill: Origins version.
* <a href="https://github.com/Sparagas/Silent-Hill">Silent Hill Reverse Engineering Repository</a> is a repository which documents tools and file format from the whole Silent Hill franchise. This repository features:
  * List of demos, related GitHub repositories and pages that document file format from Silent Hill games (all of them splitted depending on the game)
  * [A Noesis that script partially previews Silent Hill: Origins (Climax U.K) and Silent Hill: Shattered Memories maps (\*.bsp)](https://github.com/Sparagas/Silent-Hill/blob/main/Noesis%20-%20Python%20Plugins/fmt_renderware_ps2_bsp.py)
  * [A 010 Template for Silent Hill: Origins (Climax U.K) and Silent Hill: Shattered Memories maps (\*.bsp)](https://github.com/Sparagas/Silent-Hill/blob/main/010%20Editor%20-%20Binary%20Templates/rw_bsp.bt)
    * This 010 Template has 3 dependencies: [inc_rw.bt](https://github.com/Sparagas/Silent-Hill/blob/main/010%20Editor%20-%20Binary%20Templates/inc_rw.bt), [vector.bt](https://github.com/Sparagas/Silent-Hill/blob/main/010%20Editor%20-%20Binary%20Templates/vector.bt) and [color.bt](https://github.com/Sparagas/Silent-Hill/blob/main/010%20Editor%20-%20Binary%20Templates/color.bt)
* <a href="https://github.com/bartlomiejduda/Tools/tree/master/NEW%20Tools/Silent%20Hill%20Shattered%20Memories">A repository that contains documentation and some tools for Silent Hill: Shattered Memories</a>