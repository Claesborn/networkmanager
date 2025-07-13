#ifndef _LOG_HXX
#define _LOG_HXX

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <mutex>
#include <filesystem>

// Safe LOG macro
#define LOG(level) \
    if ((level > Logger::getLogLevel())) ; \
    else LogStream(level, __FILE__, __LINE__)


enum LogLevel {
    logOFF = 0,
    logFATAL,
    logERROR,
    logWARNING,
    logINFO,
    logDEBUG,
    logTRACE
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    static void setLogLevel(LogLevel level) {
        getInstance().m_level = level;
    }

    static LogLevel getLogLevel() {
        return getInstance().m_level;
    }

    void log(LogLevel level, const std::string& msg) {
        if (level > m_level) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        rotateLogFileIfNeeded();

        const char* color = getColor(level);

        std::ostringstream prefixStream;

        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        prefixStream << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
                     << "[" << levelToString(level) << "] ";

        std::string prefix = prefixStream.str();

        // Write to file (no color)
        if (m_logFile.is_open()) {
            m_logFile << color << prefix << msg << std::endl;
        }
    }

private:
    Logger() {
        m_lastHour = getCurrentHourKey();
        openLogFile();
    }

    ~Logger() {
        if (m_logFile.is_open())
            m_logFile.close();
    }

    std::mutex m_mutex;
    std::ofstream m_logFile;
    std::string m_lastHour;
    LogLevel m_level = logINFO;

    std::string getCurrentHourKey() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d%H");
        return oss.str();  // e.g., 20250713_14
    }

    void rotateLogFileIfNeeded() {
        std::string currentHour = getCurrentHourKey();
        if (currentHour != m_lastHour) {
            m_lastHour = currentHour;
            openLogFile();  // will close and reopen with new name
        }
    }

    void openLogFile() {
        if (m_logFile.is_open())
            m_logFile.close();

        std::string directory = "/var/log/";
        std::string filename = "networkmanager_" + m_lastHour + ".log";
        std::string path = directory + filename;

        try {
            std::filesystem::create_directories(directory);

            m_logFile.open(path, std::ios::app);
            if (!m_logFile) {
                std::cerr << "[Logger] Failed to open log file: " << path << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[Logger] Exception while opening log file: " << e.what() << "\n";
        }
        cleanOldLogs(directory);
    }

    void cleanOldLogs(const std::string& directory, int daysToKeep = 14) {
        const auto now = std::chrono::system_clock::now();

        try {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (!entry.is_regular_file())
                    continue;

                const auto& path = entry.path();

                // Only match files with name like: networkmanager_YYYYMMDD_HH.log
                if (path.filename().string().find("networkmanager_") != 0)
                    continue;

                auto ftime = last_write_time(path);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
                );
                auto age = std::chrono::duration_cast<std::chrono::hours>(now - sctp);


                if (age > std::chrono::hours(24 * daysToKeep)) {
                    std::cerr << "[Logger] Removing old log: " << path << std::endl;
                    std::error_code ec;
                    remove(path, ec);
                    if (ec) {
                        std::cerr << "[Logger] Failed to remove log " << path << ": " << ec.message() << std::endl;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[Logger] Exception while cleaning old logs: " << e.what() << std::endl;
        }
    }


    const char* levelToString(LogLevel level) {
        switch (level) {
            case logOFF:   return "INFO";
            case logFATAL: return "FATAL";
            case logERROR: return "ERROR";
            case logWARNING:  return "WARN";
            case logINFO:  return "INFO";
            case logDEBUG: return "DEBUG";
            case logTRACE: return "TRACE";
            default:        return "UNKNOWN";
        }
    }

    const char* getColor(LogLevel level) {
        switch (level) {
            case logOFF:   return "\033[37m"; // White
            case logFATAL: return "\033[31m"; // Red
            case logERROR: return "\033[91m"; // Bright Red
            case logWARNING:  return "\033[33m"; // Yellow
            case logINFO:  return "\033[37m"; // White
            case logDEBUG: return "\033[35m"; // Purple
            case logTRACE: return "\033[32m"; // Green
            default:        return "\033[0m";
        }
    }
};

class LogStream {
public:
    LogStream(LogLevel level, const char* file, int line)
        : m_level(level), m_file(file), m_line(line) {}

    ~LogStream() {
        std::ostringstream fullMsg;
        fullMsg << "(" << m_file << ":" << m_line << ") " << m_stream.str();
        Logger::getInstance().log(m_level, fullMsg.str());
    }

    template<typename T>
    LogStream& operator<<(const T& value) {
        m_stream << value;
        return *this;
    }

private:
    LogLevel m_level;
    const char* m_file;
    int m_line;
    std::ostringstream m_stream;
};


#endif