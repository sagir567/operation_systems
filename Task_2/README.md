Simple Shell (stshell)

A simple shell implementation in C that allows running basic commands, handling input/output redirection, and piping.
How to Compile and Run

    Compile the code with the following command:

gcc -o stshell stshell.c

    Run the shell using:

bash

./stshell

Features

    Execute basic commands
    Input/output redirection with <, > and >>
    Piping with |
    Change directory with cd
    Exit the shell with exit

Codec Libraries (codecA and codecB)

Two codec libraries for encoding and decoding text.
How to Compile

    Compile codecA with the following command:

vbnet

gcc -shared -o codecA.so codecA.c -fPIC

    Compile codecB with the following command:

vbnet

gcc -shared -o codecB.so codecB.c -fPIC

Encoding and Decoding (encode.c and decode.c)

Programs to encode and decode text using codecA and codecB libraries.
How to Compile

    Compile the encoding program with the following command:

gcc -o encode encode.c -ldl

    Compile the decoding program with the following command:

gcc -o decode decode.c -ldl

How to Run

    Run the encoding program:

php

./encode <codec> <message>

    Run the decoding program:


php

./decode <codec> <message>

File Copy (copy.c)

A program to copy the contents of one file to another.
How to Compile and Run

    Compile the code with the following command:

go

gcc -o copy copy.c

    Run the program using:

css

./copy <file1> <file2> [-v] [-f]

Flags:

    -v: Display the result in a literal format.
    -f: Overwrite the destination file if it exists.

File Comparison (cmp.c)

A program to compare the contents of two files.
How to Compile and Run

    Compile the code with the following command:

gcc -o cmp cmp.c

    Run the program using:

css

./cmp <file1> <file2> [-v] [-i]

Flags:

    -v: Display the result in a literal format.
    -i: Enable case sensitivity during comparison.