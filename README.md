# FCB1010 Editor/Tools

## Overview

This project provides tools for managing the Behringer FCB1010 MIDI Foot Controller. It allows users to receive, send, inspect, and edit SysEx data dumps, as well as convert SysEx data to and from CSV format. The project is written in C and uses the `ncurses` library for a terminal-based user interface.

## Features

- **Receive SysEx Dump:** Capture SysEx data from the FCB1010 and save it to a file.
- **Send SysEx Dump:** Send a SysEx file to the FCB1010.
- **Parse and Inspect SysEx Data:** Inspect the SysEx data in a readable format.
- **Create CSV from SysEx:** Convert SysEx data to a CSV file for easy editing.
- **Create SysEx from CSV:** Generate a SysEx file from a CSV input.
- **Backup SysEx Files:** Automatically back up SysEx files with timestamped filenames.

## Installation
This project comes with a binary for x64 linux built on Debian 12 stable
you will need to run `make clean` prior to `make` to remove it

1. **Clone the repository:**
    ```sh
    git clone https://github.com/trafficpest/fcbtool.git
    cd fcbtool
    ```

2. **Build the project:**
    ```sh
    make
    ```

3. **Locate the binary:**
   The binary will be generated in the `./build/bin/` directory.

- Optional
    If you would like to install the app to the system place it in you bin PATH
    System wide: `sudo cp ./build/bin/fcbtool /usr/local/bin`
    or to User `cp ./build/bin/fcbtool ~/<path to your user bin dir>`
## Usage

Run the application from the terminal:

```sh
./build/bin/fcbtool
```

Use the on-screen menu to select the desired operation.

## File Structure
- **SysEx and CSV Files:** All generated SysEx and CSV files are stored in `~/.fcb1010/`.
- **Backup Files:** Backup files are saved in `~/.fcb1010/backups/` with a `yymmdd_hhmm.syx` format.

## Dumps
- In the dump folder you will find the three default sysex dumps for the device.

## Acknowledgments
This project was greatly helped by the work of Brian Walton's (brian@riban.co.uk) Python functions, which were converted to C for this application.

## License
MIT License

Copyright (c) 2024 Jason March 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
