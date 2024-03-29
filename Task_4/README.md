# Reactor-based Server

This is a simple server implemented using a reactor design pattern. The server listens for incoming connections and handles data exchange between clients.

## Getting Started

### Prerequisites

- C compiler (e.g., GCC)
- POSIX-compliant operating system (Linux, macOS)

### Building the Project

1. Clone the repository:
https://github.com/sagir567/operation_systems.git


2. Navigate to the project directory:
cd reactor-server

3. Build the server:


### Running the Server

After building the project, you can run the server by executing the `react_server` binary:
./react_server


The server will start listening on the specified port (9035 by default). You can connect to the server using a TCP client and send/receive data.

### Customization

If you want to change the listening port or adjust any other server parameters, you can modify the source code accordingly. Refer to the comments within the code for guidance.

### File Structure

- `main.c`: The entry point of the server application.
- `reactor.h`: Header file containing the definitions of the reactor and event-related structures, as well as function prototypes.
- `reactor.c`: Source file implementing the reactor and its associated functions.
- `chat.h`: Header file defining the functions for handling chat events.
- `chat.c`: Source file implementing the chat event handlers.

### Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - A comprehensive guide to network programming in C. It covers various topics, including socket programming and network communication.

## Contributing

Contributions to the project are welcome! If you have any ideas, improvements, or bug fixes, feel free to submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.


## Contact

For any inquiries or feedback, please contact [sagir567@gmail.com](mailto:your-email@example.com).
