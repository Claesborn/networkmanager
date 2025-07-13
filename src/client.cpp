#include "client.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

Client::Client(const std::string& host, uint16_t port, ProtocolType proto)
    : m_host(host.empty() ? "127.0.0.1" : host), m_port(port), m_protocol(proto)
{
    m_initialized = init();
    if (m_initialized) {
        
        // Bind correct method
        if (proto == ProtocolType::TCP)
        m_sendFunc = [this](const std::string& msg) { tcpConnectAndSend(msg); };
        else
        m_sendFunc = [this](const std::string& msg) { udpSendTo(msg); };
        
        LOG(logINFO) << "Client initialized, ready to send messages to "
             << m_host << ":" << m_port << " using "
             << (m_protocol == ProtocolType::TCP ? "TCP" : "UDP");
    }
}

Client::~Client() {
    if (m_socketFd >= 0) {
        close(m_socketFd);
    }
}

void Client::sendMessage(const std::string& message) {
    m_sendFunc(message);
}

bool Client::init()
{
    if (m_port == 0) m_port = 2000;

    LOG(logINFO) << "Setting up "
                 << (m_protocol == ProtocolType::TCP ? "TCP" : "UDP")
                 << " client to " << m_host << ":" << m_port;

    int type = (m_protocol == ProtocolType::TCP) ? SOCK_STREAM : SOCK_DGRAM;
    m_socketFd = socket(AF_INET, type, 0);
    if (m_socketFd < 0) {
        LOG(logERROR) << "Failed to create client socket: " << strerror(errno);
        return false;
    }

    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(m_port);
    if (inet_pton(AF_INET, m_host.c_str(), &m_serverAddr.sin_addr) <= 0) {
        LOG(logERROR) << "Invalid address: " << m_host;
        return false;
    }

    // Optional: Connect here or defer to send()
    return true;
}

void Client::tcpConnectAndSend(const std::string& message) {
    // Always recreate socket for each message
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        LOG(logWARNING) << "Failed to create socket\n";
        return;
    }

    if (connect(m_socketFd, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
        LOG(logWARNING) << "TCP connect failed: " << strerror(errno) << "\n";
        close(m_socketFd);
        m_socketFd = -1;
        return;
    }

    ssize_t sent = send(m_socketFd, message.c_str(), message.size(), 0);
    if (sent < 0) {
        LOG(logWARNING) << "TCP send failed: " << strerror(errno) << "\n";
    }

    char buffer[1024] = {};
    ssize_t n = recv(m_socketFd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "TCP Server response: " << buffer << "\n";
    }

    close(m_socketFd);
    m_socketFd = -1;
}

void Client::udpSendTo(const std::string& message) {
    sendto(m_socketFd, message.c_str(), message.size(), 0,
           (sockaddr*)&m_serverAddr, sizeof(m_serverAddr));
}
