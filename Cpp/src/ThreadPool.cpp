#include "ThreadPool.h"


ThreadPool::ThreadPool(int _minNum, int _maxNum) : minThreadNum(_minNum), maxThreadNum(_maxNum),
    is_shotdown(false), totalThreadNum(_minNum), idleThreadNum(_minNum), exitThreadNum(0)
{
    manager.reset(new std::thread(&ThreadPool::managerCallback, this));
    std::cout << "create manager thread, id: " << manager->get_id() << std::endl;
    for (int i = 0; i < totalThreadNum; ++i)
    {
        std::thread t(&ThreadPool::workerCallback, this);
        std::cout << "create worker thread, id: " << t.get_id() << std::endl;
        workers.insert(std::make_pair(t.get_id(), std::move(t)));
    }
};

ThreadPool::~ThreadPool()
{
    is_shotdown.store(true);
    workers_condition.notify_all();
    for (auto &worker : workers)
    {
        if (worker.second.joinable())
        {
            std::cout << "exit worker thread, id: " << worker.second.get_id() << std::endl;
            worker.second.join();
        }
    }
    if (manager->joinable())
    {
        std::cout << "exit manager thread, id: " << manager->get_id() << std::endl;
        manager->join();
    }
};


void ThreadPool::addTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> locker(tasks_mutex);
        tasks.emplace(task);
    }
    workers_condition.notify_one();
}


void ThreadPool::managerCallback()
{
    while (!is_shotdown.load())
    {
        {
            std::lock_guard<std::mutex> locker(destory_ids_mutex);
            for (auto id : destory_ids)
            {
                auto iter = workers.find(id);
                if (iter != workers.end())
                {
                    std::cout << "exit thread, id: " << iter->first << std::endl;
                    if (iter->second.joinable())
                        iter->second.join();
                    workers.erase(iter);
                }
            }
            destory_ids.clear();
        }

        int idle_num = idleThreadNum.load();
        int total_num = totalThreadNum.load();
        if (idle_num == 0 && total_num < maxThreadNum) // add the new thread
        {
            std::thread t(&ThreadPool::workerCallback, this);
            std::cout << "create thread, id: " << t.get_id() << std::endl;
            workers.insert(std::make_pair(t.get_id(), std::move(t)));
            ++totalThreadNum;
            ++idleThreadNum;
        }
        else if (idle_num > total_num / 2 && total_num > minThreadNum) // kill the idle thread
        {
            int destory_num = std::min(2, total_num - minThreadNum);
            exitThreadNum.store(destory_num);
            workers_condition.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}


void ThreadPool::workerCallback()
{
    while (!is_shotdown.load())
    {
        std::unique_lock<std::mutex> locker(tasks_mutex);
        while (tasks.empty() && !is_shotdown.load())
        {
            workers_condition.wait(locker);
            if (exitThreadNum > 0)
            {
                --totalThreadNum;
                --idleThreadNum;
                --exitThreadNum;
                std::cout << "ready to exit thread, id: " << std::this_thread::get_id() << std::endl;
                std::lock_guard<std::mutex> locker(destory_ids_mutex);
                destory_ids.push_back(std::this_thread::get_id());
                return;
            }
        }
        if (!tasks.empty() && !is_shotdown.load())
        {
            std::function<void()> task(std::move(tasks.front()));
            tasks.pop();
            locker.unlock();
            std::cout << "get a task..." << std::endl;
            if (task)
            {
                --idleThreadNum;
                task();
                ++idleThreadNum;
            }
        }
    }
}




