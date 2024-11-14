#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>


// 任务结构体
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
} Task;

struct ThreadPool
{
    // 任务队列
    Task* taskQ;
    int queueCapacity;  // 容量
    int queueSize;      // 当前任务个数
    int queueFront;     // 队头 -> 取数据
    int queueRear;      // 队尾 -> 放数据

    pthread_t managerID;    // 管理者线程ID
    pthread_t *threadIDs;   // 工作的线程ID
    int minNum;             // 最小线程数量
    int maxNum;             // 最大线程数量
    int busyNum;            // 忙的线程的个数
    int liveNum;            // 存活的线程的个数
    int exitNum;            // 要销毁的线程个数
    pthread_mutex_t mutexPool;  // 锁整个的线程池
    pthread_mutex_t mutexBusy;  // 锁busyNum变量
    pthread_cond_t notFull;     // 任务队列是不是满了
    pthread_cond_t notEmpty;    // 任务队列是不是空了

    int shutdown;           // 是不是要销毁线程池, 销毁为1, 不销毁为0
};

typedef struct ThreadPool ThreadPool;
ThreadPool *threadPoolCreate(int min, int max, int queueSize);

int threadPoolDestroy(ThreadPool* pool);

void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);

int threadPoolBusyNum(ThreadPool* pool);

int threadPoolAliveNum(ThreadPool* pool);

void* worker(void* arg);

void* manager(void* arg);

void threadExit(ThreadPool* pool);

#endif  // _THREADPOOL_H
