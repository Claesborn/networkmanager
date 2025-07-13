#ifndef SERVER_HPP
#define SERVER_HPP

#include <thread>
#include <atomic>
#include <netinet/in.h>
#include <functional>
#include "nettypes.hpp"
#include "util/log.hxx"

class Server {
public:
    Server(uint16_t port, ProtocolType proto);
    ~Server();

    bool start();
    void stop();

private:
    void tcpListenLoop();
    void udpReceiveLoop();
    void handleClient(int clientFd);

    ProtocolType m_protocol;
    uint16_t m_port;
    int m_socketFd{-1};
    std::atomic<bool> m_running{false};
    std::thread m_thread;

    sockaddr_in m_clientAddr{};
    socklen_t m_clientLen{};

    std::function<void()> m_runLoop;  // Bound to tcpListenLoop or udpReceiveLoop
};

#endif