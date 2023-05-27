# stnc- student network comunication

This is a sample project demonstrating client-server communication and performance testing.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [License](#license)

## Introduction

This project showcases different aspects of network communication, including client-server communication and performance testing. It consists of various functions and modules to perform different tasks related to networking.

## Features

- Client-Server Communication: The project includes functions to establish a client-server connection and allows chat-style communication between the client and server.

- Performance Testing: The project provides performance testing capabilities for different communication types, such as IPv4, IPv6, UDS, mmap, and pipe.

- MD5 Checksum Calculation: The project includes a function to calculate the MD5 checksum for a given data array.

- Error Handling: The code includes error handling mechanisms to handle network-related errors and display meaningful error messages.

## Requirements

- C Compiler
- OpenSSL library

## Installation

1. Clone the repository: `git clone https://github.com/sagir567/operation_systems.git`
2. Change to the project directory: `cd project-directory`
3. Compile the code: `gcc -o stnc stnc.c -lssl -lcrypt`
4. Run the executable: `./stnc`

## Usage

- Client-Server Communication:
  - Start the server: `./stnc -s PORT`
  - Connect a client: `./stnc -c IP PORT`

- Performance Testing:
  - Client-side:
    - `./stnc -c IP PORT -p <type> <param>`
    - Available communication types:
      - IPv4 tcp
      - IPv4 udp
      - IPv6 tcp
      - IPv6 udp
      - UDS dgram
      - UDS stream
      - mmap <file>
      - pipe <file>
  - Server-side:
    - `./stnc -s PORT -p -q`
    - The `-p` flag indicates performance testing, and the `-q` flag enables quiet mode.

## License

This project is licensed under the [MIT License](LICENSE).
