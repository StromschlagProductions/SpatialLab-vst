#include "RoomEngine.h"
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>

static std::atomic<bool> watch {false};
static std::atomic<int> allocations {0};
void* operator new(std::size_t n)
{
    if (watch.load()) ++allocations;
    if (auto* p = std::malloc(n == 0 ? 1 : n)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
static void require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }

int main()
{
    try
    {
        for (double rate : {44100.0, 48000.0})
        {
            spatiallab::Engine engine;
            std::array<double, 3> energies {};
            for (int room = 0; room < 3; ++room)
            {
                engine.prepare(rate, room, 0.5f, 1, 0.5f, 1, 1);
                double early = 0, tail = 0, stereoDifference = 0;
                for (int i = 0; i < static_cast<int>(rate * 6); ++i)
                {
                    const auto out = engine.process(i == 0 ? 1.0f : 0.0f, i == 0 ? 1.0f : 0.0f);
                    if (i == 0) require(out.left == 0 && out.right == 0, "100% wet leaks direct impulse");
                    require(std::isfinite(out.left) && std::isfinite(out.right), "Nonfinite impulse response");
                    require(std::abs(out.left) < 2 && std::abs(out.right) < 2, "Unstable impulse response");
                    const double e = out.left * out.left + out.right * out.right;
                    if (i < rate) early += e;
                    if (i > rate * 5) tail += e;
                    stereoDifference += std::abs(out.left - out.right);
                }
                require(early > 0.01 && tail < early * 1e-5, "Missing or nondecaying room tail");
                require(stereoDifference > 0.01, "Mono input has no stereo room image");
                energies[static_cast<std::size_t>(room)] = early;
            }
            require(std::abs(energies[0] - energies[2]) > 0.001, "Rooms sound identical");
            engine.prepare(rate, 2, 0.5f, 2, 0, 0, 1);
            for (int i = 0; i < 10000; ++i)
            {
                const auto out = engine.process(i == 0 ? 1.0f : 0, 0);
                require(out.left == out.right, "Width zero is not mono compatible");
            }
            engine.reset();
            for (int i = 0; i < 10000; ++i)
            {
                auto out = engine.process(0, 0);
                require(out.left == 0 && out.right == 0, "Reset left a tail");
            }
            engine.prepare(rate, 0, 0.5f, 1, 0.5f, 1, 1);
            watch = true;
            bool finite = true;
            for (int block = 0; block < 1000; ++block)
            {
                engine.set(block % 3, (block % 2) ? 1.0f : 0.0f, (block % 2) ? 2.0f : 0.5f,
                    (block % 2) ? 0.0f : 1.0f, 1, 1.995262f);
                for (int i = 0; i < 127; ++i)
                {
                    const float input = std::sin(static_cast<float>(block * 127 + i) * 0.071f) * 0.5f;
                    auto out = engine.process(input, -input);
                    finite = finite && std::isfinite(out.left) && std::isfinite(out.right)
                        && std::abs(out.left) < 10 && std::abs(out.right) < 10;
                }
            }
            watch = false;
            require(finite, "Automation stress was unstable");
            require(allocations == 0, "Audio processing allocated heap memory");
            // Every balance setting stays wet; early-only ends after the last reflection,
            // late-only cannot precede the shortest FDN delay. Midpoint preserves both fields.
            for (int room = 0; room < 3; ++room)
            {
                spatiallab::Engine earlyOnly, lateOnly, combined;
                earlyOnly.prepare(rate, room, 0, 1, 0.5f, 1, 1);
                lateOnly.prepare(rate, room, 1, 1, 0.5f, 1, 1);
                combined.prepare(rate, room, 0.5f, 1, 0.5f, 1, 1);
                double earlyEnergy = 0, lateEnergy = 0;
                for (int i = 0; i < static_cast<int>(rate * 0.3); ++i)
                {
                    const float input = i == 0 ? 1.0f : 0;
                    const auto e = earlyOnly.process(input, 0), l = lateOnly.process(input, 0);
                    const auto c = combined.process(input, 0);
                    if (i < rate * 0.001)
                        require(e.left == 0 && e.right == 0 && l.left == 0 && l.right == 0,
                            "Balance endpoint leaked direct signal");
                    if (i > rate * 0.05) require(e.left == 0 && e.right == 0, "Early-only contains late tail");
                    if (i < rate * 0.015) require(l.left == 0 && l.right == 0, "Late-only contains early reflections");
                    require(std::abs(c.left - 0.5f * (e.left + l.left)) < 1e-6f
                         && std::abs(c.right - 0.5f * (e.right + l.right)) < 1e-6f, "Balance midpoint changed room voicing");
                    earlyEnergy += e.left * e.left + e.right * e.right;
                    lateEnergy += l.left * l.left + l.right * l.right;
                }
                require(earlyEnergy > 0.01 && lateEnergy > 0.01, "Balance endpoint is silent");
            }
            std::cout << "PASS " << rate << " Hz: wet-only, early/late, rooms, decay, reset, automation, allocations\n";
        }
        // Repeated block-boundary target refresh must not restart ramps.
        for (double rate : {44100.0, 48000.0})
        {
        spatiallab::Engine a, b;
        a.prepare(rate, 0, 0, 1, 0.5f, 1, 1);
        b.prepare(rate, 0, 0, 1, 0.5f, 1, 1);
        a.set(2, 1, 2, 0, 0.3f, 0.5f);
        for (int i = 0; i < 20000; ++i)
        {
            if (i % 7 == 0) b.set(2, 1, 2, 0, 0.3f, 0.5f);
            const auto x = a.process(i == 0 ? 1.0f : 0, 0);
            const auto y = b.process(i == 0 ? 1.0f : 0, 0);
            require(x.left == y.left && x.right == y.right, "Block size changes smoothing");
        }
        }
        // Higher rates receive a short correctness smoke test, not full tuning/CPU coverage.
        for (double rate : {96000.0, 192000.0})
        {
            spatiallab::Engine e;
            e.prepare(rate, 2, 0.5f, 2, 0, 1, 1);
            double energy = 0;
            for (int i = 0; i < static_cast<int>(rate * 0.1); ++i)
            {
                const auto y = e.process(i == 0 ? 1.0f : 0, 0);
                require(std::isfinite(y.left) && std::isfinite(y.right), "High-rate smoke nonfinite");
                if (i == 0) require(y.left == 0 && y.right == 0, "High-rate dry leak");
                energy += y.left * y.left + y.right * y.right;
            }
            require(energy > 0.01, "High-rate smoke silence");
        }
        std::cout << "PASS high-rate smoke and primary-rate block partition invariance\n";
    }
    catch (const std::exception& e) { watch = false; std::cerr << e.what() << '\n'; return 1; }
}
