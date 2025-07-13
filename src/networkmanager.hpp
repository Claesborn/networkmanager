#ifndef NETWORKMANAGER
#define NETWORKMANAGER


#include "server.hpp"
#include "client.hpp"
#include <memory>
#include <string>
#include <cstdint>
#include "nettypes.hpp"

class NetworkManager {
public:
    NetworkManager(uint16_t port, ProtocolType proto, bool runAsServer = true, bool runAsClient = false);
    NetworkManager(uint16_t serverPort, uint16_t clientPort, ProtocolType proto, bool runAsServer = true, bool runAsClient = false);
    ~NetworkManager();

    void start();
    void stop();

    void sendMessage(const std::string& message); // Only used if client is active

private:
    ProtocolType m_protocol;
    std::string m_host{"127.0.0.1"}; // Can be set later

    bool m_runAsServer;
    bool m_runAsClient;

    std::unique_ptr<Server> m_server;
    std::unique_ptr<Client> m_client;
};

#endif