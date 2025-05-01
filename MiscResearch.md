Less relevant or less important reseach, but interesting or curious information.


# Codes
Addresses in the game executable made as codes for emulators. PSP codes are intented to be added as cheats codes, the same with PS2 codes, but as \*.pnach and Wii codes are intented to be added as Gecko Codes.

## No noise filter codes (SH:SM)
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
            <td rowspan=2 align=center><b><code>_C1 No noise
_L 0x0034B002 0x00</code></b></td>
            <td align=center><b><code>patch=1,EE,2047ABC0,extended,00000000</code></b></td>
            <td align=center><b><code>00528842 00000000</code></b></td>
        </tr>
            <td align=center><b>PAL</b></td>
            <td align=center><b><code>patch=1,EE,2047ADC0,extended,00000000</code></b></td>
            <td align=center><b><code>0052A5D2 00000000</code></b></td>
        </tr>
            <td align=center><b>NTSC-J</b></td>
            <td align=center><b><code>_C1 No noise
_L 0x0034AF22 0x00</code></b></td>
            <td align=center><b><code>patch=1,EE,2047B0C0,extended,00000000</code></b></td>
            <td align=center><b><code>0052AAD2 00000000</code></b></td>
        </tr>
    </tbody>
</table>

# No black borders code (SH:SM)
On PS2 it replace a value in the function that orders the value used by the cutscenes to enable the black borders to be 1 to instead always put 0. On Wii it grabs what seems to be a static value that is allocated onces the game is boot and forces the value to always be 0.

<table align=center>
    <tbody>
        <tr>
            <td align=center><b>Regional version</b></td>
            <td align=center><b>PS2</b></td>
            <td align=center><b>Wii</b></td>
        </tr>
        <tr>
            <td align=center><b>NTSC</b></td>
            <td align=center><b><code>patch=1,EE,0019E3B0,word,3C010000</code></b></td>
            <td align=center><b><code>04678758 00000000</code></b></td>
        </tr>
            <td align=center><b>PAL</b></td>
            <td align=center><b><code>patch=1,EE,0019E3D0,word,3C010000</code></b></td>
            <td align=center><b><code>046797B8 00000000</code></b></td>
        </tr>
            <td align=center><b>NTSC-J</b></td>
            <td align=center><b><code>patch=1,EE,0019E400,word,3C010000</code></b></td>
            <td align=center><b><code>04679CB8 00000000</code></b></td>
        </tr>
    </tbody>
</table>