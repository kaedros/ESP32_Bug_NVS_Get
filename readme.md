## ESP32 NVS_Get_i32

## Solution
The friends from Espressif warned us that the program starts running on the board immediately after flashing and when it time to open the monitor the value is already set.
To work around this, either develop a timeguard on boot or use simple_monitor after flash.

## Problem Description
In this code, we are trying to read a key on the NVS partition. It initially doesn't exist in the flash and we want to initialize it as a certain value.
After running the software for the first time, the algorithm will act as if the value were already in the memory, even though the memory is blank.

## Expected Behavior
The first time you run the software, it must identify that the value does not exist in the partition, and initialize with the determined value, stating that there was no key but that it was initialized with the value 123.
The second time you run the software, it must identify the existence of the key and present the value.

## Actual Behavior
At de first time we run the software, it show a value that has not yet been initialized in the partition.

# How to Reproduce The Error
## Environment
- Development Kit: none
- Module or chip used: ESP32-WROOM-32
- IDF version: v4.0-138-g024a2755b
- Build System: Make
- Compiler version: xtensa-esp32-elf-gcc (crosstool-NG esp32-2019r2) 8.2.0
- Operating System:	Linux
- Power Supply: external 3.3V

## Steps to Reproduce
### Initial Setup
1. Erase the ESP32 Flash using ```make -j9 erase_flash``` command
2. Read the NVS partition using ```esptool.py -p /dev/ttyS10 -b 921600 read_flash 0x9000 0x6000 dumpflash01.bin``` command to confirm it is empty
3. Compile, Flash and open the serial monitor using ```make -j9 flash monitor``` command
4. Read the NVS partition using ```esptool.py -p /dev/ttyS10 -b 921600 read_flash 0x9000 0x6000 dumpflash02.bin``` command to confirm the writted value

### Runtime
In order to reproduce the problem, a demo will run after the boot. This demo tries read a key to the NVS partition that does not yet exist.

#### First Run
**In Step 1**, it is possible to see that the nvs partition is blank after using the command  `make -j9 erase_flash` and `esptool.py -p /dev/ttyS10 -b 921600 read_flash 0x9000 0x6000 dumpflash01.bin`. And And it is possible to read the file with the `xxd -p dumpflash01.bin` command:
```
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
...
```
**In Step 2**, the run demo should check if the key exists and, since it doesn't, it should print the message and create the key on the NVS, but instead the value is shown as if it was already recorded with the key:
```
I (0) cpu_start: App cpu up.
I (241) heap_init: Initializing. RAM available for dynamic allocation:
I (248) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (254) heap_init: At 3FFB3128 len 0002CED8 (179 KiB): DRAM
I (260) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (266) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (273) heap_init: At 40089FA8 len 00016058 (88 KiB): IRAM
I (279) cpu_start: Pro cpu start user code
I (297) spi_flash: detected chip: gd
I (298) spi_flash: flash io: dio
I (298) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
The read value is 123 [getVar]
```
It was expected:
```
I (0) cpu_start: App cpu up.
I (241) heap_init: Initializing. RAM available for dynamic allocation:
I (248) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (254) heap_init: At 3FFB3128 len 0002CED8 (179 KiB): DRAM
I (260) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (266) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (273) heap_init: At 40089FA8 len 00016058 (88 KiB): IRAM
I (279) cpu_start: Pro cpu start user code
I (297) spi_flash: detected chip: gd
I (298) spi_flash: flash io: dio
I (298) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
The value is not initialized yet!
The written value is 123
```

**In Step 3**, it can be seen that now, the `nvs` partition has the page and a key with the value 0x7b [123] after command `esptool.py -p /dev/ttyS10 -b 921600 read_flash 0x9000 0x6000 dumpflash02.bin`. And we can read a file with `xxd -p dumpflash02.bin` command:
```
feffffff00000000feffffffffffffffffffffffffffffffffffffff842d
bab9faffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffff000101ffdf720a8553544f5241474500000000000000000001ff
ffffffffffff011401ff18b8c3424b657956616c75650000000000000000
7b000000ffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
...
```