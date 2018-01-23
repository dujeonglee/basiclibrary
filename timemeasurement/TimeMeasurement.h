#include <chrono>   // std::chrono
#include <map>      // std::map
#include <mutex>    // std::mutex, std::unique_lock
#include <deque>    // std::deque
#include <iostream> // std::cout
#include <string>   // std::string

#ifdef __linux__
// Linux platform
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >= 40800
typedef std::chrono::steady_clock CLOCK;
#else
typedef std::chrono::monotonic_clock CLOCK;
#endif

#elif _WIN32
// Windows platform
typedef std::chrono::steady_clock CLOCK;
#else
// Other platforms
typedef std::chrono::steady_clock CLOCK;
#endif

typedef std::chrono::time_point<CLOCK> TimePoint;

class TimeMeasure
{
  public:
    static std::map<std::string, std::deque<double>> g_History;
    static std::mutex g_Lock;

  public:
    TimeMeasure() = delete;
    TimeMeasure(const std::string &classStr) : m_Class(classStr)
    {
        m_Start = CLOCK::now();
    }

    ~TimeMeasure()
    {
        std::chrono::duration<double> elapsed_seconds = CLOCK::now() - m_Start;
        std::unique_lock<std::mutex> lock(g_Lock);
        g_History[m_Class].push_back(elapsed_seconds.count());
    }

    static void CollectResults(std::ostream& out)
    {
        std::unique_lock<std::mutex> lock(g_Lock);
        for (std::map<std::string, std::deque<double>>::iterator dq_it = g_History.begin(); dq_it != g_History.end(); ++dq_it)
        {
            double avg = .0;
            for (double t : dq_it->second)
            {
                avg += t;
            }
            out << "Class:      " << dq_it->first << std::endl;
            out << "Counts:     " << dq_it->second.size() << std::endl;
            out << "Total Time: " << avg << std::endl;
            out << "Avg. Time:  " << avg / dq_it->second.size() << std::endl;
        }
    }

    void Reset()
    {
        std::unique_lock<std::mutex> lock(g_Lock);
        g_History.clear();
    }

  private:
    std::string m_Class;
    TimePoint m_Start;
};

std::map<std::string, std::deque<double>> TimeMeasure::g_History;
std::mutex TimeMeasure::g_Lock;
