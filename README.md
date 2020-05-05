This is a clone of [kmackay/micro-ecc](https://github.com/kmackay/micro-ecc)

with added benchmark example for Arduino

## Benchmark Results 

ATmega328P 16MHz with Arduino 1.8.12 Compiler

Code size is measured as the increase in code size vs a dummy program.

Runtime is for one uECC_make_key() or one uECC_shared_secret() call, both appear to take the same time.

secp256k1 opt=1 gives the smallest code size (!)

|Curve + Optimizion|Code Size (bytes)|Runtime (ms)|
|:--:|--:|--:|
|secp160r1 opt=0|5556|67780|
|secp160r1 opt=1|5256|1380|
|secp160r1 opt=2|6896|1020|
|secp160r1 opt=3|16718|550|
||||
|secp192r1 opt=1|5264|1860|
|secp192r1 opt=2|6410|1720|
|secp192r1 opt=3|16232|950|
||||
|secp224r1 opt=1|6238|2800|
|secp224r1 opt=2|7408|2640|
|secp224r1 opt=3|17230|1460|
||||
|secp256r1 opt=1|6114|4640|
|secp256r1 opt=2|7160|4300|
|secp256r1 opt=3|17082|2570|
||||
|secp256k1 opt=1|5124|4640|
|secp256k1 opt=2|7338|3670|
|secp256k1 opt=3|17160|1940|

-----------------

micro-ecc
==========

A small and fast ECDH and ECDSA implementation for 8-bit, 32-bit, and 64-bit processors.

The static version of micro-ecc (ie, where the curve was selected at compile-time) can be found in the "static" branch.

Features
--------

 * Resistant to known side-channel attacks.
 * Written in C, with optional GCC inline assembly for AVR, ARM and Thumb platforms.
 * Supports 8, 32, and 64-bit architectures.
 * Small code size.
 * No dynamic memory allocation.
 * Support for 5 standard curves: secp160r1, secp192r1, secp224r1, secp256r1, and secp256k1.
 * BSD 2-clause license.

Usage Notes
-----------
### Point Representation ###
Compressed points are represented in the standard format as defined in http://www.secg.org/sec1-v2.pdf; uncompressed points are represented in standard format, but without the `0x04` prefix. All functions except `uECC_compress()` only accept uncompressed points; use `uECC_compress()` and `uECC_decompress()` to convert between compressed and uncompressed point representations.

Private keys are represented in the standard format.

### Using the Code ###

I recommend just copying (or symlink) the uECC files into your project. Then just `#include "uECC.h"` to use the micro-ecc functions.

For use with Arduino, you can use the Library Manager to download micro-ecc (**Sketch**=>**Include Library**=>**Manage Libraries**). You can then use uECC just like any other Arduino library (uECC should show up in the **Sketch**=>**Import Library** submenu).

See uECC.h for documentation for each function.

### Compilation Notes ###

 * Should compile with any C/C++ compiler that supports stdint.h (this includes Visual Studio 2013).
 * If you want to change the defaults for any of the uECC compile-time options (such as `uECC_OPTIMIZATION_LEVEL`), you must change them in your Makefile or similar so that uECC.c is compiled with the desired values (ie, compile uECC.c with `-DuECC_OPTIMIZATION_LEVEL=3` or whatever).
 * When compiling for a Thumb-1 platform, you must use the `-fomit-frame-pointer` GCC option (this is enabled by default when compiling with `-O1` or higher).
 * When compiling for an ARM/Thumb-2 platform with `uECC_OPTIMIZATION_LEVEL` >= 3, you must use the `-fomit-frame-pointer` GCC option (this is enabled by default when compiling with `-O1` or higher).
 * When compiling for AVR, you must have optimizations enabled (compile with `-O1` or higher).
 * When building for Windows, you will need to link in the `advapi32.lib` system library.
