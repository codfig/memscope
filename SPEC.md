# memscope - simulator of program memory allocation for educational purposes

## Overview

A small program, consisting of a few functions, is generated randomly. The program contains global and local variables, as well as heap-allocated data. It is shown as both pseudocode and bytecode. The simulated memory is divided into four parts: static data, stack, heap, and code. The memory is displayed up to the point where all functions in the call chain have their stack frames allocated.

The interface is textual. In normal mode, the program runs all at once, prints the program and memory layout, and finishes. There is also an interactive mode, which prompts the user to execute the program step by step.

## Versions and incremental development

The simplest version has no interactive mode and no heap allocation. This keeps the codebase smaller and more approachable for students. This repository can also be used as a programming assignment.

The next version includes interactive mode.

The full-fledged version includes heap allocation.

Each version has a separate branch in the version control system (`git`): `no-prompt-no-heap`, `no-heap`, and `master`.

This is a C++ project, but there is also a branch implemented in plain C, called `sprout-in-c`.

## Data Specification

### Memory

Memory is a collection of cells.
Each cell has an address and content.
Therefore, memory has an address range defined by an initial address and a size.
There are default values for the initial address and size, for example, `4800` and `128`.

Memory is divided into four segments: static data, stack, heap, and code.
There are default values for the size of each segment, for example: `8`, `48`, `8`, and `64`.

### Programs

Programs are composed of:
- global variable declarations
- function declarations, including a mandatory function called `main`

Programs have:
- a source code representation, close to C block syntax
- a bytecode representation

Programs are generated randomly. Many attributes of a program can vary.
For example, the total number of functions, the size, sequence, and types of operations in each function, and so on.
Each generated program has an associated number. This number is defined from the decisions made during program generation.
Besides representing how the program was constructed, this number also serves as the program ID.
One execution parameter of this tool allows the user to provide a previously generated number so the same program can be simulated again.

#### Variables and parameters

All variables, whether global or local, and all parameters are named with either one letter or one letter followed by a digit.
Each variable or parameter also has an address. The address is calculated based on the segment address and the allocation of other variables and function frames on the stack.

Variables are declared:
- at the beginning of the program, in which case they are global variables
- at the beginning of function declarations, in which case they are local variables

Parameters are declared in function declarations.

#### Functions

Functions are composed of:
- a name
- a parameter set
- an operation set
- a bytecode address in the code segment

Functions can be recursive.

#### Operations

An operation can be:
- variable declaration
- assignment of a small positive integer to a variable
- assignment of the sum of variables to a variable
- function call
- assignment of a return value to a variable
- end of function without a return value (`return`)
- end of function with a return value (`return ...`)
- conditional end of function, based on the comparison of a decremented variable with zero

Each type of operation has a textual representation and, when applicable, a bytecode representation. Addresses use 2 bytes in bytecode:
- variable declaration, examples: `var a`, `var b`, `var m2`; no bytecode
- assignment of an integer, examples: `a = 1`, `b = 2`; bytecode (4 bytes): `01 <addressH> <addressL> <number>`
- assignment of a sum, examples: `z = x + y`;
  bytecode (7 bytes): `02 <destH> <destL> <or1H> <or1L> <or2H> <or2L>`
- function call, examples: `f()`, `g(a,k)`, `h(x)`;
  bytecode (3 bytes): `03 <funcH> <funcL>`
- assignment from return value, example: `p = f(a)`;
  bytecode (5 bytes): `04 <varH> <varL> <funcH> <funcL>`
- return without value: `return`; bytecode (1 byte): `05`
- return with value, example: `return i`; bytecode (3 bytes): `06 <varH> <varL>`
- conditional return, example: `if (z-- = 0), return`; bytecode (3 bytes): `07 <varH> <varL>`

About addresses:
- all values are decimal
- the address range is `0` to `9999`
- the range of the low part of an address is `0` to `99`
- the range of the high part of an address is `0` to `99`; for example, `42` corresponds to addresses `4200` to `4299`

#### Random generation

The first element to generate is the maximum chain of function calls in a possible execution.
There is a minimum and maximum number of functions in this chain, for example: `2` and `5`.

Then some functions are generated randomly, including their names, parameters, local variables, and their points of invocation in other functions.
- the last function is always `m4` (meaning `main`)
- the first function is either a function that has no function call or a recursive function with an initial parameter value of `1` that is tested by the conditional return operation

Then the global variables are chosen randomly.
There is a default minimum and maximum number of global variables, such as `1` and `3`.

Finally, all functions are completed with:
- parameters (default minimum may be `0`, default maximum may be `3`)
- local variables (default minimum may be `1`, default maximum may be `4`)
- operations such as:
  - variable assignments from numbers, sums, or function returns
  - function calls, as defined by the call chain
  - proper termination

`m4` (`main`) has no parameters, one local variable, and one function call.

The number of functions is also generated randomly.

Recursive calls must occur exactly twice. This is done by calling a function with a variable whose value is `1`, and ending with a conditional return based on this variable.

The program is internally represented as the sequence of global variable declarations followed by the sequence of function declarations. (`m4` is always the last one.)

There must be predefined name sets. To define the names of variables and parameters in a function, these name sets must be chosen.
For example, in general these sets are used for parameter sets or variable sets, whether global or local:
- `a, b, c, d, x, y, z`
- `x1, x2, y1, y2, z1, z2`
- `m, n, p, q, p1, p2`
- `i, j, k, l, j1, j2`
- `p, q, r, s, r1, r2`
- `f, g, h, f1, f2, f2` (for functions)
- `g, f, g1, g2, g3` (for functions)

#### Program ID

The Program ID is defined by a combination of small pieces, or small numbers, produced during the generation process.
The ID reflects:
- the call chain
- the global variables
- the function definitions

#### Showing a program

The program is shown in ASCII format:
- the first column is the line number of the source code representation
- the second column contains the source code operation
- the third column contains the bytecodes
- an arrow marks the first instruction of the function at the top of the stack

The Program ID appears below.

### Program execution

#### Stack frames

Each function call is represented by a stack frame on the stack:
- parameters are at lower addresses
- then comes the return address, from the code segment with bytecodes
- then come the local variables

## Memory simulation

### Program loading in memory

The static data segment is filled with global variables.
The code segment is filled with bytecodes, and the return addresses of calls are stored there.
The stack is filled with the stack frames of the call chain.

### Memory display

Memory is displayed in a number of columns. There is a default value, for example `4`.
The numbers increase from top to bottom. The first column goes from the initial address to `(initial address + size / 4 - 1)`,
and so on.
Each cell is shown as an address and a box with content. The initial address of each segment
has a string after the box indicating the segment name: `STATIC`, `STACK`, `HEAP`, `CODE`.

## Architectural

The code must be separated into files:
- program representation and display
- memory representation and display
- program generation
- main
