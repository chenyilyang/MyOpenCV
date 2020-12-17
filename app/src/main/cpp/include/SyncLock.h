//
// Created by chenyilyang on 12/17/20.
//

#ifndef MYNATIAVEAPPLICATION_SYNCLOCK_H
#define MYNATIAVEAPPLICATION_SYNCLOCK_H
#include <pthread.h>
class SyncLock {
private:
    pthread_mutex_t m_mutex;
    pthread_mutexattr_t m_attr;
public:
    SyncLock(){
        pthread_mutexattr_init(&m_attr);
        pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &m_attr);
    }
    ~SyncLock() {
        pthread_mutex_destroy(&m_mutex);
        pthread_mutexattr_destroy(&m_attr);
    }
    int Lock() {
        return pthread_mutex_lock(&m_mutex);
    }
    int UnLock() {
        return pthread_mutex_unlock(&m_mutex);
    }
    int TryLock() {
        return pthread_mutex_trylock(&m_mutex);
    }
};
class ScopeSyncLock {
private:
    SyncLock *m_pLock;
public:
    ScopeSyncLock(SyncLock * pLock) : m_pLock(pLock) {
        if (pLock != nullptr) m_pLock->Lock();
    }
    ~ScopeSyncLock() {
        if (m_pLock != nullptr) m_pLock->UnLock();
    }
};
#endif //MYNATIAVEAPPLICATION_SYNCLOCK_H
