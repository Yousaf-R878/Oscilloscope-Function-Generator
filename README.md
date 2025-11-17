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

The program supports file-based command execution for automation and scripting. You can execute commands from a file:

```bash
./main commands.txt
```

#### Command Format

Commands are space-separated and case-insensitive. Each command should be on a single line. Comments start with `#`.

**Available Commands:**

- `start` - Start oscilloscope data collection
- `stop` - Stop oscilloscope data collection
- `read <count>` - Read N bytes from FTDI device (e.g., `read 64`)
- `write <count>` - Write N bytes to FTDI device (e.g., `write 64`)
- `samples <number>` - Set number of samples for oscilloscope (e.g., `samples 10000`)
  - If oscilloscope is running, data collection will begin immediately
- `readFile <filename>` - Read data from a file (e.g., `readFile input.txt`)
- `writeFile <filename>` - Write data to a file (e.g., `writeFile output.txt`)

#### Example Command File

Create a file `commands.txt`:

```
# Start the oscilloscope
start

# Set number of samples to collect
samples 10000

# Read 64 bytes from FTDI device
read 64

# Write 64 bytes to FTDI device
write 64

# Stop the oscilloscope
stop

# Write collected data to file
writeFile output.txt
```

Then execute:
```bash
./main commands.txt
```

#### Oscilloscope Data Collection

The oscilloscope functionality allows you to collect data from the FTDI device:

1. Start the oscilloscope with `start`
2. Set the number of samples with `samples <number>`
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

