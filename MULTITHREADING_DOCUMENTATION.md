# Multi-Threading Implementation Documentation

## Overview

This document describes the multi-threaded implementation of the Oscilloscope wait function. The implementation uses separate threads for data collection and display, allowing real-time data visualization while collecting samples from the FTDI device.

## Class Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    FTDOscilloscopeThreaded                  │
├─────────────────────────────────────────────────────────────┤
│ - reader: FTDReader*                                         │
│ - writer: FTDWriter*                                         │
│ - running: atomic<bool>                                      │
│ - sampleCount: atomic<size_t>                                │
│ - collectedData: vector<unsigned char>                      │
│ - dataMutex: mutex                                           │
│ - collectionThread: thread                                   │
│ - displayThread: thread                                      │
│ - currentSampleIntervalMicros: int                          │
│ - currentWaitTimeMicros: int                                 │
├─────────────────────────────────────────────────────────────┤
│ + start()                                                     │
│ + stop()                                                      │
│ + collectDataWithWait(sampleIntervalMicros, waitTimeMicros)│
│ + isRunning(): bool                                          │
│ + getCollectedData(): vector<unsigned char>                 │
│ + clearData()                                                │
│ + getSampleCount(): size_t                                   │
│ - collectionThreadFunction()                                 │
│ - displayThreadFunction()                                    │
│ - updateDisplay(latestData)                                  │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ uses
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                        FTDController                         │
├─────────────────────────────────────────────────────────────┤
│ - oscilloscopeThreaded: unique_ptr<FTDOscilloscopeThreaded>  │
├─────────────────────────────────────────────────────────────┤
│ + runScopeWithWait(sampleIntervalMicros, waitTimeMicros)    │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ executes
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                        ScopeCommand                          │
├─────────────────────────────────────────────────────────────┤
│ - sampleIntervalMicros: int                                  │
│ - waitTimeMicros: int                                       │
├─────────────────────────────────────────────────────────────┤
│ + execute(controller: FTDController*)                       │
│ + getName(): string                                          │
└─────────────────────────────────────────────────────────────┘
```

## Activity Diagram - Scope Collection with Wait

```
[Start] 
   │
   ▼
[Parse Command: scope start, sampleTime=1ms, wait=5s, stop]
   │
   ▼
[Create ScopeCommand with parameters]
   │
   │
   ▼
[FTDController.runScopeWithWait()]
   │
   ▼
[FTDOscilloscopeThreaded.start()]
   │
   ├─────────────────────────────────────┐
   │                                     │
   ▼                                     ▼
[Start Collection Thread]        [Start Display Thread]
   │                                     │
   │                                     │
   ▼                                     ▼
[Loop: while running && time < wait]    [Loop: while running]
   │                                     │
   │                                     │
   ▼                                     ▼
[Read from FTDI Device]          [Sleep 50ms]
   │                                     │
   ▼                                     ▼
[Lock mutex]                      [Lock mutex]
   │                                     │
   ▼                                     ▼
[Store data in collectedData]    [Get latest data]
   │                                     │
   ▼                                     ▼
[Unlock mutex]                    [Unlock mutex]
   │                                     │
   ▼                                     ▼
[Increment sampleCount]           [Update terminal display]
   │                                     │
   ▼                                     ▼
[Sleep sampleIntervalMicros]     [Check for keyboard input]
   │                                     │
   │                                     │
   └─────────────┬───────────────────────┘
                 │
                 ▼
         [Time expired?]
                 │
         ┌───────┴───────┐
         │              │
      Yes│              │No
         │              │
         ▼              │
    [Set running=false]│
         │              │
         └──────┬───────┘
                │
                ▼
        [Join threads]
                │
                ▼
        [Stop oscilloscope]
                │
                ▼
            [End]
```

## Thread Synchronization

The implementation uses the following synchronization mechanisms:

1. **std::atomic<bool> running**: Controls thread lifecycle
2. **std::atomic<size_t> sampleCount**: Thread-safe sample counter
3. **std::mutex dataMutex**: Protects shared `collectedData` vector
4. **std::thread collectionThread**: Data collection thread
5. **std::thread displayThread**: Display update thread

## Command Syntax

The new scope command supports the following syntax:

```
scope start, sampleTime=1ms, wait=5s, stop
```

Where:
- `start` - Starts the oscilloscope
- `sampleTime=X` - Sets the interval between samples (supports ms, us, s)
- `wait=X` - Sets the total collection duration (supports ms, us, s)
- `stop` - Stops the oscilloscope

## Usage Example

```bash
main.exe scope start, sampleTime=1ms, wait=5s, stop
```

This will:
1. Start the oscilloscope
2. Collect samples every 1 millisecond
3. Continue collecting for 5 seconds
4. Display scrolling data in the terminal during collection
5. Stop the oscilloscope

## Implementation Details

### Collection Thread
- Reads data from FTDI device at specified intervals
- Stores data in thread-safe vector
- Continues until wait time expires or stopped

### Display Thread
- Updates terminal display every 50ms
- Shows sample count, byte count, and latest data values
- Allows early termination via keyboard input (Windows)

### Thread Safety
- All shared data access is protected by mutexes
- Atomic variables used for counters and flags
- Proper thread joining in destructor

