#ifndef APPLICATION
#define APPLICATION
#include <csignal>
#include <map>
#include "networkmanager.hpp"

extern volatile std::sig_atomic_t quit_flag;

class Application {

public:
    Application(std::map<std::string, std::string> argMap);
    ~Application();

    void run();
    void init();
    void update();
    void render();
    
    private:
    void parseArgMap(std::map<std::string, std::string>& map);
    void commandLineHelp();
    
    // Network settings
    uint16_t m_serverPort;
    uint16_t m_clientPort;
    bool isServer;
    bool isClient;
    std::unique_ptr<NetworkManager> nwm;
    ProtocolType protocol;

    // System settings
    bool m_running;
    int m_logLevel;
    std::thread m_inputThread;
};


#endif