#include "ThreadPool.h"

ThreadPool::ThreadPool(const size_t numThreads) : stopFlag(false) {
    this->workers.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        this->workers.emplace_back([this]() { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(this->queueMutex);
        this->stopFlag = true;
    }
    this->condition.notify_all();
    for (auto& w : this->workers) {
        if (w.joinable()) w.join();
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::lock_guard<std::mutex> lock(this->queueMutex);
        this->jobs.push(std::move(job));
    }
    this->condition.notify_one();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            this->condition.wait(lock, [this]() { return this->stopFlag || !this->jobs.empty(); });
            if (this->stopFlag && this->jobs.empty()) return;
            job = std::move(this->jobs.front());
            this->jobs.pop();
        }
        try {
            job();
        } catch (const std::exception& e) {
            std::cerr << "[ThreadPool] Unhandled exception in a job: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "[ThreadPool] Unknown exception in a job\n";
        }
    }
}
