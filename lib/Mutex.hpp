#ifndef LIB_MUTEX_HPP
#define LIB_MUTEX_HPP

#include <stdexcept>
#include <windows.h>

class Mutex {
private:
    HANDLE handle_;
public:
    Mutex() : handle_(CreateMutexW(NULL, FALSE, NULL)) {
        if (!handle_) {
            throw std::runtime_error("Failed to create mutex");
        }
    }
    ~Mutex() {
        if (handle_) {
            CloseHandle(handle_);
        }
    }
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void lock() {
        WaitForSingleObject(handle_, INFINITE);
    }
    void unlock() {
        ReleaseMutex(handle_);
    }
};

class LockGuard {
private:
    Mutex& mutex_;
public:
    explicit LockGuard(Mutex& m) : mutex_(m) {
        mutex_.lock();
    }
    ~LockGuard() {
        mutex_.unlock();
    }
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
};

#endif // LIB_MUTEX_HPP
