#ifndef SINGLESHOTTIMER_H_
#define SINGLESHOTTIMER_H_

#include <chrono>
#include <ctime>
#include <algorithm>
#include "ThreadPool.h"

//#define BUSYWAITING
namespace dujeonglee
{
namespace basiclibrary
{
#ifdef __linux__
// Linux platform
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION >= 40700
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

// Task information.
class TaskInformation
{
  public:
    // When to execute 'm_Task'.
    TimePoint m_ExecutionTime;
    // Task for execution.
    std::function<void()> m_Task;
    // Priority
    uint32_t m_Priority;
    // TaskID used to cancel the task.
    uint32_t m_TaskID;
    // If the task is canceled, 'm_Active' is marked as 'false'.
    bool m_Active;
};

template <uint32_t PRIORITY, uint32_t THREAD>
class SingleShotTimer
{
  private:
    // Synchronization of API calls among applications
    std::mutex m_APILock;

    // Synchronization on the access to m_ActiveTaskInformationList.
    std::mutex m_ActiveTaskInformationListLock;
    // Condition variable to wake up 'm_Thread'. The signal is generated around task's execution time.
    std::condition_variable m_Condition;
    // A list of tasks
    std::vector<TaskInformation *> m_ActiveTaskInformationList;
    // Main loop checking execution timeof tasks.
    std::thread m_Thread;
    // When a task reaches to its execution time, the task is pushed into 'm_ThreadPool' by 'm_Thread'.
    ThreadPool<PRIORITY, THREAD> m_ThreadPool;
    // The state of SingleShotTimer.
    std::atomic<bool> m_Running;
    // Generate unique TaskID
    uint32_t m_TaskID;
#define INVALID_TASK_ID ((uint32_t)0)
#define IMMEDIATE_TASK_ID ((uint32_t)1)
#define MINIMUM_TASK_ID ((uint32_t)2)
  public:
    // Ctor. Initialize member variables and call 'Start'
    SingleShotTimer() : m_Running(false),
                        m_TaskID(MINIMUM_TASK_ID)
    {
        Start();
    }

    // Dtor. Call 'Stop'
    ~SingleShotTimer()
    {
        Stop();
    }

    // Schedule a task being executed immediately with priority.
    uint32_t ImmediateTaskNoExcept(const std::function<void()> to, const uint32_t priority = 0)
    {
        uint32_t ret = INVALID_TASK_ID;
        while (ret == INVALID_TASK_ID && m_Running)
        {
            ret = ImmediateTask(to, priority);
        }
        return ret;
    }

    // Schedule a task being executed after milli with priority.
    uint32_t ScheduleTaskNoExcept(const uint32_t milli, const std::function<void()> to, const uint32_t priority = 0)
    {
        uint32_t ret = INVALID_TASK_ID;
        while (ret == INVALID_TASK_ID && m_Running)
        {
            ret = ScheduleTask(milli, to, priority);
        }
        return ret;
    }

    // Schedule a task being executed immediately with priority.
    // This function may fail.
    uint32_t ImmediateTask(const std::function<void()> to, const uint32_t priority = 0)
    {
        return ScheduleTask(0, to, priority);
    }

    // Schedule a task being executed after milli with priority.
    // This function may fail.
    uint32_t ScheduleTask(const uint32_t milli, const std::function<void()> to, const uint32_t priority = 0)
    {
        std::unique_lock<std::mutex> APILock(m_APILock);
        if (m_Running == false)
        {
            return INVALID_TASK_ID;
        }
        if (milli == 0)
        {
            return (m_ThreadPool.Enqueue([to]() { to(); }, priority) ? IMMEDIATE_TASK_ID : INVALID_TASK_ID); /*One cannot cancel immediate task*/
        }
        TaskInformation *newone = nullptr;
        // 1. Get a free TaskInformation
        try
        {
            newone = new TaskInformation();
        }
        catch (const std::bad_alloc &ex)
        {
            return INVALID_TASK_ID;
        }
        // 2. Setup TaskInformation
        newone->m_ExecutionTime = CLOCK::now() + std::chrono::milliseconds(milli);
        newone->m_Task = to;
        newone->m_Priority = priority;
        const uint32_t TID = newone->m_TaskID = m_TaskID++;
        if (m_TaskID < MINIMUM_TASK_ID)
        {
            m_TaskID = MINIMUM_TASK_ID;
        }
        newone->m_Active = true;

        // 3. Push TaskInformation into ActiveTimerList, which is min heap.
        {
            std::unique_lock<std::mutex> ActiveTaskInformationListLock(m_ActiveTaskInformationListLock);
            try
            {
                m_ActiveTaskInformationList.push_back(newone);
            }
            catch (std::bad_alloc &ex)
            {
                std::cout << ex.what() << std::endl;
                delete newone;
                return INVALID_TASK_ID;
            }
            std::push_heap(m_ActiveTaskInformationList.begin(), m_ActiveTaskInformationList.end(), [](TaskInformation *&a, TaskInformation *&b) -> bool {
                return a->m_ExecutionTime > b->m_ExecutionTime;
            });
        }
        m_Condition.notify_one();
        return TID;
    }

    // Cancel Task of which ID is taskid.
    void CancelTask(const uint32_t taskid)
    {
        std::unique_lock<std::mutex> APILock(m_APILock);
        if (m_Running == false)
        {
            return;
        }
        if (taskid < MINIMUM_TASK_ID)
        {
            return;
        }
        std::unique_lock<std::mutex> ActiveTaskInformationListLock(m_ActiveTaskInformationListLock);
        for (uint32_t i = 0; i < m_ActiveTaskInformationList.size(); i++)
        {
            if (m_ActiveTaskInformationList[i]->m_TaskID == taskid)
            {
                m_ActiveTaskInformationList[i]->m_Active = false;
                return;
            }
        }
    }

    // Start the main thread loop which check tasks execution time and push them into 'm_ThreadPool'.
    void Start()
    {
        std::unique_lock<std::mutex> APILock(m_APILock);
        if (m_Running)
        {
            return;
        }
        m_ThreadPool.Start();
        SingleShotTimer *const self = this;
        m_Thread = std::thread([self]() {
            while (self->m_Running)
            {
                TaskInformation *task = nullptr;
                {
                    std::unique_lock<std::mutex> ActiveTaskInformationListLock(self->m_ActiveTaskInformationListLock);
                    if (self->m_ActiveTaskInformationList.size() == 0)
                    {
                        while (self->m_ActiveTaskInformationList.size() == 0 && self->m_Running)
                        {
                            self->m_Condition.wait_for(ActiveTaskInformationListLock, std::chrono::milliseconds(500));
                        }
                    }
                    if (!self->m_Running)
                    {
                        return;
                    }
#ifdef BUSYWAITING
                    if (self->m_ActiveTaskInformationList[0]->m_ExecutionTime <= CLOCK::now() && self->m_Running)
                    {
                        std::pop_heap(self->m_ActiveTaskInformationList.begin(), self->m_ActiveTaskInformationList.end(), [](TaskInformation *&a, TaskInformation *&b) -> bool {
                            return a->m_ExecutionTime > b->m_ExecutionTime;
                        });
                        task = self->m_ActiveTaskInformationList.back();
                        self->m_ActiveTaskInformationList.pop_back();
                    }
                    else
                    {
                        if (self->m_Running == false)
                        {
                            return;
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                        continue;
                    }
#else
                    while (self->m_ActiveTaskInformationList[0]->m_ExecutionTime > CLOCK::now() && self->m_Running)
                    {
                        self->m_Condition.wait_for(ActiveTaskInformationListLock, self->m_ActiveTaskInformationList[0]->m_ExecutionTime - CLOCK::now());
                    }
                    if (self->m_Running == false)
                    {
                        return;
                    }
                    std::pop_heap(self->m_ActiveTaskInformationList.begin(), self->m_ActiveTaskInformationList.end(), [](TaskInformation *&a, TaskInformation *&b) -> bool {
                        return a->m_ExecutionTime > b->m_ExecutionTime;
                    });
                    task = self->m_ActiveTaskInformationList.back();
                    self->m_ActiveTaskInformationList.pop_back();
#endif
                }
                if (task && task->m_Active)
                {
                    self->m_ThreadPool.Enqueue([task]() {
                        if (task->m_Task)
                        {
                            task->m_Task();
                        }
                        delete task;
                    },
                                               task->m_Priority);
                }
            }
        });
        m_Running = true;
    }

    // Stop the main thread loop and release resources.
    void Stop()
    {
        std::unique_lock<std::mutex> APIlock(m_APILock);
        if (!m_Running)
        {
            return;
        }
        m_Running = false;
        m_Condition.notify_one();
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
        {
            std::unique_lock<std::mutex> ActiveTaskInformationListLock(m_ActiveTaskInformationListLock);
            for (uint32_t i = 0; i < m_ActiveTaskInformationList.size(); i++)
            {
                delete m_ActiveTaskInformationList[i];
            }
            m_ActiveTaskInformationList.clear();
        }
        m_ThreadPool.Stop();
    }

    // Return the number of pending tasks.
    size_t Tasks()
    {
        std::unique_lock<std::mutex> APIlock(m_APILock);
        std::unique_lock<std::mutex> ActiveTaskInformationListLock(m_ActiveTaskInformationListLock);
        return m_ActiveTaskInformationList.size();
    }

    // Return state of SingleShotTimer.
    bool Running()
    {
        std::unique_lock<std::mutex> APIlock(m_APILock);
        return m_Running;
    }

  private:
    // Wrapper function used by 'PeriodicTask'
    static void PeriodicTaskWrapper(SingleShotTimer<PRIORITY, THREAD> *const timer, const uint32_t interval, const std::function<const bool(void)> task, const uint32_t priority = 0)
    {
        std::function<void()> wrapper = std::bind(SingleShotTimer<PRIORITY, THREAD>::PeriodicTaskWrapper, timer, interval, task, priority);
        if (task() && timer->m_Running)
        {
            timer->ScheduleTaskNoExcept(interval, wrapper, priority);
        }
    }
    // Wrapper function used by 'PeriodicTaskAdv'
    static void PeriodicTaskWrapperAdv(SingleShotTimer<PRIORITY, THREAD> *const timer, const std::function<const std::tuple<bool, uint32_t, uint32_t>(void)> task)
    {
        std::function<void()> wrapper = std::bind(SingleShotTimer<PRIORITY, THREAD>::PeriodicTaskWrapperAdv, timer, task);
        const std::tuple<bool, uint32_t, uint32_t> ret = task();
        if (std::get<0>(ret) && timer->m_Running)
        {
            timer->ScheduleTaskNoExcept(std::get<1>(ret), wrapper, std::get<2>(ret));
        }
    }

  public:
    // Start periodic task
    void PeriodicTask(const uint32_t interval, const std::function<const bool(void)> task, const uint32_t priority = 0)
    {
        ImmediateTaskNoExcept([this, interval, task, priority]() {
            SingleShotTimer<PRIORITY, THREAD>::PeriodicTaskWrapper(this, interval, task, priority);
        },
                              priority);
    }
    // Start periodic task
    void PeriodicTaskAdv(const std::function<const std::tuple<bool, uint32_t, uint32_t>(void)> task, const uint32_t priority = 0)
    {
        ImmediateTaskNoExcept([this, task]() {
            SingleShotTimer<PRIORITY, THREAD>::PeriodicTaskWrapperAdv(this, task);
        },
                              priority);
    }
};

// Alias.
template <uint32_t PRIORITY, uint32_t CUNCURRENCY>
using TaskScheduler = SingleShotTimer<PRIORITY, CUNCURRENCY>;

#undef GCC_VERSION
}
}
#endif
