# memscope - simulator of program memory allocation for educational purposes

## Overview

A small program, consisting of a few functions, is generated randomly. The program contains global and local variables, as well as heap-allocated data. It is shown as both pseudocode and bytecode. The simulated memory is divided into four parts: static data, stack, heap, and code. The memory is displayed up to the point where all functions in the call chain have their stack frames allocated.

The interface is textual. In normal mode, the program runs all at once, prints the program and memory layout, and finishes. There is also an interactive mode, which prompts the user to execute the program step by step.

s## Versions and incremental development

The simplest version has no interactive mode and no heap allocation. This keeps the codebase smaller and more approachable for students. This repository can also be used as a programming assignment.

The next version includes interactive mode.

The full-fledged version includes heap allocation.

Each version has a separate branch in the version control system (`git`): `no-prompt-no-heap`, `no-heap`, and `master`.

This is a C++ project, but there is also a branch implemented in plain C, called `sprout-in-c`.

## Usage

### sprout-in-c
```bash
$ ./memscope
```
Shows a randomly generated program (source code and bytecodes), then the memory allocation layout.
