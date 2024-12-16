Less irrelevant or less important reseach, but interesting or curious information.


# No noise codes (SH:SM)

Addresses in the game executable made for emulators. The easiest way to find them is to boot the game, search into the memory for a 0x00 value, the press start and get into the main menu, search for a 0x01 value and then restart the game, do the same and you should try playing with some of the values that should be 0x01 changing them into 0x00 and you should have find the static address of the value that disable and enable the noise filter. Wii address should be put in Dolphin patches as 8-bit value.


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
            <td align=center><b><code>80528842</code></b></td>
        </tr>
            <td align=center><b>PAL</b></td>
            <td align=center><b><code>patch=1,EE,2047ADC0,extended,00000000</code></b></td>
            <td align=center><b><code>8052A5D2</code></b></td>
        </tr>
            <td align=center><b>NTSC-J</b></td>
            <td align=center><b><code>_C1 No noise
_L 0x0034AF22 0x00</code></b></td>
            <td align=center><b><code>patch=1,EE,2047B0C0,extended,00000000</code></b></td>
            <td align=center><b><code>8052AAD2</code></b></td>
        </tr>
    </tbody>
</table>