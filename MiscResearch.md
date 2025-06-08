Interesting or curious information unrelated to file formats.


# Codes
Addresses in the game executable made as codes for emulators. PSP codes are intented to be added as CWCheat codes, the same with PS2 codes, but as \*.pnach and Wii codes are intented to be added as Gecko Codes.

## Image codes

### No noise filter codes (SH:SM)
The easiest way to find the address values is to boot the game, search into the memory for a 0x00 value, the press start and get into the main menu, search for a 0x01 value and then restart the game, do the same and you should try playing with some of the values that should be 0x01 changing them into 0x00 and you should have find the static address of the value that disable and enable the noise filter.

<table align=center>
    <tbody>
        <tr>
            <td align=center><b>Regional version</b></td>
            <td align=center><b>PSP</b></td>
            <td align=center><b>PS2</b></td>
            <td align=center><b>Wii</b></td>
        </tr>
        <tr>
            <td align=center><b>NTSC</b></td>
            <td rowspan=2 align=center><b><code>_C0 No noise
_L 0x0034B002 0x00</code></b></td>
            <td align=center><b><code>patch=1,EE,2047ABC0,extended,00000000</code></b></td>
            <td align=center><b><code>00528842 00000000</code></b></td>
        </tr>
            <td align=center><b>PAL</b></td>
            <td align=center><b><code>patch=1,EE,2047ADC0,extended,00000000</code></b></td>
            <td align=center><b><code>0052A5D2 00000000</code></b></td>
        </tr>
            <td align=center><b>NTSC-J</b></td>
            <td align=center><b><code>_C0 No noise
_L 0x0034AF22 0x00</code></b></td>
            <td align=center><b><code>patch=1,EE,2047B0C0,extended,00000000</code></b></td>
            <td align=center><b><code>0052AAD2 00000000</code></b></td>
        </tr>
    </tbody>
</table>

### No black borders code (SH:SM)
This cheat replaces the code in the function that asign the value used by the cutscenes to enable the black borders to be 1 to instead always put 0. It's possible that this code may not work on NTSC-J PSP and Wii and PAL Wii.

<table align=center>
    <tbody>
        <tr>
            <td align=center><b>Regional version</b></td>
            <td align=center><b>PSP</b></td>
            <td align=center><b>PS2</b></td>
            <td align=center><b>Wii</b></td>
        </tr>
        <tr>
            <td align=center><b>NTSC</b></td>
            <td rowspan=2 align=center><b><code>_C0 No black borders<br/>
_L 0x0009C6D8 0x0000043C<br/>
_L 0x0009C6DC 0x0000043C</code></b></td>
            <td align=center><b><code>patch=1,EE,0019E3B0,word,3C010000</code></b></td>
            <td align=center><b><code>0452c3bc 00000000
0452c3c0 00000000
04528842 00000000</code></b></td>
        </tr>
            <td align=center><b>PAL</b></td>
            <td align=center><b><code>patch=1,EE,0019E3D0,word,3C010000</code></b></td>
            <td align=center><b><code>046797B8 00000000</code></b></td>
        </tr>
            <td align=center><b>NTSC-J</b></td>
            <td align=center><b></b></td>
            <td align=center><b><code>patch=1,EE,0019E400,word,3C010000</code></b></td>
            <td align=center><b><code>04679CB8 00000000</code></b></td>
        </tr>
    </tbody>
</table>

## Access to unused maps/cutscenes

### Execute Intro IGC on New Save
<table align=center>
    <tbody>
        <tr>
            <td align=center><b>Regional version</b></td>
            <td align=center><b>PSP</b></td>
            <td align=center><b>PS2</b></td>
            <td align=center><b>Wii</b></td>
        </tr>
        <tr>
            <td align=center><b>NTSC</b></td>
            <td align=center><b><code>_C0 Execute Intro IGC on New Save<br/>
_L 0x20327CF4 0x30434749<br/>
_L 0x20327CF8 0x00000061<br/>
_L 0x20327D00 0x00000000<br/>
_L 0x20327D10 0x30434749<br/>
_L 0x20327D14 0x47495F61<br/>
_L 0x20327D18 0x00623043</code></b></td>
            <td align=center><b><code>patch=1,EE,0053e090,word,00000000<br/>
patch=1,EE,0053e080,word,30434749<br/>
patch=1,EE,0053e084,word,00000061<br/>
patch=1,EE,0053e0a0,word,30434749<br/>
patch=1,EE,0053e0a4,word,47495F61<br/>
patch=1,EE,0053e0a8,word,00623043</code></b></td>
            <td align=center><b><code>043E1BC4 69676330<br/>
043E1BC8 61000000<br/>
043E1BD4 69676330<br/>
043E1BD8 615F6967<br/>
043E1BDC 63306200</code></b></td>
        </tr>
    </tbody>
</table>

### Execute Ending Cutscenes IGCs on New Save
<table align=center>
    <tbody>
        <tr>
            <td align=center><b>Regional version</b></td>
            <td align=center><b>PSP</b></td>
            <td align=center><b>PS2</b></td>
            <td align=center><b>Wii</b></td>
        </tr>
        <tr>
            <td align=center><b>NTSC</b></td>
            <td align=center><b><code>_C0 Execute Ending Cutscenes IGCs on New Save<br/>
_L 0x20327CF4 0x34434749<br/>
_L 0x20327CF8 0x00006239<br/>
_L 0x20327D00 0x00000000<br/>
_L 0x20327D10 0x34434749<br/>
_L 0x20327D14 0x495F6139<br/>
_L 0x20327D18 0x39344347<br/>
_L 0x20327D1C 0x00000062</code></b></td>
            <td align=center><b><code>patch=1,EE,0053e090,word,00000000<br/>
patch=1,EE,0053e080,word,34434749<br/>
patch=1,EE,0053e084,word,00006139<br/>
patch=1,EE,0053e0a0,word,34434749<br/>
patch=1,EE,0053e0a4,word,495F6439<br/>
patch=1,EE,0053e0a8,word,39344347<br/>
patch=1,EE,0053e0aC,word,00000061</code></b></td>
            <td align=center><b><code>043E1BC4 69676334<br/>
043E1BC8 39610000<br/>
043E1BD4 69676334<br/>
043E1BD8 39645F69<br/>
043E1BDC 67633439<br/>
043E1BE0 61000000</code></b></td>
        </tr>
    </tbody>
</table>