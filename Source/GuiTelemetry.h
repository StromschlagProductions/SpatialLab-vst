#pragma once
#include <atomic>
#include <algorithm>

// Read-only observation of the wet output. No DSP control or serialized state.
struct GuiTelemetry
{
    static_assert(std::atomic<float>::is_always_lock_free && std::atomic<unsigned>::is_always_lock_free
                  && std::atomic<int>::is_always_lock_free && std::atomic<bool>::is_always_lock_free,
                  "GUI observation requires lock-free atomics");
    std::atomic<bool> observing {false};
    std::atomic<float> peak {0}, sampleRate {0};
    std::atomic<int> inputs {0}, outputs {0};
    std::atomic<unsigned> blocks {0};
    void publish(float level, int in, int out) noexcept
    {
        auto previous = peak.load(std::memory_order_relaxed);
        // One producer, one GUI consumer. Bounded attempts: never spin on the audio thread.
        if (!peak.compare_exchange_strong(previous, std::max(previous, level), std::memory_order_relaxed))
            peak.compare_exchange_strong(previous, std::max(previous, level), std::memory_order_relaxed);
        inputs.store(in, std::memory_order_relaxed);
        outputs.store(out, std::memory_order_relaxed);
        blocks.fetch_add(1, std::memory_order_relaxed);
    }
};
