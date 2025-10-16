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

## File Structure

