#pragma once

#include <chrono>

namespace rib
{

/**
 * @brief Elapsed Timer
 * @par example
 * @code 
 * ElapsedTimer et;
 * { some proccess }
 * et.stop();
 * std::cout << et.elapsed<std::chrono::nanoseconds>() << std::endl;
 * @endcode
 */
struct ElapsedTimer
{
    ElapsedTimer() { start = std::chrono::system_clock::now(); }
    void stop() { end = std::chrono::system_clock::now(); }

    template <class Duration = std::chrono::milliseconds>
    double elapsed() { return std::chrono::duration_cast<Duration>(end - start).count(); }

private:
    std::chrono::system_clock::time_point start, end;
};

} // namespace rib
