# TCP Stream Socket Client-Server

## Overview
This repository contains a reliable STREAM socket client and server implementation using TCP in C. It demonstrates fundamental networking concepts, including socket creation, binding, listening, and establishing connections over IPv4.

## Features
* **TCP Communication:** Establishes a reliable, ordered, and error-checked delivery of a stream of bytes between a client and a server.
* **Interactive Client:** Prompts the user to input a custom string message via standard input to send across the network.
* **Server Acknowledgment:** The server receives the client's message, logs the payload size, and automatically replies with a fixed acknowledgment string.
* **Dynamic Configuration:** Both the server and client accept target IP addresses and port numbers as command-line arguments, supporting ports between 1 and 65535.
* **Port Reusability:** The server utilizes `SO_REUSEADDR` to prevent "Address already in use" errors during rapid restarts.
* **Resource Management:** Includes dedicated cleanup routines to ensure all socket file descriptors are properly closed upon completion.

## Prerequisites
* A Unix/Linux environment.
* [cite_start]GCC (GNU Compiler Collection)[cite: 1].
* [cite_start]Make build automation tool[cite: 1].

## Building the Project
[cite_start]A `Makefile` is provided to streamline the compilation process[cite: 1]. [cite_start]It uses the `-Wall` and `-Wextra` flags to enforce strict compiler warnings and ensure code quality[cite: 1].

To compile both the server and the client executables, run:
```bash
make
