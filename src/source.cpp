#include "application.hpp"
#include <iostream>
volatile std::sig_atomic_t quit_flag = false;

void handleSigint(int);

int main(int argc, char* argv[])
{
    LOG(logOFF) << "Starting Network Manager";
    LOG(logOFF) << "Created 20250713 by Tommy Claesborn";
    LOG(logOFF) << "Version: 0.0.1";

    signal(SIGINT, handleSigint);

    std::map<std::string, std::string> argMap;
    std::string logArgs;
    for (int i = 1; i + 1 < argc; i += 2) {
        argMap[argv[i]] = argv[i + 1];
        logArgs += argv[i];
        logArgs += ", ";
        logArgs += argv[i+1];
        logArgs += ", "; 
    }
    LOG(logOFF) << "Command line arguments used: " << logArgs;

    Application app(argMap);
    app.run();

    return 0;
}

void handleSigint(int)
{
    quit_flag = true;
    std::cout << std::endl << "Application terminated, shutting down.." << std::endl;
}