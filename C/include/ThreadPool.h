#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>


// ����ṹ��
typedef struct Task
{
    void (*function)(void* arg);
    void* arg;
} Task;

struct ThreadPool
{
    // �������
    Task* taskQ;
    int queueCapacity;  // ����
    int queueSize;      // ��ǰ�������
    int queueFront;     // ��ͷ -> ȡ����
    int queueRear;      // ��β -> ������

    pthread_t managerID;    // �������߳�ID
    pthread_t *threadIDs;   // �������߳�ID
    int minNum;             // ��С�߳�����
    int maxNum;             // ����߳�����
    int busyNum;            // æ���̵߳ĸ���
    int liveNum;            // �����̵߳ĸ���
    int exitNum;            // Ҫ���ٵ��̸߳���
    pthread_mutex_t mutexPool;  // ���������̳߳�
    pthread_mutex_t mutexBusy;  // ��busyNum����
    pthread_cond_t notFull;     // ��������ǲ�������
    pthread_cond_t notEmpty;    // ��������ǲ��ǿ���

    int shutdown;           // �ǲ���Ҫ�����̳߳�, ����Ϊ1, ������Ϊ0
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
