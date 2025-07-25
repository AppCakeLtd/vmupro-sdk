/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "VMUPro SDK", "index.html", [
    [ "Table of Contents", "index.html#autotoc_md1", null ],
    [ "Documentation", "index.html#autotoc_md2", [
      [ "API Reference", "index.html#autotoc_md3", null ]
    ] ],
    [ "Prerequisites", "index.html#autotoc_md4", null ],
    [ "Installation", "index.html#autotoc_md5", [
      [ "1. Download and Install ESP-IDF", "index.html#autotoc_md6", null ],
      [ "2. Set Up Environment Variables", "index.html#autotoc_md7", null ],
      [ "3. Clone the VMUPro SDK", "index.html#autotoc_md8", null ]
    ] ],
    [ "Getting Started", "index.html#autotoc_md9", [
      [ "Project Structure", "index.html#autotoc_md10", null ]
    ] ],
    [ "Examples", "index.html#autotoc_md11", [
      [ "Minimal Example", "index.html#autotoc_md12", null ],
      [ "Graphics Samples Example", "index.html#autotoc_md13", null ],
      [ "Platformer Example", "index.html#autotoc_md14", null ]
    ] ],
    [ "Building Applications", "index.html#autotoc_md15", [
      [ "1. Navigate to Example Directory", "index.html#autotoc_md16", null ],
      [ "2. Set Up ESP-IDF Environment", "index.html#autotoc_md17", null ],
      [ "3. Build the Application", "index.html#autotoc_md18", null ],
      [ "4. Clean Build (if needed)", "index.html#autotoc_md19", null ]
    ] ],
    [ "Packaging Applications", "index.html#autotoc_md20", [
      [ "Prerequisites for Packaging", "index.html#autotoc_md21", null ],
      [ "Creating a .vmupack File", "index.html#autotoc_md22", null ],
      [ "Metadata Configuration", "index.html#autotoc_md23", null ]
    ] ],
    [ "Deploying Applications", "index.html#autotoc_md24", [
      [ "Prerequisites for Deployment", "index.html#autotoc_md25", null ],
      [ "Quick Deployment (Recommended)", "index.html#autotoc_md26", null ],
      [ "Manual Deployment with send.py", "index.html#autotoc_md27", [
        [ "Upload and Execute Application", "index.html#autotoc_md28", null ],
        [ "Upload Without Executing", "index.html#autotoc_md29", null ],
        [ "Reset VMUPro Device", "index.html#autotoc_md30", null ]
      ] ],
      [ "Serial Communication Features", "index.html#autotoc_md31", [
        [ "2-Way Serial Monitor", "index.html#autotoc_md32", null ],
        [ "Chunked File Transfer", "index.html#autotoc_md33", null ],
        [ "Error Handling", "index.html#autotoc_md34", null ]
      ] ],
      [ "Finding Your COM Port", "index.html#autotoc_md35", null ],
      [ "Complete Development Workflow", "index.html#autotoc_md36", null ]
    ] ],
    [ "IDE Integration", "index.html#autotoc_md37", [
      [ "Visual Studio Code", "index.html#autotoc_md38", [
        [ "Method 1: ESP-IDF Extension", "index.html#autotoc_md39", null ],
        [ "Method 2: Custom Tasks", "index.html#autotoc_md40", null ]
      ] ]
    ] ],
    [ "Troubleshooting", "index.html#autotoc_md41", [
      [ "Common Issues", "index.html#autotoc_md42", [
        [ "ESP-IDF Environment Not Set", "index.html#autotoc_md43", null ],
        [ "Python Virtual Environment Issues", "index.html#autotoc_md44", null ],
        [ "Build Failures", "index.html#autotoc_md45", null ],
        [ "Packaging Errors", "index.html#autotoc_md46", null ],
        [ "Deployment Issues", "index.html#autotoc_md47", null ]
      ] ],
      [ "Getting Help", "index.html#autotoc_md48", null ]
    ] ],
    [ "Standard C Library Functions", "md_docs_2stdlib__functions.html", [
      [ "Table of Contents", "md_docs_2stdlib__functions.html#autotoc_md53", null ],
      [ "File I/O Functions", "md_docs_2stdlib__functions.html#autotoc_md54", [
        [ "fopen - Open a file", "md_docs_2stdlib__functions.html#autotoc_md55", null ],
        [ "fclose - Close a file", "md_docs_2stdlib__functions.html#autotoc_md56", null ],
        [ "fread - Read data from file", "md_docs_2stdlib__functions.html#autotoc_md57", null ],
        [ "fwrite - Write data to file", "md_docs_2stdlib__functions.html#autotoc_md58", null ],
        [ "fseek - Set file position", "md_docs_2stdlib__functions.html#autotoc_md59", null ],
        [ "ftell - Get file position", "md_docs_2stdlib__functions.html#autotoc_md60", null ],
        [ "fflush - Flush file buffer", "md_docs_2stdlib__functions.html#autotoc_md61", null ],
        [ "fsync - Sync file to storage", "md_docs_2stdlib__functions.html#autotoc_md62", null ]
      ] ],
      [ "String and Memory Functions", "md_docs_2stdlib__functions.html#autotoc_md63", [
        [ "strlen - Get string length", "md_docs_2stdlib__functions.html#autotoc_md64", null ],
        [ "strcmp - Compare strings", "md_docs_2stdlib__functions.html#autotoc_md65", null ],
        [ "strchr - Find character in string", "md_docs_2stdlib__functions.html#autotoc_md66", null ],
        [ "strrchr - Find last character in string", "md_docs_2stdlib__functions.html#autotoc_md67", null ],
        [ "strstr - Find substring", "md_docs_2stdlib__functions.html#autotoc_md68", null ],
        [ "strdup - Duplicate string", "md_docs_2stdlib__functions.html#autotoc_md69", null ],
        [ "strcspn - Get span of string not containing chars", "md_docs_2stdlib__functions.html#autotoc_md70", null ],
        [ "strncat - Concatenate strings with limit", "md_docs_2stdlib__functions.html#autotoc_md71", null ],
        [ "strncpy - Copy string with limit", "md_docs_2stdlib__functions.html#autotoc_md72", null ],
        [ "strerror - Get error message string", "md_docs_2stdlib__functions.html#autotoc_md73", null ],
        [ "memcmp - Compare memory blocks", "md_docs_2stdlib__functions.html#autotoc_md74", null ],
        [ "memcpy - Copy memory", "md_docs_2stdlib__functions.html#autotoc_md75", null ],
        [ "memset - Fill memory", "md_docs_2stdlib__functions.html#autotoc_md76", null ]
      ] ],
      [ "Memory Allocation Functions", "md_docs_2stdlib__functions.html#autotoc_md77", [
        [ "malloc - Allocate memory", "md_docs_2stdlib__functions.html#autotoc_md78", null ],
        [ "calloc - Allocate zeroed memory", "md_docs_2stdlib__functions.html#autotoc_md79", null ],
        [ "realloc - Reallocate memory", "md_docs_2stdlib__functions.html#autotoc_md80", null ],
        [ "free - Free memory", "md_docs_2stdlib__functions.html#autotoc_md81", null ]
      ] ],
      [ "I/O Functions", "md_docs_2stdlib__functions.html#autotoc_md82", [
        [ "printf - Print to stdout", "md_docs_2stdlib__functions.html#autotoc_md83", null ],
        [ "fprintf - Print to file", "md_docs_2stdlib__functions.html#autotoc_md84", null ],
        [ "vfprintf - Print to file with va_list", "md_docs_2stdlib__functions.html#autotoc_md85", null ],
        [ "puts - Write string to stdout", "md_docs_2stdlib__functions.html#autotoc_md86", null ],
        [ "putchar - Write character to stdout", "md_docs_2stdlib__functions.html#autotoc_md87", null ],
        [ "fputc - Write character to file", "md_docs_2stdlib__functions.html#autotoc_md88", null ],
        [ "fputs - Write string to file", "md_docs_2stdlib__functions.html#autotoc_md89", null ]
      ] ],
      [ "Conversion Functions", "md_docs_2stdlib__functions.html#autotoc_md90", [
        [ "strtol - String to long", "md_docs_2stdlib__functions.html#autotoc_md91", null ],
        [ "strtod - String to double", "md_docs_2stdlib__functions.html#autotoc_md92", null ],
        [ "abs - Absolute value", "md_docs_2stdlib__functions.html#autotoc_md93", null ]
      ] ],
      [ "Time Functions", "md_docs_2stdlib__functions.html#autotoc_md94", [
        [ "sleep - Sleep seconds", "md_docs_2stdlib__functions.html#autotoc_md95", null ],
        [ "usleep - Sleep microseconds", "md_docs_2stdlib__functions.html#autotoc_md96", null ],
        [ "clock_gettime - Get clock time", "md_docs_2stdlib__functions.html#autotoc_md97", null ],
        [ "strftime - Format time string", "md_docs_2stdlib__functions.html#autotoc_md98", null ]
      ] ],
      [ "Thread Functions", "md_docs_2stdlib__functions.html#autotoc_md99", [
        [ "pthread_create - Create thread", "md_docs_2stdlib__functions.html#autotoc_md100", null ],
        [ "pthread_join - Join thread", "md_docs_2stdlib__functions.html#autotoc_md101", null ],
        [ "pthread_detach - Detach thread", "md_docs_2stdlib__functions.html#autotoc_md102", null ],
        [ "pthread_exit - Exit thread", "md_docs_2stdlib__functions.html#autotoc_md103", null ],
        [ "pthread_attr_init - Initialize thread attributes", "md_docs_2stdlib__functions.html#autotoc_md104", null ],
        [ "pthread_attr_setstacksize - Set thread stack size", "md_docs_2stdlib__functions.html#autotoc_md105", null ]
      ] ],
      [ "Miscellaneous Functions", "md_docs_2stdlib__functions.html#autotoc_md106", [
        [ "exit - Exit program", "md_docs_2stdlib__functions.html#autotoc_md107", null ],
        [ "abort - Abort program", "md_docs_2stdlib__functions.html#autotoc_md108", null ],
        [ "close - Close file descriptor", "md_docs_2stdlib__functions.html#autotoc_md109", null ],
        [ "setjmp - Save program state", "md_docs_2stdlib__functions.html#autotoc_md110", null ],
        [ "longjmp - Non-local jump", "md_docs_2stdlib__functions.html#autotoc_md111", null ]
      ] ],
      [ "Data Integrity Functions", "md_docs_2stdlib__functions.html#autotoc_md112", [
        [ "crc32 - Calculate CRC32 checksum", "md_docs_2stdlib__functions.html#autotoc_md113", null ]
      ] ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"_2Users_2thanos_2Projects_28BitMods_2vmupro-sdk_2sdk_2include_2vmupro_audio_8h-example.html",
"vmupro__display_8h.html#a86705245e60792886421760670ae1fc6"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';