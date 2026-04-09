# 8BM Copyright/License notice
# VMU Pro unified packer for LUA and C/C++ (native) applications
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
# - For native (C) apps: load the .elf into ram
# - For LUA apps: load scripts and resources (no ELF)
# - parse the json metadata (name, author, icon trans)
# - load and encode the icon

# save the raw binary for each section to a file
# (set via args)
debugOutput = False

# Store format: separate .lua code into ELF section as a code bundle
# Non-lua assets remain in resource section
storeFormat = False

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

# Main ELF binary (native apps only, empty for LUA apps)
sect_mainElf = bytearray()

# Device binding
sect_binding = bytearray()

# Lua code bundle (store format only — goes into ELF section)
sect_luaCodeBundle = bytearray()

sect_header = bytearray()

finalBinary = bytearray()

crypto = None
doSign = True
doProductKey = False
doDeviceKey = False


class MetadataError(Exception):
    pass


class PathException(Exception):
    pass


def ReadSDKVersion():
    """
    Read the SDK version from the VERSION file in the SDK root directory.
    Returns a tuple of (major, minor, patch) as integers.
    """
    scriptDir = Path(__file__).resolve().parent
    sdkRoot = scriptDir.parent.parent
    versionFile = sdkRoot / "VERSION"

    if not os.path.isfile(versionFile):
        return None

    try:
        with open(versionFile, "r") as f:
            versionStr = f.read().strip()
            parts = versionStr.split(".")
            if len(parts) != 3:
                return None
            major = int(parts[0])
            minor = int(parts[1])
            patch = int(parts[2])
            if major < 0 or major > 255 or minor < 0 or minor > 255 or patch < 0 or patch > 255:
                return None
            return (major, minor, patch)
    except Exception:
        return None


def DetectAppEnvironment(absMetaPath):
    """
    Peek at metadata.json to determine app_environment before full parsing.
    Returns 'lua' or 'native'.
    """
    try:
        with open(absMetaPath, "r") as f:
            meta = json.load(f)
            return meta.get("app_environment", "native")
    except Exception:
        return "native"


def main():

    global debugOutput
    global storeFormat

    global doSign
    global absPublisherPrivateKeyPath
    global deviceKeypath

    print("\n")
    print("8BM VMUPro Packer")
    print("Run py packer.py -h for help or a full list of supported arguments")
    print("\n")

    print("Executing command line args:")
    print("  ".join(sys.argv))
    print("\n")

    #
    # Parse arguments
    #

    parser = argparse.ArgumentParser(
        description="Pack a VMUPro application (LUA or native C/C++) with optional icon")
    parser.add_argument("--projectdir", required=True,
                        help="Root folder containing your app project")
    parser.add_argument("--appname", required=True,
                        help="Application name for the output .vmupack file. For native apps, also used to locate build/<appname>.app.elf")
    parser.add_argument("--meta", required=True,
                        help="Relative path .JSON metadata for your package: metadata.json from projectdir")
    parser.add_argument("--sdkversion", required=False,
                        help="SDK version in x.x.x format (auto-read from VERSION file if not provided)")
    parser.add_argument("--icon", required=True,
                        help="Relative path to a 76x76 icon from projectdir")
    parser.add_argument("--debug", required=False,
                        help="true = Save the raw binary for each section to a file in the 'debug' folder")
    parser.add_argument("--store-format", required=False, action="store_true",
                        help="Separate .lua code into ELF section as a code bundle (for store encryption)")

    args = parser.parse_args()

    if args.debug:
        debugOutput = True

    if args.store_format:
        storeFormat = True
        print("Store format enabled: .lua files will be packed into ELF section")

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

    relElfNameNoExt = args.appname

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
    # Detect app type from metadata before full parsing
    #
    appEnv = DetectAppEnvironment(str(absMetaPath))
    isNativeApp = (appEnv == "native")
    print("  App environment: {} ({})".format(appEnv, "native C/C++" if isNativeApp else "LUA scripted"))

    #
    # Resolve SDK version: from --sdkversion arg, or from VERSION file
    #
    if args.sdkversion:
        print("  SDK version (from args): {}".format(args.sdkversion))
    else:
        sdkVer = ReadSDKVersion()
        if sdkVer:
            args.sdkversion = "{}.{}.{}".format(sdkVer[0], sdkVer[1], sdkVer[2])
            print("  SDK version (from VERSION file): {}".format(args.sdkversion))
        else:
            print("  Warning: No --sdkversion provided and VERSION file not found. Using 0.0.0")
            args.sdkversion = "0.0.0"

    #
    # For native apps: validate and load the ELF
    #
    absElfPath = None
    if isNativeApp:
        try:
            elfPart = os.path.join("build", relElfNameNoExt + ".app.elf")
            absElfPath = ValidatePath(absProjectDir, elfPart)
            print("  Using abs elf path: {}".format(absElfPath))
        except Exception as e:
            print("  Exception: {}".format(e))
            print("\n  Hint: check <yourelfname>.app.elf exists in the `build` folder\n")
            sys.exit(1)

        CheckKeys()

        res = PrepareElf(absElfPath)
        if not res:
            print("Failed to prepare the binary, see previous errors")
            sys.exit(1)

        RemoveUnwantedBuildFiles(absProjectDir, relElfNameNoExt)
    else:
        print("  LUA app: skipping ELF loading")
        CheckKeys()

    #
    # Read and validate the metadata.json
    #

    res = ParseMetadata(absMetaPath, absProjectDir)
    if not res:
        print("Failed to prepare the metadata, see previous errors")
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

    # Parse SDK version into tuple
    sdkVerParts = args.sdkversion.split(".")
    sdkVersion = (int(sdkVerParts[0]), int(sdkVerParts[1]), int(sdkVerParts[2]))

    res = CreateHeader(absProjectDir, relElfNameNoExt, sdkVersion)
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
    else:
        doSign = False


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


def BuildLuaCodeBundle(luaFiles):
    # type: (List[Tuple[str, bytes]]) -> bytearray
    """
    Build a Lua code bundle in the binary format expected by the device.
    Format: [file_count:4][path_len:4][path:N][content_len:4][content:M]...
    All integers are little-endian uint32.
    """
    bundle = bytearray()

    # File count
    bundle.extend(struct.pack("<I", len(luaFiles)))

    for relativePath, data in luaFiles:
        pathBytes = relativePath.encode("utf-8")
        # Path length + path
        bundle.extend(struct.pack("<I", len(pathBytes)))
        bundle.extend(pathBytes)
        # Content length + content
        bundle.extend(struct.pack("<I", len(data)))
        bundle.extend(data)

    return bundle


def ScanFolderRecursive(baseDir, folderPath):
    """
    Recursively scan a folder and return all files with their relative paths.
    Returns list of (relative_path, absolute_path) tuples.
    """
    files = []
    absFolderPath = baseDir / folderPath
    absFolderPath = Path(absFolderPath).resolve()

    print("        Scanning folder: {}".format(absFolderPath))

    try:
        for root, _, filenames in os.walk(absFolderPath):
            for filename in filenames:
                absFilePath = Path(root) / filename
                relativeFromBase = absFilePath.relative_to(baseDir.resolve())
                relativePath = str(relativeFromBase).replace('\\', '/')
                files.append((relativePath, absFilePath))
                print("          Found: {}".format(relativePath))
    except Exception as e:
        print("        ERROR scanning folder: {}".format(e))

    return files


def ParseResources(inJsonData, absMetaFileName, absProjectDir):
    # type: (Dict[str,any], str, str) -> bool

    global outMetaJSON
    # all resources combined
    global sect_allResources
    # individual resources offsets
    global resourceNameOffsetKeyVals
    global sect_luaCodeBundle
    global storeFormat

    print("  Parsing metadata resources...")

    if (inJsonData["resources"] is None):
        print("    No resources section, skipping")
        return True

    inJsonResArray = inJsonData["resources"]
    outMetaJSON["resources"] = []

    allFiles = []  # Collect all files from resources (including folders)

    for r in inJsonResArray:
        print("    Processing resource entry: {}".format(r))

        absResPath = absProjectDir / r
        absResPath = Path(absResPath).resolve()

        if os.path.isfile(absResPath):
            allFiles.append((r, absResPath))
            print("      Added file: {}".format(r))
        elif os.path.isdir(absResPath):
            print("      Scanning folder: {}".format(r))
            folderFiles = ScanFolderRecursive(absProjectDir, r)
            allFiles.extend(folderFiles)
            print("      Found {} files in folder".format(len(folderFiles)))
        else:
            print("      ERROR: Resource {} is neither file nor folder at {}".format(r, absResPath))
            return False

    # In store format, separate .lua files from non-lua assets
    luaCodeFiles = []   # (relativePath, data) for Lua code bundle
    assetFiles = []     # (relativePath, absPath) for resource section

    if storeFormat:
        for relativePath, absResPath in allFiles:
            if relativePath.lower().endswith(".lua"):
                # Read Lua file now for the code bundle
                try:
                    with open(absResPath, "rb") as f:
                        data = f.read()
                    luaCodeFiles.append((relativePath, data))
                    print("    [STORE] Lua code file: {} ({} bytes)".format(relativePath, len(data)))
                except Exception as e:
                    print("Failed to read Lua file @ {}".format(absResPath))
                    print("Exception: {}".format(e))
                    return False
            else:
                assetFiles.append((relativePath, absResPath))
                print("    [STORE] Asset file: {}".format(relativePath))

        # Build the Lua code bundle for the ELF section
        if luaCodeFiles:
            sect_luaCodeBundle = BuildLuaCodeBundle(luaCodeFiles)
            print("    [STORE] Built Lua code bundle: {} bytes, {} files".format(
                len(sect_luaCodeBundle), len(luaCodeFiles)))

            if debugOutput:
                absFilePath = PrepDebugDir(absProjectDir, "lua_code_bundle.bin")
                with open(absFilePath, "wb") as f:
                    f.write(sect_luaCodeBundle)
                print("    DEBUG: Wrote {}".format(absFilePath))
        else:
            print("    [STORE] WARNING: No .lua files found in resources!")

        # Only pack non-lua assets into the resource section
        filesToPack = assetFiles
    else:
        # Standard format: all files go into resource section
        filesToPack = allFiles

    # Process files for the resource section
    for relativePath, absResPath in filesToPack:
        print("    Packing file: {}".format(relativePath))
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
                kvp = (relativePath, startOffset)
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
def CreateHeader(absProjectDir, relElfNameNoExt, sdkVersion):
    # type: (str, str) -> bool

    global headerVersion
    #
    global sect_header
    global sect_icon
    global sect_outMeta
    global sect_binding
    global sect_allResources
    global sect_luaCodeBundle
    global storeFormat
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

    # C-10: SDK version (major.minor.patch) + 1 reserved byte
    print("  Writing SDK version {}.{}.{} to header".format(
        sdkVersion[0], sdkVersion[1], sdkVersion[2]))
    sect_header.extend(sdkVersion[0].to_bytes(1, 'little'))
    sect_header.extend(sdkVersion[1].to_bytes(1, 'little'))
    sect_header.extend(sdkVersion[2].to_bytes(1, 'little'))
    sect_header.extend((0).to_bytes(1, 'little'))

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

    # save a little extra work
    unpaddedElfSize = len(sect_mainElf)
    PrintSectionSizes("Section sizes: (padded)")
    PadByteArray(sect_header, 512)
    PadByteArray(sect_icon, 512)
    PadByteArray(sect_outMeta, 512)
    PadByteArray(sect_binding, 512)
    PadByteArray(sect_allResources, 512)
    if storeFormat and len(sect_luaCodeBundle) > 0:
        PadByteArray(sect_luaCodeBundle, 512)
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

    # ELF section: empty for standard LUA apps, Lua code bundle for store format
    elfStart = len(finalBinary)
    if storeFormat and len(sect_luaCodeBundle) > 0:
        headerFieldPos += AddToArray(finalBinary, headerFieldPos, elfStart)
        headerFieldPos += AddToArray(finalBinary, headerFieldPos, len(sect_luaCodeBundle))
        finalBinary.extend(sect_luaCodeBundle)
        print("  Wrote ELF section (Lua code bundle) at pos {} size {}".format(
            hex(elfStart), hex(len(sect_luaCodeBundle))))
    else:
        headerFieldPos += AddToArray(finalBinary, headerFieldPos, elfStart)
        headerFieldPos += AddToArray(finalBinary, headerFieldPos, 0)  # Zero length
        print("  Wrote ELF section (empty) at pos {} size 0".format(hex(elfStart)))

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
