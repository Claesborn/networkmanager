#include "application.hpp"
#include <iostream>
#include "util/scopedtimer.hxx"

Application::Application(std::map<std::string, std::string> argMap)
    : m_running(true)
    
{
    parseArgMap(argMap);
    if (isServer && isClient) {
        nwm = std::make_unique<NetworkManager>(m_serverPort, m_clientPort, protocol, true, true);
        LOG(logOFF) << "Starting server and client";
    } else if (isServer) {
        nwm = std::make_unique<NetworkManager>(m_serverPort, protocol, true, false);
        LOG(logOFF) << "Starting server mode only";
    } else if (isClient) {
        nwm = std::make_unique<NetworkManager>(m_clientPort, protocol, false, true);
        LOG(logOFF) << "Starting client mode only";
    }
}

Application::~Application() {
    if(nwm) {
        LOG(logOFF) << "Closing Network Manager";
        nwm->stop();
    }
}

void Application::init()
{
    LOG(logOFF) << "Starting Network Manager";
    nwm->start();
    m_inputThread = std::thread([this]() {
        std::string msg;
        while (m_running) {
            std::cout << ">";
            if (!std::getline(std::cin, msg)) break;

            if(msg.empty()) continue;
            if (msg == "-quit" || msg == "-q") {
                LOG(logOFF) << "Quit command received, shutting down";
                m_running = false;
                break;
            }
            if (msg == "-help" || msg == "-h")
            {
                commandLineHelp();
                break;
            }

            nwm->sendMessage(msg);
        }
    });
}

void Application::run()
{
    init();
    
    while (!quit_flag) {
        update();
        render();
    }
}

void Application::update()
{
    // ScopedTimer timer("Update Timer");
    // std::cout << "Update" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // nwm->sendMessage("Ping");
}

void Application::render()
{

}

void Application::parseArgMap(std::map<std::string, std::string>& map) 
{
    LOG(logTRACE) << "Parsing command line arguments...";
    for (auto [key, value] : map) {
        if(key == "-ip") {

        } else if (key == "-sp" || key == "-serverport") {
            try {
            m_serverPort = std::stoi(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << std::endl;
            }
        } else if (key == "-cp" || key == "-clientport") {
            try {
            m_clientPort = std::stoi(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                std::cerr << "Out of range: " << e.what() << std::endl;
            }
        } else if (key == "-h") {
            commandLineHelp();
            std::exit(0);
        } else if (key == "-c") {
            isClient = value != "0";
        } else if (key == "-s") {
            isServer = value != "0";
        } else if (key == "-t") {
            if (value == "UPD") {
                protocol = ProtocolType::UDP;
            } else if (value == "TCP") {
                protocol = ProtocolType::TCP;
            } else {
                LOG(logOFF) << "Unknown protocol argument, defaulting to TCP";
            }
        } else if (key == "-ll" || key == "-loglevel") {
            m_logLevel = std::stoi(value);
            Logger::setLogLevel(LogLevel(m_logLevel));
            LOG(logOFF) << "Loglevel set to " << static_cast<int>(m_logLevel);
        }
    }
}

void Application::commandLineHelp()
{
    std::cout << "Usage: networkmanager [options]\n\n";

    std::cout << "Options:\n";
    std::cout << "  -sp <port>     Set port number for server to listen to.\n";
    std::cout << "  -cp <port>     Set port number for client to send to.\n";
    std::cout << "  -ip <address>  Set IP address to connect to.\n";
    std::cout << "  -c <0|1>       Run client: 1 = yes, 0 = no.\n";
    std::cout << "  -s <0|1>       Run server: 1 = yes, 0 = no.\n";
    std::cout << "  -t <TCP|UDP>   Set protocol type (TCP or UDP).\n";
    std::cout << "  -ll <0-6>      Set log level (0=OFF, 6=TRACE).\n";
    std::cout << "  -h             Show this help message and exit.\n";

    std::cout << std::endl;
}