#include "server.hpp"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

Server::Server(uint16_t port, ProtocolType proto)
    : m_protocol(proto), m_port(port)
{
    if(m_port == 0) {
        m_port = 2000;
    }
    // Type of connection
    int sock_type = (proto == ProtocolType::TCP) ? SOCK_STREAM : SOCK_DGRAM;
    // Create socket
    m_socketFd = socket(AF_INET, sock_type, 0);
    if (m_socketFd < 0) {
        throw std::runtime_error("Socket creation failed");
    }

    // Socket options
    int opt = 1;
    setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set socket properties
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    // Bind socket
    if (bind(m_socketFd, (sockaddr*)&address, sizeof(address)) < 0)
        throw std::runtime_error("Bind failed");

    // Bind the function pointer based on protocol
    if (proto == ProtocolType::TCP)
        m_runLoop = [this]() { tcpListenLoop(); };
    else
        m_runLoop = [this]() { udpReceiveLoop(); };
}

Server::~Server() {
    stop();
    if (m_socketFd >= 0) close(m_socketFd);
}

bool Server::start() {
    m_running = true;
    m_thread = std::thread([this]() {
        m_runLoop();  // Call bound loop function directly
    });
    return true;
}

void Server::stop() {
    m_running.store(false, std::memory_order_relaxed);
    // Skips the accept block
    shutdown(m_socketFd, SHUT_RDWR);

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Server::tcpListenLoop() {
    // Set max connections
    if (listen(m_socketFd, 3) < 0) {
        std::cerr << "Listen failed\n";
        return;
    }

    LOG(logINFO) << "TCP Server listening on port " << m_port;
    // Start listening for connections
    while (m_running) {
        m_clientLen = sizeof(m_clientAddr);
        int clientFd = accept(m_socketFd, (sockaddr*)&m_clientAddr, &m_clientLen);
        if (clientFd < 0) { // If no socket, go back to listen
            continue;
        } 

        LOG(logINFO) << "Accepted connection from " << inet_ntoa(m_clientAddr.sin_addr);

        char buffer[1024] = {}; // Buffer to read data (fixed)
        ssize_t n = recv(clientFd, buffer, sizeof(buffer) - 1, 0); // Read from socket
        if (n > 0) {
            buffer[n] = '\0'; // Append escape char to stop reading when printing
            std::cout << "Received from client: " << buffer << std::endl;
            LOG(logDEBUG) << "Message received from: " << clientFd << " - " << buffer;
            const char* msg = "Message received\n";
            send(clientFd, msg, strlen(msg), 0);
        }

        close(clientFd);
    }
}

void Server::udpReceiveLoop() {
    LOG(logOFF) << "UDP Server listening on port " << m_port << "\n";

    char buffer[1024];
    while (m_running) {
        socklen_t len = sizeof(m_clientAddr);
        ssize_t n = recvfrom(m_socketFd, buffer, sizeof(buffer) - 1, 0,
                             (sockaddr*)&m_clientAddr, &len);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "Received UDP from "
                      << inet_ntoa(m_clientAddr.sin_addr) << ": " << buffer << "\n";
        }
    }
}