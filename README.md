## Compiler Construction

The repository contains the compiler implementation for a custom language as a part of course work for CS F363 - Compiler Construction at BITS Pilani under instructor [**Vandana Agarwal**](https://universe.bits-pilani.ac.in/pilani/vandana/profile).

The language supports features such as 
- Static scoping of variables
- Assignment, I/O, Declarative, Iterative, Conditional, Function call statements.
- Returning multiple values from a function
- Arithmetic and boolean expressions
- Strongly typed language

The language specifications can be found [here](./Language-specifications.pdf)

### Note

**All files on this repository are for educational purpose with no intentions of promoting unfair means in any evaluative component.**

## How to run

In the Code directory, run the following commands

```
    make
    ./a.out testcase_file code_output_file
    nasm -felf64 code.asm && gcc -no-pie code.o -o code && ./code
```

where testcase_file is input test case file (Some semantically correct testcases can be found under Code/code_gen_tcs and Code/Sample_Programs), and code_output_file is the output file in which the asm code will be generated (like code.asm)

The asm code is compatible with NASM, version 2.14.02

## Dependencies 

1. You should have NASM, version 2.14.02 installed on your machine. On ubuntu you can do so by running following command

```
    sudo apt install nasm
```

2. You should have gcc, version 5.0 or above installed on your machine.

## Sample run for testcase Code/Sample_Programs/Binary_search_dyn_arr.txt

<img src="binary_search_run.gif">

## Team Members

1. Vishal Mittal [Profile](https://github.com/vismit2000)
2. Harpinder Jot Singh [Profile](https://github.com/SinghCoder)
3. Yash Vijay [Profile](https://github.com/yashvijay018)
4. Jaladi Lakshmi Teja [Profile](https://github.com/LakshmiTeja17)
5. Aditya Upadhyay [Profile](https://github.com/adhyay2000)
