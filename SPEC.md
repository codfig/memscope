# memscope - simulator of program memory allocation for educational purpose

## Overview

A small program (with few functions) is randomly generated. The program have global and local variables and heap allocated data. The program is shown as pseudocode and bytecode. The program memory has 4 parts: static data, stack, heap and code. The memory is shown up to the moment where all functions have stack frames allocated.

The interface is textual. In normal mode, the program runs at once, prints program and memory layout and finishes. There is also an interactive mode, that prompts the user for a step by step execution.

## Versions and incremental development

The simplest version has no interactive mode and no heap allocation. This can make de code base smaller and more approachable for students. This repo can be used as a programming asignment also.

The next version includes interactive mode.

The full fledge version includes heap allocation.

Each version has a separated branch on Version Control System (git). They are: bare, noheap and master

This is a C++ project, but there is a branch where all is made in plain C, called bare-c

## Data Specification

### Memory
Memory is a collection of cells.
Each cell has an address and a content.
So, memory has a range of address, that are defined by an initial address and a size.
There is default values for initial address and size, for example, 4800 and 128.

Memory is divided in 4 segments: static data, stack, heap and code.
There is default values for the address range of each segment, for example (sizes): 8, 48, 8, 64

### Programs

Programs are composed by:
- global variable declarations
- function declarations (including a mandatory function called 'main')

Programs have:
- source code representation, close do C block syntax
- bytecode representation

Programs are randomly generated. There are many attributes of a program that can vary.
For example, the overall number of functions, the size, sequence and type of operations of each function, and so on.
Each generated program has an associated number. The number is defined based on the decisions occured during de program generation. 
This number, besides representing the program construction based on its value, serves as an id of the program.
A parameter of execution of this tool is to inform a previously generated number, so that the same program is simulated again.

#### Variables and parameters

All variables (global or local) and parameters are composed by one letter or one letter followed by a digit. 
Each variable or parameter also have an address. The address is calculated based on segment address and other variable and function allocations on stack.

Variables are declared:
- in the beginning of the program, so they are global variables
- in the beginning of the function declarations, so they are local

Parameters are declared in the function declarations.

#### Functions
Functions are composed by:
- name
- parameter set
- operation set
- byte code address on code segment

Functions can be recursive.

#### Operations
An operation can be:
- variable declaration
- variable assignment of a small positive integer
- variable assignment of a sum of variables
- function call
- variable assignment of returned value
- end of function without return values (return)
- end of function with return values (return ...)
- conditional end of function (comparison of a decremented variable with zero)

Each type of operation has a textual representation and (maybe) a bytecode representation (address has 2 bytes in bytecode):
- variable declaration, examples: 'var a', 'var b', 'var m2', no bytecode
- variable assignment of int, examples: 'a = 1', 'b = 2', bytecode (4 bytes): 01 <addressH> <addressL> <number>
- variable assignment of sums, examples: 'z = x + y', 
     bytecode (7 bytes): 02 <destH> <destL> <or1H> <or1L> <or2H> <or2L>
- function call, examples: 'f()', 'g(a,k)', 'h(x)',
     bytecode (3 bytes): 03 <funcH> <funcL>
- variable assignment with ret: example: 'p = f(a)',
     bytecode (5 bytes): 04 <varH> <varL> <funcH> <funcL>
- return without: 'return' , bytecode(1 byte): 05
- return with value: example 'return i', bytecode (3 byte): 06 <varH> <varL>
- conditional return: example 'if (z-- = 0), return', bytecode (3 byte): 07 <varH> <varL>

About addresses:
- All values are decimal
- Address range is 0 to 9999
- Range of a low part of an address is 0 to 99
- Range of a high part of an address is 0 to 99; example: 42 corresponds to 4200-4299 addresses

#### Random generation

The first thing to generate is the maximum chain of called functions of a possible execution.
There are a minimum and a maximum number of function in this chain. (For example: 2 and 5)

Then, it is time do random generate some functions (names, parameters and local variables), and its point of invocation in other functions.
- the last function is always 'm4' (meaning MAin)
- the first function is either a function that has no function call or is recursive with a parameter initial value = 1 that is tested by the return conditional operation.

Then, the global variables are randomly decided.
There is a default minimum and maximum number of global variables, such as 1 and 3.

Finally, all the functions are completed with:
- parameters (min number default, that may be 0, max number default, that may be 3)
- local variables (min number default, that may be 1, max number default, that may be 4)
- operations such:
    - variable assignments, by numbers, sums or function returns
    - function calls (as defined by the call chain)
    - proper ending

'm4' (main) has no parameters, and 1 local variable and 1 function call.

The quantity of function is also randomly generated. 

Recursive calls must occur exactly twice. This is done by calling with a variable whose value is 1, and ending with a conditional return on this variable.

The program is internally represented by the sequence of global variable declarations followed by the sequence of function declarations. ('m4' is always the last one.)

There must be name sets previously defined. To define names of vars and parameters of a function, name sets must be choosed. 
For example (in general, these are for parameter sets or variable sets, global or local):
- a, b, c, d, x, y, z
- x1, x2, y1, y2, z1, z2
- m, n, p, q, p1, p2
- i, j, k, l, j1, j2
- p, q, r, s, r1, r2
- f, g, h, f1, f2, f2 (for functions)
- g, f, g1, g2, g3 (for functions)

#### Program ID
The Program ID is defined by a combination of small pieces (little numbers) defined in the generation process.
The ID reflects:
- the call chain
- the global variables
- the function definitions (each)

#### Showing a program
The program is showed in ASCII format:
- First column is the number line of the source code representation
- Operation in source code
- Bytecodes
- an arrow on the first instruction of the function on the top of the stack

The Program ID comes below.

### Program execution

#### Stack frames
Each function call is represented by a Stack Frame on stack:
- at lower addresses there are parameters
- then the return address (from code segment with bytecodes)
- then the local variables.

## Memory simulation

### Program loading on memory
The static data segment is filled with global variables.
The code segment is filled with bytecodes, return addresses of calls are filled.
The stack is filled with the stack frames of chain call.

### Memory display
The memory is displayed in a number of columns. There is a default values, e.g. 4.
The numbers grow up to down. The first column goes from initial address to (initial address + size/4-1),
and so on.
Each cell is shown as an address and a box with content. The initial address of each segment
has a string after the box, with the name of the segment (STATIC, STACK, HEAP, CODE)

## Architectural
The code must be separated in files:
- Program representation and display
- Memory representation and display
- Program generation
- Main

