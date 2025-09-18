# 8BM Copyright/License notice
# LUA-only packer for VMU Pro applications
# Note: suggested use with Python 3.6 or later due to handling of Path.resolve

import sys
import argparse
import os
import json
import struct
from typing import Any, Dict
from PIL import Image
from pathlib import Path

# Rough outline for LUA apps
# - load the LUA scripts and resources
# - parse the json metadata (name, author, icon trans)
# - load and encode the icon
# - package everything into .vmupack format

headerVersion = 1
encryptionVersion = 0

# save the raw binary for each section to a file
# (set via args)
debugOutput = False

# Icon section
sect_icon = bytearray()

# Metadata section
outMetaJSON = {}
sect_outMeta = bytearray()

# Resources (all combined)
sect_allResources = bytearray()

# Individual resource info
# (names and offsets within the master resources data blob)
resourceNameOffsetKeyVals = []

# Device binding
sect_binding = bytearray()

sect_header = bytearray()

finalBinary = bytearray()


class MetadataError(Exception):
    pass


class PathException(Exception):
    pass


def main():

    global debugOutput

    print("\n")
    print("8BM VMUPro LUA Packer")
    print("Run py packer_lua.py -h for help or a full list of supported arguments")
    print("\n")

    print("Executing command line args:")
    print("  ".join(sys.argv))
    print("\n")

    #
    # Parse arguments
    #

    parser = argparse.ArgumentParser(
        description="Pack a VMUPro LUA application with icon")
    parser.add_argument("--projectdir", required=True,
                        help="Root folder containing your LUA app")
    parser.add_argument("--appname", required=True,
                        help="Application name for output file, e.g. 'hello_world' for 'hello_world.vmupack'")
    parser.add_argument("--meta", required=True,
                        help="Relative path .JSON metadata for your package: metadata.json from projectdir")
    parser.add_argument("--sdkversion", required=True,
                        help="In the format format: x.x.x")
    parser.add_argument("--icon", required=True,
                        help="Relative path to a 76x76 BMP icon from projectdir")
    parser.add_argument("--debug", required=False,
                        help="true = Save the raw binary for each section to a file in the 'debug' folder")

    args = parser.parse_args()

    if args.debug:
        debugOutput = True

    #
    # Validate paths
    #

    print("Validating paths...")

    projectDir = args.projectdir
    if not os.path.isdir(projectDir):
        print("  projectdir doesn't appear to exist at {}".format(projectDir))
        sys.exit(1)
    projectDir = Path(projectDir)

    absProjectDir = projectDir.resolve()
    if not os.path.isdir(absProjectDir):
        print("  Can't confirm absolute path to base dir {}".format(absProjectDir))
        sys.exit(1)

    appName = args.appname

    try:
        absMetaPath = ValidatePath(absProjectDir, args.meta)
        print("  Using abs metadata path: {}".format(absMetaPath))

        absIconPath = ValidatePath(absProjectDir, args.icon)
        print("  Using abs icon path: {}".format(absIconPath))

    except Exception as e:
        print("  Exception: {}".format(e))
        print("  Failed to combine paths, see above errors")
        sys.exit(1)

    #
    # Read and validate the metadata.json
    #

    res = ParseMetadata(absMetaPath, absProjectDir)
    if not res:
        print("Failed to prepare the metadata, see previous errors")
        sys.exit(1)

    # Verify this is a LUA application
    if outMetaJSON.get("app_mode", 0) != 1:
        print("Error: This packer is for LUA applications only (app_mode must be 1)")
        sys.exit(1)

    #
    # Read or create the icon
    #

    res = AddIcon(absProjectDir, absIconPath, outMetaJSON["icon_transparency"])
    if not res:
        print("Failed to prepare the icon, see previous errors")
        sys.exit(1)

    #
    # Add device-specific bindings (stub)
    #
    res = AddBinding()
    if not res:
        print("Failed to add device bindings, see previous errors")
        sys.exit(1)

    res = CreateHeader(absProjectDir, appName)
    if not res:
        print("Failed to create header, see previous errors")
        sys.exit(1)

    print("\nExiting with code 0 (success!)\n")
    sys.exit(0)


def GetOutputFilenameAbs(absProjectDir, appName):
    # type: (str,str)->Path
    absOutputVMUPack = os.path.join(
        absProjectDir, appName + ".vmupack")
    absOutputVMUPack = Path(absOutputVMUPack).resolve()
    return absOutputVMUPack


def ValidatePath(base, tail):
    # type: (Union[str, Path], Union[str, Path]) -> str

    joined = base / tail
    resolved = Path.resolve(joined)

    print("  Validating path: {}".format(resolved))

    if not os.path.isfile(resolved):
        raise PathException(
            "projectdir ({}) + tail ({}) didn't form a valid absolute path!".format(base, tail))

    return str(joined)


def DeleteFileNoError(absPath, label):
    # type: (Path, str)->None

    try:
        print("  Checking '{}' ...".format(absPath))
        if os.path.isfile(absPath):
            os.remove(absPath)
        print("  deleted...")

    except Exception as e:
        print("  Couldn't remove {} (non fatal error)".format(label))
        print("  Exception: {}".format(e))


def PrepDebugDir(absProjectDir, fileName):
    # type (str, str)->str

    absDebugDir = os.path.join(absProjectDir, "vmupacker_debug")
    if not os.path.isdir(absDebugDir):
        os.makedirs(absDebugDir)
    absFilePath = os.path.join(absDebugDir, fileName)
    return absFilePath


def AddIcon(absProjectDir, absIconPath, transparentBit):
    # type: (str, str, bool)->bool

    global sect_icon

    print("  Loading icon")
    print("    Path: {}".format(absIconPath))

    if not os.path.isfile(absIconPath):
        print("Failed to load icon at path {}".format(absIconPath))
        return False

    try:

        im = Image.open(absIconPath)
        pix = im.load()

        width = im.size[0]
        height = im.size[1]
        dummy = 0

        if (width != 76 or height != 76):
            print("Error, expecting a 76x76px icon")
            return False

        # add width, height, trans bit and a dummy field
        sect_icon.extend(b'ICON')
        sect_icon.extend(dummy.to_bytes(4, byteorder='little'))
        sect_icon.extend(dummy.to_bytes(4, byteorder='little'))
        sect_icon.extend(dummy.to_bytes(4, byteorder='little'))
        sect_icon.extend(width.to_bytes(4, byteorder='little'))
        sect_icon.extend(height.to_bytes(4, byteorder='little'))
        sect_icon.extend(transparentBit.to_bytes(4, byteorder='little'))
        sect_icon.extend(dummy.to_bytes(4, byteorder='little'))

        for row in range(height):
            for col in range(width):

                x = col
                y = row

                if y < height:
                    rgb = pix[x, y]
                else:
                    rgb = (0, 0, 0)

                # Convert the RGB value into a 16 bit 565 value
                red = (rgb[0] >> 3) & 0x1F  # 5 bits for red
                green = (rgb[1] >> 2) & 0x3F  # 6 bits for green
                blue = (rgb[2] >> 3) & 0x1F  # 5 bits for blue

                # Pack the RGB 565 into 16 bits
                pixVal = (red << 11) | (green << 5) | blue

                # Append the high and low bytes of the 16-bit value
                sect_icon.append((pixVal >> 8) & 0xFF)
                sect_icon.append(pixVal & 0xFF)

    except Exception as e:
        print("Error {}".format(e))
        return False

    sect_iconSize = len(sect_icon)

    print("    Encoded icon from {}".format(absIconPath))
    print(
        "    Size: {:,} / {} bytes".format(sect_iconSize, hex(sect_iconSize)))

    if debugOutput:
        absFilePath = PrepDebugDir(absProjectDir, "icon.bin")
        with open(absFilePath, "wb") as f:
            f.write(sect_icon)
        print("    DEBUG: Wrote {}".format(absFilePath))

    return True

# Read metadata such as the app name and author
# we then repackage this with some extra info
# such as the offsets of each asset into the resources blob


def ParseMetadata(absMetaPath, absProjectDir):
    # type: (str, str)->bool

    print("Loading metadata json")
    print("  path {}".format(absMetaPath))

    if not os.path.isfile(absMetaPath):
        print("Metadata file not found!")
        return False

    jsonData = None
    try:
        with open(absMetaPath, "r") as f:
            jsonData = json.load(f)
    except Exception as e:
        print("Error {}".format(e))
        return False

    res = ValidateMetadata(jsonData, absMetaPath, absProjectDir)

    if not res:
        print("Failed to validate metadata json @ {}".format(absMetaPath))
        return False

    return True

# We could use jsonschema here, but due to legibility
# and flexibility concerns, let's manually review and
# try to throw meaningful errors, to help the user


def ValidateMetadata(inJsonData, absMetaFileName, absProjectDir):
    # type: (Dict[str,any], str, str) -> bool

    global outMetaJSON
    global sect_outMeta
    global debugOutput

    print("  Parsing metadata from {}".format(absMetaFileName))

    try:
        metaVersion = inJsonData["metadata_version"]
    except Exception as e:
        print("Failed to read 'metadata_version' from {}".format(absMetaFileName))
        return False

    if metaVersion != 1:
        print("Unexpected metadata_version '{}', expected '1'".format(metaVersion))
        return False

    # version looks good, let's validate the rest

    def readStr(key, minLength):
        # type: (str, int) -> str
        try:
            print("  Reading '{}'".format(key))
            val = inJsonData[key]
            if (len(val) < minLength or len(val) > 255):
                raise MetadataError(
                    "Expected key '{}' between 1 and 255 chars")

        except Exception as e:
            raise MetadataError(
                "Failed to parse key string '{}' from {}".format(key, absMetaFileName))

        outMetaJSON[key] = val
        print("    {} = {}".format(key, val))
        return val

    def readBool(key):
        # type: (str) -> bool
        try:
            print("  Reading '{}'".format(key))
            val = inJsonData[key]
        except Exception as e:
            raise MetadataError(
                "Failed to parse key bool '{}' from {}".format(
                    key, absMetaFileName)
            )
        outMetaJSON[key] = val
        print("    {} = {}".format(key, val))
        return val

    def readUInt32(key):
        # type (str) -> int
        try:
            print("  Reading '{}'".format(key))
            val = inJsonData[key]
            if not isinstance(val, int):
                raise MetadataError(
                    "Expected key '{}' to be an int".format(key))
            if val < 0 or val > 0xFFFFFFFF:
                raise MetadataError(
                    "Expected key '{}' to be an unsigned 32 bit int".format(key))
        except Exception as e:
            raise MetadataError(
                "Failed to parse key uint32_t '{}' from {}".format(key, absMetaFileName))
        outMetaJSON[key] = val
        print("    {} = {}".format(key, val))
        return val

    #
    # Read in the main vals
    #

    try:
        app_name = readStr("app_name", 1)
        app_author = readStr("app_author", 1)
        app_version = readStr("app_version", 5)
        app_entry_point = readStr("app_entry_point", 1)
        icon_trans = readBool("icon_transparency")
        app_mode = readUInt32("app_mode")
        app_environment = readStr("app_environment", 3)

    except Exception as e:
        print("Parse error: {}".format(e))
        return False

    #
    # Validate the version string
    #

    versionSplits = app_version.split(".")
    if len(versionSplits) != 3:
        return False
    validVersion = all(split.isdigit() for split in versionSplits)
    if not validVersion:
        print("Expected version in the form ?.?.?")
        return False

    res = ParseResources(inJsonData, absMetaFileName, absProjectDir)
    if not res:
        return False

    jsonString = json.dumps(outMetaJSON, indent=4)
    jsonBytes = bytearray(jsonString, "ascii")
    sect_outMeta.extend(jsonBytes)

    if debugOutput:
        absFilePath = PrepDebugDir(absProjectDir, "resources.json")
        # The accompanying json
        with open(absFilePath, "w") as f:
            f.write(jsonString)
        print("    DEBUG: Wrote {}".format(absFilePath))

    return True


def ParseResources(inJsonData, absMetaFileName, absProjectDir):
    # type: (Dict[str,any], str, str) -> bool

    global outMetaJSON
    # all resources combined
    global sect_allResources
    # individual resources offsets
    global resourceNameOffsetKeyVals

    print("  Parsing metadata resources...")

    if (inJsonData["resources"] is None):
        print("    No resources section, skipping")
        return True

    inJsonResArray = inJsonData["resources"]
    outMetaJSON["resources"] = []

    for r in inJsonResArray:
        print("    Reading resource {}".format(r))

        absResPath = absProjectDir / r
        absResPath = Path(absResPath)
        absResPath = absResPath.resolve()

        if not os.path.isfile(absResPath):
            print("      Couldn't locate resource {} from base path: {} and tail: {}".format(
                absResPath, absProjectDir, r))
            return False

        print("      Located @: {}".format(absResPath))

        try:
            with open(absResPath, "rb") as f:
                data = bytearray(f.read())
                dataLen = len(data)
                # append to the master list
                print("      Read {} / {} bytes".format(dataLen, hex(dataLen)))

                # name and location
                # mything.png : 200
                startOffset = len(sect_allResources)
                kvp = (r, startOffset)
                resourceNameOffsetKeyVals.append(kvp)

                # Add the key value pair to the output json
                outMetaJSON["resources"].append(kvp)

                # Add the file to the blob
                sect_allResources.extend(data)

                print(
                    "      Data starts at {} / {} bytes".format(startOffset, hex(startOffset)))

                # pad the data out to 512 byte boundaries for much faster SD access
                paddingLength = PadByteArray(sect_allResources, 512)
                print("      Padding data end by {} bytes to 512 boundary @ {}".format(
                    paddingLength, hex(len(sect_allResources))))

        except Exception as e:
            print("Failed to open file @ {}".format(absResPath))
            print("Exception: {}".format(e))
            return False

    sect_allResourcesSize = len(sect_allResources)
    numResources = len(resourceNameOffsetKeyVals)
    print("    Created resource blob of size {} / {} with {} files".format(
        sect_allResourcesSize, hex(sect_allResourcesSize), numResources))

    if debugOutput:
        absFilePath = PrepDebugDir(absProjectDir, "resources.bin")
        # The binary data
        # (the json offsets will be amongst the metadata)
        with open(absFilePath, "wb") as f:
            f.write(sect_allResources)
        print("    DEBUG: Wrote {}".format(absFilePath))

    return True


def AddBinding():
    # type: () -> bool

    global sect_binding

    print("  Adding dummy binding")

    sect_binding = bytearray(16)

    return True

# Pad a byte array to e.g. 512 bytes for
# faster loading from SD card, or header alignment
# returns: number of padding bytes


def PadByteArray(inArray, boundary):
    # type: (bytearray, int)->int

    modulo = len(inArray) % boundary
    if (modulo != 0):
        paddingLen = boundary - modulo
        paddingBytes = bytearray(paddingLen)
        inArray.extend(paddingBytes)
        return paddingLen

    return 0


def PrintSectionSizes(printVal):
    # type: (str)->None

    global debugOutput
    global sect_header
    global sect_icon
    global sect_outMeta
    global sect_binding

    if not debugOutput:
        return

    print(printVal)
    print("  Header   : {} / {}".format(len(sect_header), hex(len(sect_header))))
    print("  Icon     : {} / {}".format(len(sect_icon), hex(len(sect_icon))))
    print("  MetaData : {} / {}".format(len(sect_outMeta), hex(len(sect_outMeta))))
    print("  Binding  : {} / {}".format(len(sect_binding), hex(len(sect_binding))))
    print("  LUA Resources : {} / {}".format(len(sect_allResources), hex(len(sect_allResources))))

    # 00-08: uint8_t magic[8] = "VMUPACK\0"
    # 08-0C: uint32_t version = 1
    # 0C-10: uint32_t encryptionVersion = 0 (not encrypted)
    #
    # 10-30: uint8_t appName[32] = "My awesome app\0"
    #
    # 30-34: uint32_t appMode     # 1= applet, 2= fullscreen
    # 34-38: uint32_t appEnv      # 0 = native, 1 = LUA
    # 38-40: uint32_t reserved[2]
    #
    # 40-44: uint32_t iconOffset
    # 44-48: uint32_t iconLength
    #
    # 48-4C: uint32_t metadataOffset
    # 4C-50: uint32_t metadataLength
    #
    # 50-54: uint32_t resourceOffset
    # 54-58: uint32_t resourceLength
    #
    # V---- Encrypted section ----V
    #
    # 58-5C: uint32_t bindingOffset
    # 5C-60: uint32_t bindingLength
    #
    # 60-64: uint32_t elfOffset
    # 64-68: uint32_t elfLength
    #
    # 68-78: uint32_t reserved[4]
    #
    # padded to 512 bytes

# adds to the header in the final binary
# not the header stub
def AddToArray(targ, pos, val):
    # type: (bytearray, int,int)->int

    global finalBinary

    bVal = struct.pack("<I", val)
    targ[pos:pos+4] = bVal

    return 4


def CreateHeader(absProjectDir, appName):
    # type: (str, str) -> bool

    global headerVersion
    global encryptionVersion
    #
    global sect_header
    global sect_icon
    global sect_outMeta
    global sect_binding
    global sect_allResources
    #
    global finalBinary

    # 0-8: magic
    magic = b"VMUPACK\0"
    sect_header.extend(magic)

    # Add the values we know immediately

    # 8-C: version
    sect_header.extend(headerVersion.to_bytes(4, 'little'))

    # C-10: encryption
    sect_header.extend(encryptionVersion.to_bytes(4, 'little'))

    # 10-30 - mini header identifier
    appNameHeader = outMetaJSON["app_name"]
    appNameHeader = bytearray(appNameHeader, "ascii")
    # clamp it at 31 chars
    if (len(appNameHeader) > 31):
        appNameHeader = appNameHeader[:31]
    # pad it to exactly 32 chars
    PadByteArray(appNameHeader, 32)
    sect_header.extend(appNameHeader)

    # 30-34 - app mode
    # 0 = AUTO (not applicable for ext apps)
    # 1 = APPLET (WIP)
    # 2 = FULLSCREEN
    # 3 = EXCLUSIVE (not applicable)
    # Pick 2 for now!
    appMode = outMetaJSON["app_mode"]

    # 34-38 app env
    envStr = outMetaJSON["app_environment"]
    envVal = 0
    if envStr == "native":
        envVal = 0
    elif envStr == "lua":
        envVal = 1
    else:
        envVal = 0xFFFFFFFF
    envPacked = struct.pack("<I", envVal)
    sect_header.extend(envPacked)

    # 2 reserved fields
    # then we'll start adding the other sections
    res1Packed = struct.pack("<I", 0)
    res2Packed = struct.pack("<I", 0)
    sect_header.extend(res1Packed)
    sect_header.extend(res2Packed)

    headerFieldPos = len(sect_header)
    print("Continuing header from offset {}".format(headerFieldPos))

    #
    # Pad out some byte arrays and then let's start piecing them together
    #

    PrintSectionSizes("Section sizes:")
    PadByteArray(sect_header, 512)
    PadByteArray(sect_icon, 512)
    PadByteArray(sect_outMeta, 512)
    PadByteArray(sect_binding, 512)
    PadByteArray(sect_allResources, 512)
    PrintSectionSizes("Padded section sizes:")

    #
    # Write Header int the final binary
    # Update header fields as we append new sections
    #

    finalBinary.extend(sect_header)
    
    iconStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, iconStart)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, len(sect_icon))
    finalBinary.extend(sect_icon)
    print("  Wrote icon at pos {} size {}".format(
        hex(iconStart), hex(len(sect_icon))))

    metaStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, metaStart)
    headerFieldPos += AddToArray(finalBinary,
                                 headerFieldPos, len(sect_outMeta))
    finalBinary.extend(sect_outMeta)
    print("  Wrote metadata at pos {} size {}".format(
        hex(metaStart), hex(len(sect_outMeta))))

    resStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, resStart)
    headerFieldPos += AddToArray(finalBinary,
                                 headerFieldPos, len(sect_allResources))
    finalBinary.extend(sect_allResources)
    print("  Wrote LUA resources at pos {} size {}".format(
        hex(resStart), hex(len(sect_allResources))))

    bindingStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, bindingStart)
    headerFieldPos += AddToArray(finalBinary,
                                 headerFieldPos, len(sect_binding))
    finalBinary.extend(sect_binding)
    print("  Wrote binding at pos {} size {}".format(
        hex(bindingStart), hex(len(sect_binding))))

    # For LUA apps, we don't have ELF data, so write empty section
    luaStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, luaStart)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, 0)  # Zero length
    print("  Wrote LUA section (empty) at pos {} size 0".format(hex(luaStart)))

    sect_finalBinarySize = len(finalBinary)
    print("Final binary size: {} / {}".format(
        sect_finalBinarySize, hex(sect_finalBinarySize)))

    absOutPath = GetOutputFilenameAbs(absProjectDir, appName)
    try:
        with open(absOutPath, "wb") as f:
            f.write(finalBinary)
    except Exception as e:
        print("The .vmupack was successfully built but the file could not be saved to {}".format(
            absOutPath))
        print("Please ensure that the file is not currently open!")
        return False

    print("Write file to: {}".format(absOutPath))

    return True


if __name__ == "__main__":
    main()