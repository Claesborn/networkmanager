#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <cstdint>
#include <functional>
#include <netinet/in.h>
#include "nettypes.hpp"
#include "util/log.hxx"

class Client {
public:
    Client(const std::string& host, uint16_t port, ProtocolType proto);
    ~Client();

    void sendMessage(const std::string& message);
    bool isInitialized() { return m_initialized; }
private:
    bool init();
    void tcpConnectAndSend(const std::string& message);
    void udpSendTo(const std::string& message);

    std::string m_host;
    uint16_t m_port;
    ProtocolType m_protocol;
    int m_socketFd{-1};

    bool m_initialized;
    sockaddr_in m_serverAddr{};
    std::function<void(const std::string&)> m_sendFunc;
};

#endif