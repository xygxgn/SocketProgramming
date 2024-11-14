#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H
#include <iostream>
#include <queue>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>
#include <condition_variable>


class ThreadPool
{
public:
    ThreadPool(int _minNum = std::thread::hardware_concurrency() / 2, int _maxNum = std::thread::hardware_concurrency());
    ~ThreadPool();
    inline bool isShotdown() { return is_shotdown; }
    void addTask(std::function<void()>);

    // not support for reference args, i.e. args
    template<typename F, typename... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto p_task = std::make_shared<std::packaged_task<return_type()>> 
            (std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = p_task->get_future();
        {
            std::lock_guard<std::mutex> locker(tasks_mutex);
            tasks.emplace([p_task]() {
                (*p_task)();
            });
        }
        workers_condition.notify_one();
        return res;
    }

private:
    void managerCallback();
    void workerCallback();

    std::unique_ptr<std::thread> manager;
    std::map<std::thread::id, std::thread> workers;
    std::vector<std::thread::id> destory_ids;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mutex, destory_ids_mutex;
    std::condition_variable workers_condition;

    std::atomic<int> minThreadNum, maxThreadNum;
    std::atomic<int> totalThreadNum, idleThreadNum, exitThreadNum;
    std::atomic<bool> is_shotdown;
};

#endif
