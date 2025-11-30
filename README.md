# Oscilloscope-Function-Generator

## Overview
This project demonstrates how to communicate with **FTDI USB devices** using the **FTD2XX driver** in C++.  
It provides object-oriented abstractions for reading and writing data to FTDI devices and includes a simple
command-line interface for testing and interaction.

The program allows you to:
- Initialize and configure FTDI devices.
- Send and receive data using `FTDWriter` and `FTDReader`.
- Run a **Driver Test** that reads one byte from an input file and writes it to an output file.

---

## Features

- **Device Initialization**: Opens, resets, purges, and configures FTDI devices.
- **Encapsulated I/O**:
  - `FTDWriter` — sends bytes to the device.
  - `FTDReader` — reads bytes from the device.
- **Error Handling**: Uses C++ exceptions for clean error reporting.
- **Driver Test Mode**:
  - Reads one byte from a file (`input.txt`).
  - Writes it through the FTDI device (simulated if only one handle is used).
  - Saves the result to an output file (`output.txt`).

---
## Build Instructions

Make sure the **FTDI D2XX library** is installed and available as `libftd2xx.a`. 

### macOS

You can then compile with: 
```make ``` 

Then run: 
```./main```

## Running the Program

### Interactive Menu Mode

When executed without arguments, the menu looks like this:

```
Control Menu:
- 1. Control LEDs
- 2. Send Morse Code
- 3. Write byte to port
- 4. Read byte from port
- 5. Driver Test
- 6. Exit
Enter your choice:
``` 

### Command-Line Interface (CLI) Mode

The program supports both file-based and direct command-line execution for automation and scripting.

#### File-Based Execution

Execute commands from a file:

```bash
./main commands.txt
```

#### Direct Command-Line Execution

You can also run commands directly from the command line:

```bash
./main start samples 10 write 0xFF 5 stop
```

Multiple commands can be chained together. The program will automatically detect if the first argument is a file (if it exists) or treat all arguments as commands.

#### Command Format

Commands are space-separated and case-insensitive. Each command should be on a single line. Comments start with `#`.

**Available Commands:**

- `start` - Start oscilloscope data collection
- `stop` - Stop oscilloscope data collection
- `read <count>` - Read N bytes from FTDI device (e.g., `read 64`)
- `write <byte> [count]` - Write byte value to FTDI device (e.g., `write 0xFF` or `write 0xFF 5`)
  - Byte can be specified in hex (0xFF, FF) or decimal (255)
  - Optional count parameter specifies how many times to write (default: 1)
- `samples <number> <interval>` - Configure oscilloscope sampling (e.g., `samples 10000 5ms`)
  - Interval supports `us`, `ms`, or `s` suffixes (default microseconds)
  - Oscilloscope must be running to begin collection; if already running, sampling begins immediately
- `readFile <filename>` - Read data from a file (e.g., `readFile input.txt`)
- `writeFile <filename>` - Write data to a file (e.g., `writeFile output.txt`)
- `scope start, sampleTime=X, wait=X, stop` - Multi-threaded scope collection with wait (e.g., `scope start, sampleTime=1ms, wait=5s, stop`)
  - `sampleTime` - Interval between samples (supports `us`, `ms`, `s` suffixes)
  - `wait` - Total collection duration (supports `us`, `ms`, `s` suffixes)
  - Displays scrolling data in terminal during collection
  - Uses separate threads for data collection and display

#### Example Command File

Create a file `commands.txt`:

```
# Start the oscilloscope
start

# Set number of samples to collect with 5 ms spacing
samples 10000 5ms

# Read 64 bytes from FTDI device
read 64

# Write byte 0xFF to FTDI device (once)
write 0xFF

# Write byte 0xAA to FTDI device 5 times
write 0xAA 5

# Stop the oscilloscope
stop

# Write collected data to file
writeFile output.txt
```

#### Command-Line Examples

```bash
# Single command
./main start

# Multiple commands
./main start samples 10 5ms write 0xFF 5 stop

# Write byte in different formats
./main write 0xFF        # Hex format
./main write FF          # Hex format (no 0x prefix)
./main write 255         # Decimal format
./main write 0xFF 10     # Write 0xFF ten times
```

#### Multi-Threaded Oscilloscope Data Collection

The oscilloscope functionality allows you to collect data from the FTDI device with multi-threading support:

**New Scope Command (Multi-threaded):**
```bash
./main scope start, sampleTime=1ms, wait=5s, stop
```

This command:
- Starts the oscilloscope
- Collects samples at the specified interval (1ms in this example)
- Continues collecting for the specified duration (5s in this example)
- Displays scrolling data in the terminal during collection
- Uses separate threads for data collection and display
- Stops automatically when the wait time expires

**Legacy Oscilloscope Data Collection:**

1. Start the oscilloscope with `start`
2. Configure sampling with `samples <number> <interval>` (e.g., `samples 500 2ms`)
3. If the oscilloscope is running when you set samples, data collection begins immediately
4. Stop with `stop`
5. Use `writeFile` to save collected data to a file


## Driver Test

The **Driver Test** performs a simulated transfer:

1. Reads **1 byte** from `input.txt`.
2. Writes it through the FTDI device (using the same handle if only one device is present).
3. Writes that byte to `output.txt`.

### Creating Raw Input Files

The Driver Test expects a **binary file**, not plain text.

#### On macOS / Linux:
```bash
printf "\xFF" > input.txt
```
You can replace "FF" with any other byte such as "00" or "F0"
### Verifying the Output

After running the Driver Test, you can check that the byte was successfully written to `output.txt`.

#### On macOS / Linux:
Use the `xxd` command to display the file contents in hexadecimal:

```bash
xxd output.txt
```
## Figma Design

A simple Figma mockup illustrates the interface and flow of the **Oscilloscope-Function-Generator** application.

You can view the design here:  
👉 [**View Figma Design**](https://www.figma.com/design/vYemvnTYgpOh9eQVtW2I5X/Osciloscope?node-id=0-1&p=f&t=51dGk6KWzvfs07sf-0)

### Preview

![Figma Preview](./images/OsciloscopeFigma.png)

