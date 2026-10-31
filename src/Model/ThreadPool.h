#ifndef IRONBRASS_THREADPOOL_H
#define IRONBRASS_THREADPOOL_H

#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * ThreadPool handle the job queue.
 * Is responsible for accepting pending jobs and delegate them to the workers threads.
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    /**
     * Enqueue accepts a function that will be executed by a working thread.
     * @param job Function to process (note: it's better if those functions are defined inside Model/Systems folder)
     */
    void enqueue(std::function<void()> job);

    /**
     * Returns the number of working threads created based on the computer physical cores.
     * @return Number of threads
     */
    [[nodiscard]] size_t workerCount() const { return workers.size(); }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stopFlag;

    void workerLoop();
};

#endif //IRONBRASS_THREADPOOL_H
