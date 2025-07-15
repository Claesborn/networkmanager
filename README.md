

## NetworkManager

A minimal, configurable client-server communication tool written in C++. Supports both TCP and UDP protocols. Built for local or network-based messaging, with customizable ports, logging, and runtime control. Building it for fun in order to learn and perhaps use at home.
- Note: This is a work in progress!

---

## TODO

- Message framing protocol (multi-part support)
- IPv6 support
- Unit testing
- Config file (JSON/toml/yaml) support
- Interactive terminal interface
- epoll
- Threadpool

## Features

- TCP & UDP communication
- Bidirectional client/server architecture
- Configurable via command-line
- Color-coded log system with hourly rotating log files
- Auto-deletes logs older than 14 days
- Threaded input handler (non-blocking)
- Built-in CLI help (`-h`)

---

## Build Instructions

### Prerequisites

- C++17 compiler (e.g., g++ 10+ or clang 10+)
- xmake
- POSIX-compatible system (Linux recommended)

### Build

```bash
git clone https://github.com/yourusername/networkmanager.git
cd networkmanager
xmake
cd build
```

### 📃 License

This project is licensed under the [MIT License](LICENSE)
