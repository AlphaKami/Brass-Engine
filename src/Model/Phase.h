#ifndef IRONBRASS_PHASE_H
#define IRONBRASS_PHASE_H

#include "ThreadPool.h"
#include <latch>
#include <functional>

/**
 * Executes a set of jobs on pool and blocks the caller till all jobs are ended.
 * @param pool thread pool
 * @param jobs a vector of functions to execute
 */
inline void runPhaseAndWait(ThreadPool& pool, std::vector<std::function<void()>> jobs) {
    if (jobs.empty()) return;

    std::latch phaseLatch(jobs.size());

    /** FIXME(deadlock): se job() lancia, count_down() non viene mai chiamato e phaseLatch.wait()
     * blocca per sempre (ThreadPool::workerLoop cattura e ignora l'eccezione).
     * Da sistemare prima di collegare la produzione: guard RAII sul count_down,
     * oppure catturare l'eccezione (std::exception_ptr) e rilanciarla dopo wait(). */
    for (auto& job : jobs) {
        pool.enqueue([&phaseLatch, job = std::move(job)]() {
            job();
            phaseLatch.count_down();
        });
    }

    phaseLatch.wait();
}

#endif //IRONBRASS_PHASE_H
