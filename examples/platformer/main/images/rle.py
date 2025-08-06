import struct
from PIL import Image

def rle_encode_16bit(data: bytes) -> bytes:

    # read out a 16bit 565 value from 2 bytes
    def Get16(pos):        
        hiByte = data[pos*2]
        loByte = data[(pos*2) +1]
        byteVal = loByte << 8 | hiByte
        return byteVal
    
    numPixels = len(data) /2

    #pixels = struct.unpack('<{}H'.format(len(data)//2), data)
    out = bytearray()
    i = 0
    while i < numPixels:
        count = 1
        # while (i + count < numPixels
        #        and Get16(i) == Get16(i + count)
        #        and count < 255):
        currentVal = Get16(i)
        while True:
            if  i + count >= numPixels :
                break
            if  currentVal != Get16(i+count) :
                break
            if count >= 255:
                break;
            count += 1

        out.append(count)
        #out.extend(struct.pack('<H', pixels[i]))

        u16 = struct.pack('<H', currentVal)
        out.extend( u16 )

        i += count
    return out

def rle_decode_16bit(rle_data: bytes) -> bytes:
    rVal = bytearray()
    i = 0
    while i < len(rle_data):
        runLength = rle_data[i]
        #pixel = struct.unpack('<H', rle_data[i+1:i+3])[0]
        #rVal.extend(struct.pack('<H', pixel) * count)

        u16 = rle_data[i+2] << 8 | rle_data[i+1]
        u16 = struct.pack('<H', u16)

        for run in range(runLength):
            rVal.extend(u16)

        i += 3
    return bytes(rVal)

# Example usage
if __name__ == "__main__":

    #im = Image.open("tilemap.bmp");
    #pix = im.load()
    #width = im.size[0]
    #height = im.size[1]
    #print( type(pix))
    f = open("tilemap.raw", "rb")
    fBytes = f.read();
    


    # Fake 16-bit image: [0x1234, 0x1234, 0x5678, 0x5678, 0x5678, 0x9ABC]
    #raw = struct.pack('<6H', 0x1234, 0x1234, 0x5678, 0x5678, 0x5678, 0x9ABC)
    compressed = rle_encode_16bit(fBytes)
    restored = rle_decode_16bit(compressed)

    savefile = open("savebytes2", "wb")
    savefile.write(compressed)

    #print(f"Original:   {fBytes.hex()}")
    #print(f"Compressed: {compressed.hex()}")
    #print(f"Restored:   {restored.hex()}")
    print(f"Original size {len(fBytes)} new size is {len(compressed)} restored is {len(restored)}")
    
    print("Success!" if fBytes == restored else "FAIL")