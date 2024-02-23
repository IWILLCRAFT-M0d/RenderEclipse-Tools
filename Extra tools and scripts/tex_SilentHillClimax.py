# Silent Hill Climax Games TXD Textures Noesis Importer
# RenderWare Version 3.7.0.2 (0x1C020065)
# Based from By Allen Script for PS2 SHO adjustments
# made by IWILLCRAFT using code from Allen (PS2 & PSP
# textures) and Zhenёq (Wii Textures)

from inc_noesis import *
import struct
import lib_zq_nintendo_tex as nintex

def registerNoesisTypes():
	handle = noesis.register("Silent Hill Climax Games TXD Textures", ".txd")
	noesis.setHandlerTypeCheck(handle, noepyCheckType)
	noesis.setHandlerLoadRGBA(handle, txdLoadRGBA)    
	return 1

def noepyCheckType(data):
    bs = NoeBitStream(data)
    idstring = bs.readUInt()
    if idstring != 0x16:
            return 0
    return 1

def txdLoadRGBA(data, texList):
    bs = NoeBitStream(data)
    chunk = rwChunk(bs)
    if chunk.chunkID == 0x16:
        rtex = rTex(bs.readBytes(chunk.chunkSize))
        rtex.rTexList()
        for i in range(len(rtex.texList)):
            texList.append(rtex.texList[i])
            dirName = rapi.getDirForFilePath(rapi.getInputName())
            try:
                outName = dirName + "/Textures/" + rtex.texList[i].texName + ".png"
                noesis.saveImageRGBA(outName,rtex.texList[i])
            except:
                print("A texture is missing!")
    return 1

class rwChunk(object):   
        def __init__(self,bs):
                self.chunkID,self.chunkSize,self.chunkVersion = struct.unpack("3I", bs.readBytes(12))

def readRWString(bs):
    rwStrChunk = rwChunk(bs)
    endOfs = bs.tell() + rwStrChunk.chunkSize
    string = bs.readString()
    bs.seek(endOfs)
    return string
      
class rTexNative(object):
    def __init__(self,datas,deviceId):
        self.deviceId = deviceId
        if self.deviceId == 3:
            self.bs = NoeBitStream(datas, NOE_BIGENDIAN)
        else:
            self.bs = NoeBitStream(datas)
    def rTexture(self):
        if self.deviceId == 3:
            #gamecube/wii
            #unk values
            self.bs.seek(36)
            nameEndOfs = self.bs.tell()+64
            texName = self.bs.readString()
            self.bs.seek(nameEndOfs)


            unk0 = self.bs.readUInt()
            width = self.bs.readUShort()
            height = self.bs.readUShort()
            unk1 = self.bs.readUByte()
            mips = self.bs.readUByte()
            dataFormat = self.bs.readUByte()
            unk2 = self.bs.readUByte()
            unk3 = self.bs.readUInt()
            size = self.bs.readUInt()
            
            try:
                texture = nintex.readTexture(self.bs, width, height, dataFormat)
                texture.texName = texName
            except:
                return
        elif self.deviceId == 6:
            #ps2
            texNativeHeaderStruct = rwChunk(self.bs)
            platformId = self.bs.readInt()
            textureFormat = self.bs.readInt()
            texName = readRWString(self.bs)
            maskName = readRWString(self.bs)
            nativeStruct = rwChunk(self.bs)
            rasterStruct = rwChunk(self.bs)
            width = self.bs.readUInt()
            height = self.bs.readUInt()
            depth = self.bs.readUInt()
            rasterFormat = self.bs.readUInt()
            TEX0_GS_REGISTER = self.bs.readUInt64()
            TEX1_GS_REGISTER = self.bs.readUInt64()
            MIPTBP1_GS_REGISTER = self.bs.readUInt64()
            MIPTBP2_GS_REGISTER = self.bs.readUInt64()
            texelDataSectionSize = self.bs.readUInt()
            paletteDataSectionSize = self.bs.readUInt()
            gpuDataAlignedSize = self.bs.readUInt()
            skyMipmapVal = self.bs.readUInt()
            
            texStruct = rwChunk(self.bs)

            texFormatExt = rasterFormat & 0xf000
            texFormat = rasterFormat & 0xf00
            pixelBuffSize = texelDataSectionSize - 80
            palltetBuffSize = paletteDataSectionSize - 80
            
            self.bs.seek(80,NOESEEK_REL) #skip TexPixelHeader
            pixelBuff = self.bs.readBytes(pixelBuffSize)
            
            if paletteDataSectionSize == 0:
                ext = rwChunk(self.bs)
                self.bs.seek(ext.chunkSize,NOESEEK_REL)
                # For some odd ocation pallete could be empty but it seems to be
                # a really weird case as I have only encountered this case in the
                # TXD from UI (PS2 SHSM)

                # Thanks again to Allen/Leeao because they sort out this in their
                # Twisted Metal Black Script
                pixelBuff = parseRgba32(pixelBuff,width,height)
                texData = rapi.imageDecodeRaw(pixelBuff, width, height, "r8g8b8a8")
                texture = NoeTexture(texName, width, height, texData, noesis.NOESISTEX_RGBA32)
                texture.setFlags(noesis.NTEXFLAG_FILTER_NEAREST)
                return texture
            
            self.bs.seek(80, NOESEEK_REL)
            
            if texFormatExt == 0x2000:
                    palette = readPS2Palette(self.bs,256)
            elif texFormatExt == 0x4000:
                    palette = readPS2Palette(self.bs,16)
                    self.bs.seek(palltetBuffSize - 64, NOESEEK_REL)#skip padding
            ext = rwChunk(self.bs) 
            self.bs.seek(ext.chunkSize, NOESEEK_REL)
            if depth == 4:
                # using the noesis function gives corrupted results
                # the bug occurs because the original function doesn't handle
                # less than 32x16 textures
                pixelBuff = unswizzle4(pixelBuff, width, height)
                # pixelBuff = rapi.imageUntwiddlePS2(pixelBuff, width, height, depth)
            elif depth == 8:
                pixelBuff = rapi.imageUntwiddlePS2(pixelBuff, width, height, depth)
                palette = unswizzlePalette(palette)
            texData = rapi.imageDecodeRawPal(pixelBuff, palette, width, height, depth, "r8g8b8a8")
            texture = NoeTexture(texName, width, height, texData, noesis.NOESISTEX_RGBA32)
            texture.setFlags(noesis.NTEXFLAG_FILTER_NEAREST)
        elif self.deviceId == 9:
            #psp
            texNativeHeaderStruct = rwChunk(self.bs)

            rasterFormat = self.bs.readUInt()
            width = self.bs.readUShort()
            height = self.bs.readUShort()
            depth = self.bs.readUByte()
            numLevels = self.bs.readUByte()
            rasterType = self.bs.readUByte()
            bitFlag = self.bs.readUByte()
            
            unkFlag = self.bs.readInt()
            self.bs.seek(76,NOESEEK_REL)
            unk1 = self.bs.readInt()
            unk2 = self.bs.readInt()        
            
            platformId = self.bs.readInt()
            textureFormat = self.bs.readInt()
            nameEndOfs = self.bs.tell()+64
            texName = self.bs.readString()
            self.bs.seek(nameEndOfs)
            
            texFormatExt = rasterFormat & 0xf000
            
            texData = bytearray()
            
            if texFormatExt == 0x2000:
                palette = self.bs.readBytes(1024)
            else:
                palette = self.bs.readBytes(64)
            
            pixelBuffSize = texNativeHeaderStruct.chunkSize - self.bs.tell() + 12
            pixelBuff = self.bs.readBytes(pixelBuffSize)
            pixelBuff = rapi.imageUntwiddlePSP(pixelBuff,width,height, depth)
            texData = rapi.imageDecodeRawPal(pixelBuff, palette, width, height, depth, "r8g8b8a8")
            texture = NoeTexture(texName, width, height, texData, noesis.NOESISTEX_RGBA32)
        texture.texName = texName
        return texture

def rgba32(rawPixel):
    t = bytearray(4)
    t[0] = rawPixel & 0xFF
    t[1] = (rawPixel >> 8) & 0xFF
    t[2] = (rawPixel >> 16)  & 0xFF
    t[3] = min(((rawPixel >> 24) & 0xFF)*2, 255)
    return t

def readPS2Palette(bs,numColor):
    palette = bytes()
    for i in range(numColor):
        palette += rgba32(bs.readUInt()) 
    return palette

def unswizzlePalette(palBuffer):  
    newPal = bytearray(1024)
    for p in range(256):
        pos = ((p & 231) + ((p & 8) << 1) + ((p & 16) >> 1)) 
        newPal[pos*4:pos*4+4] = palBuffer[p*4:p*4+4]
    return newPal

def parseRgba32(buf,width,height):
    ms = NoeBitStream(buf)
    outPixel = bytes()
    for i in range(width*height):
        pixel = ms.readUInt()
        outPixel += bytes(rgba32(pixel))
    return outPixel

def unswizzle4(buffer, width, height):
    pixels = bytearray(width * height)
    for i in range(width * height//2):
        index = buffer[i]
        id2 = (index >> 4) & 0xf
        id1 = index & 0xf
        pixels[i*2] = id1
        pixels[i*2+1] = id2
    newPixels = rapi.imageUntwiddlePS2(pixels, width, height, 8)
    result = bytearray(width * height)
    for i in range(width*height//2):
        idx1 = newPixels[i*2]
        idx2 = newPixels[i*2+1]
        idx = ((idx2 << 4) | idx1) & 0xff
        result[i] = idx
    return result

class rTex(object):
    def __init__(self,datas):
        self.bs = NoeBitStream(datas)
        self.texList = []
        self.texCount = 0
    def rTexList(self):
        texStruct = rwChunk(self.bs)
        texCount = self.bs.readUShort()
        self.texCount = texCount
        deviceId = self.bs.readUShort() # 1 for D3D8, 2 for D3D9, 3 for Gamecube/Wii, 6 for PlayStation 2, 8 for XBOX, 9 for PSP
        for i in range(self.texCount):
            texNativeHeader = rwChunk(self.bs)
            datas = self.bs.readBytes(texNativeHeader.chunkSize)
            texNative = rTexNative(datas, deviceId)
            texture = texNative.rTexture()
            texture.setFlags(noesis.NTEXFLAG_FILTER_NEAREST)
            self.texList.append(texture)
