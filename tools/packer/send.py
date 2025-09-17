#!/usr/bin/env python3
"""
@file send.py
@brief VMUPro Serial Communication Tool

This script provides functionality to upload .vmupack files to VMUPro devices
over serial connection and provides a 2-way serial monitor for debugging.

Features:
- File upload with chunked transfer
- Device reset capability  
- Auto-execution of uploaded applications
- Interactive 2-way serial monitor
- Progress tracking and error handling

Usage:
    Upload file: python send.py --func send --localfile app.vmupack --remotefile apps/app.vmupack --comport COM3 --exec true
    Reset device: python send.py --func reset --comport COM3

@author 8BitMods
@version 1.0.0
@date 2025-06-23
@copyright Copyright (c) 2025 8BitMods. All rights reserved.
"""

# 8BM Copyright/License notice
# For use with ESP IDF Python 3.10.x

import sys
import serial
import time
import argparse

# soz, but it's more portable lol
import queue
import threading
import struct

# safest windows way to get keyb input
if sys.platform == "win32":
    import msvcrt
# for linux/osx we'll use curses,


# 8 char input buffer
# So every time we clock in a byte, we check if it
# contains a valid command like "SEND_BIN" or "MOREDATA"
inputbuffer = [''] * 8
# Serial object
uart = None
# Key queue for input thread
keyQueue = queue.Queue()
debugMode = False

CHUNK_SIZE = 2048 * 8

def ListenerThread():
    """ Input listener thread, to prevent blocking serial """
    # not required for msvcrt, but is for *nix

    if sys.platform == "win32":
        while True:
            if msvcrt.kbhit():
                key = msvcrt.getch()
                if key == b'\x00' or key == b'\xE0':
                    msvcrt.getch()
                    continue
                keyQueue.put(key)
    
            time.sleep(0.01)
    else:
        while True:
            time.sleep(0.01)


def AddToBuffer(newChar):
    """
    Add each char to the fifo buffer as we read it in
    so after each byte we can check if it matches a
    known command such as "SEND_BIN" or "MOREDATA"
    while filtering noise bytes before and after
    """

    global inputbuffer

    bLen = len(inputbuffer)
    for i in range(0, bLen - 1):
        inputbuffer[i] = inputbuffer[i+1]

    inputbuffer[bLen-1] = newChar


def Monitor2Way():
    """
    2-Way serial monitor, can type to send keystrokes
    displays read input per-line, not per character
    """

    if uart.in_waiting:
        line = uart.readline()
        if line:
            decoded = line.decode(errors='replace')
            decoded = decoded.strip()
            print("Received:", decoded)

    while not keyQueue.empty():
        key = keyQueue.get()
        if key == '\x1B':  # escape
            raise KeyboardInterrupt
        uart.write(key)
        # print(f"Sent: {key!r}")


def LoopMonitorMode(acceptInput):

    if acceptInput:
        print("PC: Entering 2-way monitor mode")
        print("PC: You may send keystrokes to the app")
        print("PC: Ctrl+C / ESC to exit")
    else:
        print("PC: No --monitor flag provided")
        print("PC: The console will not send keystrokes to the VMUPro")
        print("PC Ctrl+C / ESC to exit")

    while True:
        Monitor2Way()


def MonitorBytes():
    """ 
    Read incoming bytes from the VMUPro.
    Automatically added to the fifo buffer
    So you can 
    A: see incoming bytes from VMUPro printed to screen
    B: check if the last x fifo buffer bytes were a command like SEND_BIN
    """

    if uart.in_waiting:
        charBytes = uart.read()

        if charBytes:
            inLen = len(charBytes)
            for i in range(0, inLen):

                charVal = chr(charBytes[i])
                if debugMode:
                    sys.stdout.write(charVal)
                    sys.stdout.flush()
                AddToBuffer(charVal)


def ClearInputBuffer():
    """Read input from serial untill it's empty"""

    if debugMode:
        print("  Clearing input buffer..")
    while uart.in_waiting:
        MonitorBytes()


def CheckBufferForCommand(inCommand):
    # type: (str)->bool
    """
    Check if the last few bytes from the VMUPro
    match the given input command.
    E.g. "SEND_BIN", "REQ_DATA" etc
    """

    bLen = len(inputbuffer)
    inLen = len(inputbuffer)

    if (bLen != inLen):
        print("Error mismatched buffer length check")
        return

    for i in range(0, bLen):
        if inputbuffer[i] != inCommand[i]:
            return False

    return True


def WriteBytes(inBytes):
    # type: (bytes)->None
    if debugMode:
        print(f"  Writing {inBytes} to com port")
    uart.write(inBytes)
    uart.flush()


def WriteUInt32(inVal):
    # type (int)->None
    if debugMode:
        print(f"  Writing UInt32 {hex(inVal)}")
    data = struct.pack('<I', inVal)
    uart.write(data)


def WaitForResponse(matchString, failStringOrNone):
    # type: (str, str)->bool
    """
    Wait for a specific response from the VMUPro
    to move to the next stage in a sequence.    
    returns True if matchString found, e.g. "MOREDATA"
    returns False if failString found, e.g. "FILE_ERR"
    """

    print(f"  Waiting for response {matchString} from VMUPRO")
    while True:
        MonitorBytes()
        if CheckBufferForCommand(matchString):
            if debugMode:
                print(f"\n  PC: Got {matchString} response from VMUPro")
            ClearInputBuffer()
            return True
        if not failStringOrNone == None:
            if CheckBufferForCommand(failStringOrNone):
                print(f"\n  PC: FAIL: {matchString} response from VMUPro")
                return False


def ErrorUnknownCommand(inString):
    print(
        f"The VMUPro doesn't recognise the command {inString}, please update firmware and/or SDK!")


def ErrorHandlingFile():
    print("The VMUPro failed to handle the filename, see console for details")


def WriteBytesChunked(inBytes, chunkSize):

    bytesSent = 0
    totalBytes = len(inBytes)
    chunkCounter = 0
    maxChunks = (totalBytes/chunkSize)
    while bytesSent < totalBytes:

        print(f"PC: Writing chunk {chunkCounter} / {maxChunks}")

        bytesLeft = totalBytes - bytesSent
        thisChunkSize = chunkSize
        if (thisChunkSize > bytesLeft):
            thisChunkSize = bytesLeft

        chunk = inBytes[bytesSent: bytesSent + thisChunkSize]

        if debugMode:
            print(f"PC: Using chunk size {thisChunkSize}")
        uart.write(chunk)
        uart.flush()
        bytesSent += len(chunk)

        print(f"PC: Sent: {bytesSent} of {totalBytes}")

        if (bytesSent < totalBytes):
            WaitForResponse("MOREDATA", None)

        chunkCounter += 1

    print(f"\n\nPC: Sent {bytesSent} bytes")


def main():

    print("\n")
    print("8BM VMUPro Serial Tool")
    print("Run py send.py-h for help or a full list of supported arguments")
    print("\n")

    print("Executing command line args:")
    print("  ".join(sys.argv))
    print("\n")

    # Pre-parse the args for a known command
    # e.g. "send" or "reset"
    # ignore others as we'll reparse those per-command

    parser = argparse.ArgumentParser(
        description="VMUPro serial functions:")
    parser.add_argument("--func", required=True,
                        help="Function such as send, reset")

    args, unknownArgs = parser.parse_known_args()

    func = args.func

    if func == "send":
        SendFile()
    elif func == "reset":
        ResetVMUPro()
    else:
        print("Unknown command: {}".format(args.func))
        sys.exit(1)


def ResetVMUPro():

    global uart

    """Reset the VMUPro simply by opening a serial connection with RTS and DTR"""

    parser = argparse.ArgumentParser(
        description="Send a file to the VMUPro SD card")
    parser.add_argument("--func", required=True,
                        help="e.g. reset")

    parser.add_argument("--comport", required=False,
                        help="e.g. COM18, /dev/ttyxxx")

    args = parser.parse_args()

    comPort = CheckComPort(args)

    try:
        uart = serial.Serial(
            port=comPort,
            baudrate=115200,
            dsrdtr=None,  # Prevent pyserial from asserting DSR/DTR control lines
            timeout=1
        )

        # With the ESP Prog over JTAG, this should be enough
        # to reset the ESP electrically
        uart.setRTS(True)
        uart.setDTR(True)

    except Exception as e:
        print("\nError initing the serial port: {}".format(e))
        print("Hint: is the ESP IDF or another console using the COM port?\n")
    finally:
        if not uart == None:
            uart.close()


def SaveComPort(comport):
    # type: (str) -> None

    print("Saving comport {} to comport.txt".format(comport))

    try:
        with open("comport.txt", "w") as f:
            f.write(comport)
    except Exception as e:
        print("Unable to write to comport.txt: {}".format(
            e))
        print("Please ensure that the file is not currently open!")


def LoadComPort():
    # type: () -> str

    print("Checking if comport param is saved in comport.txt...")

    outVal = ""
    try:
        with open("comport.txt", "r") as f:
            outVal = f.readline().strip()
            print("Using --comport {} from comport.txt".format(outVal))
            return outVal
    except Exception as e:
        print("Unable to read from comport.txt (it may not exist)")
        return ""


def CheckComPort(args):
    # type: (ArgumentParser) -> str

    # First, did the user provide a com port?

    argVal = args.comport

    if argVal:
        SaveComPort(argVal)
        return argVal

    # Second, do we already have a saved value?

    argVal = LoadComPort()
    if argVal:
        return argVal
    
    # Third, just ask the user

    print("No --comport param provided, please type it in now")
    print("On Windows it will be something like COM4, COM19, etc")
    print("(found via devmgmt.msc)")
    print("On unix-like systems it may be /dev/cu.usbmodem101 or /dev/ttyXXXX")

    argVal = input(":")

    SaveComPort(argVal)
    return argVal


def SendFile():

    global uart

    """
    Send a file over serial with a PC-side (local) 
    and VMUPro-side (remote) file name.
    """

    # We'll reparse the args for this specific command
    parser = argparse.ArgumentParser(
        description="Send a file to the VMUPro SD card")
    parser.add_argument("--func", required=True,
                        help="e.g. send")
    parser.add_argument("--localfile", required=True,
                        help="e.g. myfile.vmupack from the PC")
    parser.add_argument("--remotefile", required=True,
                        help="e.g. test.vmupack on the SD card")

    parser.add_argument("--comport", required=False,
                        help="e.g. COM18, /dev/ttyxxx")

    parser.add_argument("--exec", action='store_true', required=False,
                        help="Execute afterwards")

    parser.add_argument("--debug", action='store_true', required=False, default=False,
                        help="Extra debug spam")

    parser.add_argument("--monitor", action='store_true', required=False,
                        default=False, help="Open a 2-way console to the VMU pro")

    args = parser.parse_args()
    localFile = args.localfile
    remoteFile = args.remotefile
    comPort = CheckComPort(args)
    debugMode = args.debug
    acceptInput = args.monitor

    try:

        # Start the listen thread...
        if acceptInput:
            threadArgs = tuple()
            t = threading.Thread(target=ListenerThread,
                                 args=threadArgs, daemon=True)
            t.start()

        # Init the serial connection
        uart = serial.Serial(
            port=comPort,
            baudrate=921600,
            dsrdtr=None,
            timeout=1
        )

        # Prevent immediately restarting the VMUPro
        uart.setRTS(False)
        uart.setDTR(False)

        with open(localFile, "rb") as f:

            # Load file

            print("PC: Loading file...")
            bytes = f.read()
            fileSize = len(bytes)
            print(f"  Loaded {fileSize} bytes from {localFile}")

            ClearInputBuffer()

            # Enter serial mode
            # and send the "SEND_BIN" command

            print("PC: Triggering sio mon")
            WriteBytes(b'X')

            print("PC: Sending command")
            WriteBytes(b'SEND_BIN')

            # Wait for VMUPro to react with "REQ_SIZE"
            # then send the size

            if not WaitForResponse("REQ_SIZE", "UNK_CMD!"):
                ErrorUnknownCommand("SEND_BIN")
                sys.exit(1)

            print("PC: Sending file size")
            WriteUInt32(fileSize)

            # Wait for the VMUPro to react with "REQ_NAME"
            # for the filename on the SD card

            WaitForResponse("REQ_NAME", None)

            WriteBytes(remoteFile.encode('ascii'))
            WriteBytes(b'\0')

            if not WaitForResponse("REQ_DATA", "FILE_ERR"):
                ErrorHandlingFile()
                sys.exit(1)

            # Send the file contents
            # in chunks of CHUNK_SIZE bytes

            print("PC: Sending file")
            WriteBytesChunked(bytes, CHUNK_SIZE)

            # Wait for the VMUPro to ask if we want
            # to execute the file, and send a response
            WaitForResponse("ASK_EXEC", None)

            if args.exec:
                WriteUInt32(1)
            else:
                WriteUInt32(0)

            # We're done
            # Open a 2-way serial

            LoopMonitorMode(acceptInput)

        uart.close()

    except OSError as e:
        print(f"\nError opening file: {e}")
        print("Hint: is the ESP IDF or another console using the COM port?\n")
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(2)
    except KeyboardInterrupt:
        print("\nExiting.")
    finally:
        if not uart == None:
            uart.close


if __name__ == "__main__":
    main()
