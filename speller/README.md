# Spell Checker

A  fast spell checking application written in C. This program checks a text file against a dictionary, finds misspelled words, suggests corrections, and shows detailed performance results.

## Features

- **Spell Checking**: Checks words in a text file against a large dictionary.
- **Auto Correction**: Finds misspelled words and replaces them with the best matching correction.
- **Performance Reports**: Shows the time taken for loading, checking, sizing, and unloading.
- **Custom Dictionaries**: Works with the default dictionary or one you choose.
- **Robust Handling**: Handles punctuation, spacing, and different file types well.

## Getting Started

### Prerequisites

- A C compiler (clang is recommended).
- `make` tool.

### Compilation

To compile the program, run this command in the project folder:

```bash
make speller
```

This will create a program named `speller`.

To clean up old files, run:

```bash
make clear
```

## Usage

You can run the program with the default dictionary or a custom one.

### Basic Usage

To check a text file using the default dictionary:

```bash
./speller <text_file>
```

**Example:**
```bash
./speller texts/alice.txt
```

### Using a Custom Dictionary

To use a different dictionary:

```bash
./speller <dictionary_file> <text_file>
```

**Example:**
```bash
./speller dictionaries/small texts/alice.txt
```

## Functionality Overview

1.  **Loading**: The program loads the dictionary into memory.
2.  **Checking**: It goes through the text file and checks words against the dictionary.
3.  **Correction**: If a word is wrong, it finds the best match and updates the text file.
4.  **Reporting**: Finally, it prints the number of misspelled words, dictionary size, total words, and time taken.

## Project Structure

- `speller.c`: Main program code.
- `dictionary.c` & `dictionary.h`: Dictionary code and settings.
- `helper.c`: Helper functions for fixing words.
- `Makefile`: Script to build the project.
- `dictionaries/`: Folder with dictionary files.
- `texts/`: Folder with text files for testing.


