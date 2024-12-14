# Low Level Assembler [![Assembler](https://img.shields.io/badge/Assembler-v1.0-blue.svg)](#) [![](https://komarev.com/ghpvc/?username=LowLevelAssembler&style=pixel&label=Repo%20Views&color=0e75b6)](#)

## Overview

The **Low Level Assembler** is a program designed to act as an assembler, converting assembly commands into their binary counterparts. </br>
The conversion process involves multiple stages to ensure accurate and reliable assembly-to-binary transformation. </br>
This assembler handles preprocessing, label checking, parsing, and file writing, producing multiple output files to give a detailed view of the assembly-to-binary conversion process.

## Features

- **Preprocessing**: Reads and expands macros.
- **Label Checking**: Validates labels for any discrepancies.
- **Parsing**: Analyzes and validates the syntax.
- **File Generation**: Produces a processed file (*.am), entry labels file (*.ent), extern labels file (*.ext), and a binary code file (*.ob).
- **Error Handling**: Strong focus on error handling to ensure accurate conversions.
- **Memory Management**: Ensures all allocated memory is appropriately freed after use.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Files](#files)
- [Contributing](#contributing)
- [License](#license)

## Installation

To use the assembler, clone the repository and compile the source code:

```bash
git clone https://github.com/danielbrodi/LowLevelAssembler.git
cd LowLevelAssembler
make
```

## Usage

To run the assembler, use the following command:

```bash
./assembler <file1> <file2> ... <fileN>
```
Each <file> should be an assembly file you wish to process. </br>
The program will generate corresponding output files with the following extensions:
- *.am* for the processed file.
- *.ent* for the entry labels file.
- *.ext* for the extern labels file.
- *.ob* for the binary code file.

#### Run the Command
```bash
./assembler tests/example.asm
```
This command will process **example.asm** located in the **tests** directory and generate **example.am**, **example.ent**, **example.ext**, and **example.ob** files.

## Flow Example

### Input File: `example.as`
The sample example file for running can be found in the [**tests**](./tests) directory, among other test files that you can run.

```assembly
.entry LENGTH
.extern W
MAIN: mov @r3, LENGTH
LOOP: jmp L1
prn -5
bne W
sub @r1, @r4
bne L3
L1: inc K
.entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6, -9, 15
K: .data 22
.extern L3
```

### Processing Steps

#### Step 1: Macro Expansion

```assembly
.entry LENGTH
.extern W
MAIN: mov @r3, LENGTH
LOOP: jmp L1
prn -5
bne W
sub @r1, @r4
bne L3
L1: inc K
.entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6, -9, 15
K: .data 22
.extern L3
```

#### Step 2: Label Checking

The assembler checks for any discrepancies or errors in the labels, such as undefined labels or duplicate definitions. In this example, no discrepancies were found.

#### Step 3: Parsing and Binary Code Generation

**Binary Machine Code**

| Address | Source Code               | Binary Code         |
|---------|---------------------------|---------------------|
| 0100    | MAIN: mov @r3, LENGTH     | 101000001100        |
|         |                           | 000110000000        |
|         |                           | 000111110110        |
| 0103    | LOOP: jmp L1              | 000100101100        |
|         |                           | 000111000110        |
| 0105    | prn -5                    | 000110000100        |
|         |                           | 111111101100        |
| 0107    | bne W                     | 000101001100        |
|         |                           | 000000000001        |
| 0109    | sub @r1, @r4              | 101001110100        |
|         |                           | 000010010000        |
| 0111    | bne L3                    | 000101001100        |
|         |                           | 000000000001        |
| 0113    | L1: inc K                 | 000011101100        |
|         |                           | 001000000010        |
| 0115    | jmp W                     | 000100101100        |
|         |                           | 000000000001        |
| 0117    | END: stop                 | 000111100000        |
| 0120    | STR: .string "abcdef"     | 000001100001        |
|         |                           | 000001100010        |
|         |                           | 000001100011        |
|         |                           | 000001100100        |
|         |                           | 000001100101        |
|         |                           | 000001100110        |
|         |                           | 000000000000        |
| 0125    | LENGTH: .data 6, -9, 15   | 000000000110        |
|         |                           | 111111110111        |
|         |                           | 000000001111        |
| 0128    | K: .data 22               | 000000010110        |


#### Step 4: Writing Output Files

**example.ob**

Binary machine code output file containing the compiled instructions in base64 format.

```
18 11
oM
GA
H2
Es
HG
GE
/s
FM
AB
p0
CQ
FM
AB
Ds
IC
Es
AB
Hg
Bh
Bi
```

**example.ent**

Entry labels output file listing symbols marked as entry points with their corresponding addresses.<br>
Listed in decimal (base 10) format.

```
LOOP 103
LENGTH 125
```

**example.ext**

External labels output file listing symbols referenced but not defined within the program.</br>
Listed in decimal (base 10) format.

```
W 108
L3 112
W 116
```

> Note: If there are no external labels, the `.ext` file will not be created. </br> Similarly, if there are no entry labels, the `.ent` file will not be created.

## Files

### Source Files

- **assembler.c**: Main file containing the program's entry point and core logic.
- **preprocessor.c**: Handles macro expansion.
- **label_checker.c**: Validates labels.
- **parser.c**: Parses and validates the syntax.
- **file_writer.c**: Writes the output files.

### Output Files

- **.am**: Processed file after macro expansion.
- **.ent**: File containing entry labels.
- **.ext**: File containing external labels.
- **.ob**: Binary code file, converted to base64.

## Contributing

Contributions are welcome!</br>
Please fork the repository and create a pull request with your changes. </br>

1. Fork the repository.
2. Create a new branch (`git checkout -b add-feature-branch`).
3. Commit your changes (`git commit -am 'Add new feature'`).
4. Push to the branch (`git push origin add-feature-branch`).
5. Create a new Pull Request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Additional Information

For more detailed information about the program's workflow, refer to the comments in the [assembler.c](assembler.c) file.

### Main Method Workflow

```c
/**
 * Serves as the program's entry point. The flow of the program includes:
 *    1. Initiation of the program state. The assumption of the program is that the starting memory block is
 *       located at address '100'.
 *    2. Reading files and appending appropriate extensions(.am, .ext, .ent, .ob).
 *    3. Preprocessing: reading and expanding macros.
 *    4. Checking labels for any discrepancies.
 *    5. Parsing files to understand and validate the syntax.
 *    6. Writing all labels, both entry & external to separated files(.ext, .ent).
 *    7. Conversion from binary to base64 and save in a file (.ob).
 *       Note: If any of the stages encounters an error or fails, the program will not produce a base64 *.ob file.
 *    8. Finally, freeing the program state after processing.
 */
```

## Contact

For any questions or feedback, feel free to open an issue or contact me:
- **Daniel Brodsky** - [Linkedin](https://www.linkedin.com/in/daniel-brod/)

### Special thanks to Lior Katav for writing this software with me.
