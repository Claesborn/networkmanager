#include "networkmanager.hpp"
#include <iostream>

NetworkManager::NetworkManager(uint16_t serverPort, uint16_t clientPort, ProtocolType proto, bool runAsServer, bool runAsClient)
    : m_protocol(proto), m_runAsServer(runAsServer), m_runAsClient(runAsClient)
{
    if (m_runAsServer) {
        m_server = std::make_unique<Server>(serverPort, m_protocol);
        LOG(logOFF) << "[SERVER] Starting on port " << serverPort;
    }

    if (m_runAsClient) {
        m_client = std::make_unique<Client>(m_host, clientPort, m_protocol);
        LOG(logOFF) << "[CLIENT] Starting, target port: " << clientPort;
    }
}

NetworkManager::NetworkManager(uint16_t port, ProtocolType proto, bool runAsServer, bool runAsClient)
    : m_protocol(proto), m_runAsServer(runAsServer), m_runAsClient(runAsClient)
{
    if (m_runAsServer) {
        m_server = std::make_unique<Server>(port, m_protocol);
        LOG(logOFF) << "[SERVER] Starting on port " << port;
    }

    if (m_runAsClient) {
        m_client = std::make_unique<Client>(m_host, port, m_protocol);
        LOG(logOFF) << "[Client] Starting, target port: " << port;
    }
}

NetworkManager::~NetworkManager() {
}

void NetworkManager::start() {
    if (m_runAsServer) {
        m_server->start();
    }

    // optional: lazy client connection or connect on demand
}

void NetworkManager::stop() {
    if (m_server) {
        m_server->stop();
    }
}

void NetworkManager::sendMessage(const std::string& message) {
    if (m_client) {
        m_client->sendMessage(message);
    }
    else {
        std::cerr << "Client not initialized.\n";
    }
}