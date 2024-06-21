# Low Level Assembler

![Assembler](https://img.shields.io/badge/Assembler-v1.0-blue.svg)

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

### Example

```bash
./assembler example.asm
```
This command will process **example.asm** and generate **example.am**, **example.ent**, **example.ext**, and **example.ob** files.

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
