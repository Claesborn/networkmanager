#ifndef SCOPEDTIMER
#define SCOPEDTIMER

#include <chrono>
#include <string>
#include <iostream>

class ScopedTimer {
public:
    ScopedTimer(std::string label) 
        : m_label(label), start(std::chrono::steady_clock::now())
    { }
    ~ScopedTimer()
    {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        LOG(logTRACE) << "[Scoped Timer] " << m_label << " - Time taken: " << duration.count();
    }

private:
    std::string m_label;
    std::chrono::steady_clock::time_point start;
};

#endif