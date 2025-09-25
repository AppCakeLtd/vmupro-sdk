# 8BM Copyright/License notice
# For use with ESP IDF Python 3.10.x
# Note: suggested use with Python 3.6 or later due to handling of Path.resolve

import sys
import argparse
# import crcmod
import os
import json
import struct
from typing import Any, Dict
from PIL import Image
from pathlib import Path

# Rough outline
# - load the .elf into ram
# - parse the json metadata (name, author, icon trans)
# - load and encode the icon

# save the raw binary for each section to a file
# (set via args)
debugOutput = False

# Binary section
sect_mainElf = bytearray()

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

crypto = None
doSign = False
doProductKey = False
doDeviceKey = False


class MetadataError(Exception):
    pass


class PathException(Exception):
    pass


def main():

    global debugOutput

    global doSign
    global absPublisherPrivateKeyPath
    global deviceKeypath

    print("\n")
    print("8BM VMUPro Packer")
    print("Run py packer.py-h for help or a full list of supported arguments")
    print("\n")

    print("Executing command line args:")
    print("  ".join(sys.argv))
    print("\n")

    #
    # Parse arguments
    #

    parser = argparse.ArgumentParser(
        description="Pack a VMUPro binary with optional icon")
    parser.add_argument("--projectdir", required=True,
                        help="Root folder, one beneath build")
    parser.add_argument("--elfname", required=True,
                        help="In projectdir/build, e.g. 'vmupro_minimal' for 'build/vmupro_minimal.app.elf'")
    parser.add_argument("--meta", required=True,
                        help="Relative path .JSON metadata for your package: metadata.json from projectdir")
    parser.add_argument("--sdkversion", required=True,
                        help="In the format format: x.x.x")
    parser.add_argument("--icon", required=True,
                        help="Relative path to a 76x76 icon from projectdir")
    parser.add_argument("--debug", required=False,
                        help="true = Save the raw binary for each section to a file in the 'debug' folder")

    args = parser.parse_args()

    if args.debug:
        debugOutput = True

    #
    # Validate paths
    #

    print("Validating paths...")

    # ensure something like ../../examples/minimal/
    # is resolved to d:\mystuff\examples\minimal, etc
    projectDir = args.projectdir
    if not os.path.isdir(projectDir):
        print("  projectdir doesn't appear to exist at {}".format(projectDir))
        sys.exit(1)
    projectDir = Path(projectDir)

    absProjectDir = projectDir.resolve()
    if not os.path.isdir(absProjectDir):
        print("  Can't confirm absolute path to base dir {}".format(absProjectDir))
        sys.exit(1)

    # e.g. "my_lovely_game" as part of "<projectDir>/build/my_lovely_game.app.elf"
    # which will become "<projectDir>/my_lovely_game.vmupack"
    relElfNameNoExt = args.elfname

    try:
        # "vmupro_minimal" -> "build/vmupro_minimal.app.elf"
        elfPart = os.path.join("build", relElfNameNoExt + ".app.elf")
        absElfPath = ValidatePath(absProjectDir, elfPart)
        print("  Using abs elf path: {}".format(absElfPath))

        absMetaPath = ValidatePath(absProjectDir, args.meta)
        print("  Using abs metadata path: {}".format(absMetaPath))

        absIconPath = ValidatePath(absProjectDir, args.icon)
        print("  Using abs icon path: {}".format(absIconPath))

    except Exception as e:
        print("  Exception: {}".format(e))
        print("  Failed to combine paths, see above errors")
        print("\n  Hint: check <yourelfname>.app.elf exists in the `build` folder\n")
        sys.exit(1)

    CheckKeys()

    #
    # Read and validate the .elf file
    #
    res = PrepareElf(absElfPath)

    if not res:
        print("Failed to prepare the binary, see previous errors")
        sys.exit(1)

    #
    # We've validated that the .elf exists
    # good time to remove some auto built firmwares
    #
    RemoveUnwantedBuildFiles(absProjectDir, relElfNameNoExt)

    #
    # Read and validate the metadata.json
    #

    res = ParseMetadata(absMetaPath, absProjectDir)
    if not res:
        print("Failed to prepare the metadata, see previous errors")

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

    res = CreateHeader(absProjectDir, relElfNameNoExt)
    if not res:
        print("Failed to create header, see previous errors")
        sys.exit(1)

    print("\nExiting with code 0 (success!)\n")
    sys.exit(0)


def CheckKeys():

    global crypto
    global doSign
    global doProductKey
    global doDeviceKey

    import os
    import importlib.util
    import sys
    from pathlib import Path

    cryptoPath = os.environ.get("VMUPRO_CRYPTO_PATH")
    if cryptoPath:
        path = Path(cryptoPath).resolve()
        spec = importlib.util.spec_from_file_location("crypto", path)
        crypto = importlib.util.module_from_spec(spec)
        sys.modules["crypto"] = crypto
        spec.loader.exec_module(crypto)

        doSign, doProductKey, doDeviceKey = crypto.ValidateSigningLogic()


def GetOutputFilenameAbs(absProjectDir, relElfNameNoExt):
    # type: (str,str)->Path
    absOutputVMUPack = os.path.join(
        absProjectDir, relElfNameNoExt + ".vmupack")
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


def EncryptBuffer(inBuffer: bytearray):

    global doDeviceKey
    global doProductKey

    if doDeviceKey or doProductKey:
        crypto.Encrypt(inBuffer)


def PrepareElf(elfPath):
    # type: (str)->bool

    global sect_mainElf
    global doDeviceKey
    global doProductKey

    print("Loading elf file")
    print("  Path: {}".format(elfPath))

    if not os.path.isfile(elfPath):
        print("Elf file not found!")
        return False

    try:
        with open(elfPath, "rb") as f:
            sect_mainElf = bytearray(f.read())
            EncryptBuffer(sect_mainElf)
            sect_mainElfSize = len(sect_mainElf)

    except Exception as e:
        print("Error {}".format(e))
        return False

    print("  Loaded main elf")
    print("  Size: {:,} / {} bytes".format(sect_mainElfSize,
          hex(sect_mainElfSize)))

    return True


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


# the IDF generates a firmware by default as well as your app
# this will be unusable on its own, so to avoid confusion, let's delete it
# note: this is not the minimal yourfile.app.elf, it's yourfile.elf

def RemoveUnwantedBuildFiles(absProjectDir, elfNameNoExt):
    # type: (str, str, str)->None

    print("Cleaning up unwanted build files")

    try:
        # "your_game_name" -> "build/your_game_name.elf"
        delElf = os.path.join(absProjectDir, "build", elfNameNoExt + ".elf")
        delElf = Path(delElf).resolve()
        DeleteFileNoError(delElf, "auto built firmware (elf)")

        # "your_game_name" -> "build/your_game_name.bin"
        delBin = os.path.join(absProjectDir, "build", elfNameNoExt + ".bin")
        delBin = Path(delBin).resolve()
        DeleteFileNoError(delBin, "auto built fiwmare (bin)")

        # delete the old output file (if it exists)
        absPrevBuild = GetOutputFilenameAbs(absProjectDir, elfNameNoExt)
        DeleteFileNoError(absPrevBuild, "previous build (vmupack)")

    except Exception as e:
        print("  Couldn't remove auto built firmware elf (non fatal error)")
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

        # numPixels = width * height

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


#
# Enforcced:
#   Native Apps:
#     Signing: Required
#     Encryption: Optional
#   LUA:
#     Signing: Optional
#     Encryption: Optional
#


# Placeholder for now
# 00-04: reserved0
# 04-08: reserved1
# 08-0C: reserved2
# 0C-0F: reserved3

def AddBinding():
    # type: () -> bool

    global sect_binding
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
    global sect_mainElf

    if not debugOutput:
        return

    print(printVal)
    print("  Header   : {} / {}".format(len(sect_header), hex(len(sect_header))))
    print("  Icon     : {} / {}".format(len(sect_icon), hex(len(sect_icon))))
    print("  MetaData : {} / {}".format(len(sect_outMeta), hex(len(sect_outMeta))))
    print("  Binding  : {} / {}".format(len(sect_binding), hex(len(sect_binding))))
    print("  Elf      : {} / {}".format(len(sect_mainElf), hex(len(sect_mainElf))))


def SignPackage():

    global doSign
    global finalBinary

    if not doSign:
        print("""WARNING: BINARY IS NOT SIGNED""")
        return

    signedHash = crypto.CryptoSign(finalBinary)

    sect_signing = bytearray()

    # 0x00 - 0x10: header
    signHdr = b"SIGN"

    verHdr = struct.pack("<I", 0)

    byteLen = len(signedHash)
    lenHdr = struct.pack("<I", byteLen)

    reserved3 = struct.pack("<I", 0)

    sect_signing.extend(signHdr)
    sect_signing.extend(verHdr)
    sect_signing.extend(lenHdr)
    sect_signing.extend(reserved3)

    # 0x10 - 0xXX: RSA-signed SHA256 signature
    sect_signing.extend(signedHash)

    PadByteArray(sect_signing, 512)

    # and finally pop that on the end of the entire .vmupack
    finalBinary.extend(sect_signing)


def ValidateSignature(absFilePath):

    global doSign

    # If we're not signing, it can't really fail
    if not doSign:
        return True

    # import crypto

    with open(absFilePath, "rb") as vmuPack:

        # Read the whole thing in
        # let the crypto function handle the header, size, etc
        fileBytes = vmuPack.read()

        success = crypto.CryptoTestSign(fileBytes)
        if not success:
            raise Exception("Failed to validate signed package")
        else:
            print("Signature validation success!")
            return True


# adds to the header in the final binary
# not the header stub
def AddToArray(targ, pos, val):
    # type: (bytearray, int,int)->int

    global finalBinary

    bVal = struct.pack("<I", val)
    targ[pos:pos+4] = bVal

    return 4

    # 00-08: uint8_t magic[8] = "VMUPACK\0"
    # 08-0C: uint8_t vmuPackVersion = 1
    #        uint8_t targetDevice = 0
    #        uint8_t productBindingVersion
    #        uint8_t deviceBindingVersion
    # 0C-10: uint32_t reserved
    #
    # 10-30: uint8_t appName[32] = "My awesome app\0"
    #
    # 30-34: uint32_t appMode        # 1= applet, 2= fullscreen
    # 34-38: uint32_t appEnv         # 0 = native, 1 = LUA
    # 38-38: uint32_t reserved
    # 3C-40: uint32_t fileSizeBytesMinusSignature    # aka SignaturePos
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
    # 58-5C: uint32_t bindingOffset
    # 5C-60: uint32_t bindingLength
    #
    # 60-64: uint32_t elfOffset
    # 64-68: uint32_t elfLength
    #
    # 68-78: uint32_t reserved[4]
    #
    # padded to 512 bytes

# TODO: we'll put this in an actual struct once the file format and requirements have settled a bit
def CreateHeader(absProjectDir, relElfNameNoExt):
    # type: (str, str) -> bool

    global headerVersion
    #
    global sect_header
    global sect_icon
    global sect_outMeta
    global sect_binding
    global sect_mainElf
    #
    global finalBinary
    global doSign

    # 0-8: magic
    magic = b"VMUPACK\0"
    sect_header.extend(magic)

    # Add the values we know immediately

    # 8-C: version, targ device, 
    vmuPackVersion = 1
    sect_header.extend(vmuPackVersion.to_bytes(1, 'little'))
    targDevice = 0
    sect_header.extend(targDevice.to_bytes(1,'little'))
    prodBindingVersion = 1 if doProductKey else 0
    sect_header.extend(prodBindingVersion.to_bytes(1,'little'))
    devBindingversion = 1 if doDeviceKey else 0
    sect_header.extend(devBindingversion.to_bytes(1,'little'))

    # C-10: reserved
    reserved0 = 0    
    sect_header.extend(reserved0.to_bytes(4, 'little'))

    # 10-30 - mini header identifier
    appName = outMetaJSON["app_name"]
    appName = bytearray(appName, "ascii")
    # clamp it at 31 chars
    if (len(appName) > 31):
        appName = appName[:31]
    # pad it to exactly 32 chars
    PadByteArray(appName, 32)
    sect_header.extend(appName)

    # 30-34 - app mode
    # 0 = AUTO (not applicable for ext apps)
    # 1 = APPLET (WIP)
    # 2 = FULLSCREEN
    # 3 = EXCLUSIVE (not applicable)
    # Pick 2 for now!
    appMode = outMetaJSON["app_mode"]
    modePacked = struct.pack("<I", appMode)
    sect_header.extend(modePacked)

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
    PadByteArray(sect_mainElf, 512)
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
    print("  Wrote resources at pos {} size {}".format(
        hex(resStart), hex(len(sect_allResources))))

    bindingStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, bindingStart)
    headerFieldPos += AddToArray(finalBinary,
                                 headerFieldPos, len(sect_binding))
    finalBinary.extend(sect_binding)
    print("  Wrote binding at pos {} size {}".format(
        hex(bindingStart), hex(len(sect_binding))))

    elfStart = len(finalBinary)
    headerFieldPos += AddToArray(finalBinary, headerFieldPos, elfStart)
    headerFieldPos += AddToArray(finalBinary,
                                 headerFieldPos, len(sect_mainElf))
    finalBinary.extend(sect_mainElf)
    print("  Wrote elf at pos {} size {}".format(
        hex(elfStart), hex(len(sect_mainElf))))

    # Pad it to 512 for fast SD access
    PadByteArray(finalBinary, 512)

    # and set the main file size in bytes before appending the signing data
    AddToArray(finalBinary, 0x3C, len(finalBinary))

    # and finally, sign it
    SignPackage()

    sect_finalBinarySize = len(finalBinary)
    print("Final binary size: {} / {}".format(
        sect_finalBinarySize, hex(sect_finalBinarySize)))

    absOutPath = GetOutputFilenameAbs(absProjectDir, relElfNameNoExt)
    try:
        with open(absOutPath, "wb") as f:
            f.write(finalBinary)
    except Exception as e:
        print("The .vmupack was successfully built but the file could not be saved to {}".format(
            absOutPath))
        print("Please ensure that the file is not currently open!")
        return False

    print("Write file to: {}".format(absOutPath))

    valid = ValidateSignature(absOutPath)
    if not valid:
        raise Exception("Signature validation failed")

    return True


if __name__ == "__main__":
    main()
