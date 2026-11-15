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
                engine.prepare(rate, room, 0.5f, 0.5f);
                double early = 0, tail = 0, stereoDifference = 0;
                for (int i = 0; i < static_cast<int>(rate * 6); ++i)
                {
                    const float input = i == 0 ? 1.0f : 0.0f;
                    const auto out = engine.process(input, input);
                    if (i == 0) require(out.left == 0 && out.right == 0, "Direct impulse leaked");
                    require(std::isfinite(out.left) && std::isfinite(out.right), "Nonfinite room response");
                    require(std::abs(out.left) < 2 && std::abs(out.right) < 2, "Unstable impulse response");
                    const double e = out.left * out.left + out.right * out.right;
                    if (i < rate) early += e;
                    if (i > rate * 5) tail += e;
                    stereoDifference += std::abs(out.left - out.right);
                }
                require(early > 0.01 && tail < early * 1e-5, "Missing or nondecaying room tail");
                require(stereoDifference > 0.01, "Mono input has no stereo room image");
                energies[static_cast<std::size_t>(room)] = early;
                // At neutral macros the preset retains both its early and late components.
                spatiallab::Room preset;
                preset.prepare(rate, spatiallab::rooms[static_cast<std::size_t>(room)], 1, 0.5f);
                engine.prepare(rate, room, 0.5f, 0.5f);
                double earlyField = 0, lateField = 0;
                for (int i = 0; i < static_cast<int>(rate * 0.2); ++i)
                {
                    const float x = i == 0 ? 1.0f : 0;
                    const auto fields = preset.process(x, 0);
                    const auto wet = engine.process(x, 0);
                    require(std::abs(wet.left - fields.early.left - fields.late.left) < 1e-6f
                         && std::abs(wet.right - fields.early.right - fields.late.right) < 1e-6f,
                         "Neutral macro changed the preset voicing");
                    earlyField += fields.early.left * fields.early.left;
                    lateField += fields.late.left * fields.late.left;
                }
                require(earlyField > 0.01 && lateField > 0.01, "Preset lost internal early/late field");
                // Size must extend late energy; brighter tone must increase normalized HF.
                spatiallab::Engine small, bright, dark;
                small.prepare(rate, room, 0, 1);
                bright.prepare(rate, room, 1, 1);
                dark.prepare(rate, room, 1, 0);
                double smallEnergy = 0, brightEnergy = 0, darkEnergy = 0, brightHigh = 0, darkHigh = 0;
                float previousBright = 0, previousDark = 0;
                for (int i = 0; i < static_cast<int>(rate); ++i)
                {
                    const float input = i == 0 ? 1.0f : 0;
                    const auto a = small.process(input, input), b = bright.process(input, input), c = dark.process(input, input);
                    if (i > rate * 0.3)
                    {
                        smallEnergy += a.left * a.left;
                        brightEnergy += b.left * b.left; darkEnergy += c.left * c.left;
                        brightHigh += (b.left - previousBright) * (b.left - previousBright);
                        darkHigh += (c.left - previousDark) * (c.left - previousDark);
                    }
                    previousBright = b.left; previousDark = c.left;
                }
                require(smallEnergy > 0 && darkEnergy > 0 && brightEnergy > smallEnergy * 4, "Size does not extend tail");
                require(darkHigh / darkEnergy < 0.5 * brightHigh / brightEnergy, "Tone does not change spectral decay");
            }
            require(std::abs(energies[0] - energies[2]) > 0.001, "Rooms sound identical");
            engine.reset();
            for (int i = 0; i < 10000; ++i)
            {
                const auto out = engine.process(0, 0);
                require(out.left == 0 && out.right == 0, "Reset left a tail");
            }
            engine.prepare(rate, 2, 0.5f, 0.5f);
            for (int i = 0; i < 10000; ++i) engine.process(i == 0 ? 1.0f : 0, 0);
            for (int room : {0, 2})
            {
                engine.set(room, 0.5f, 0.5f);
                for (int i = 0; i < 10000; ++i)
                {
                    const auto out = engine.process(0, 0);
                    require(out.left == 0 && out.right == 0, "Preset switch blended or revived an old tail");
                }
            }
            engine.prepare(rate, 0, 0.5f, 0.5f);
            watch = true;
            bool finite = true;
            for (int block = 0; block < 1000; ++block)
            {
                engine.set((block / 17) % 3, static_cast<float>(block % 2), static_cast<float>((block / 2) % 2));
                for (int i = 0; i < 127; ++i)
                {
                    const float input = std::sin(static_cast<float>(block * 127 + i) * 0.071f) * 0.5f;
                    const auto out = engine.process(input, -input);
                    finite = finite && std::isfinite(out.left) && std::isfinite(out.right)
                        && std::abs(out.left) < 10 && std::abs(out.right) < 10;
                }
            }
            watch = false;
            require(finite, "Macro automation was unstable");
            require(allocations == 0, "DSP processing or preset selection allocated");
            // Refreshing unchanged targets must not restart sample smoothing.
            spatiallab::Engine a, b;
            a.prepare(rate, 0, 0.5f, 0.5f); b.prepare(rate, 0, 0.5f, 0.5f);
            a.set(0, 1, 0);
            for (int i = 0; i < 20000; ++i)
            {
                if (i % 7 == 0) b.set(0, 1, 0);
                const auto x = a.process(i == 0 ? 1.0f : 0, 0), y = b.process(i == 0 ? 1.0f : 0, 0);
                require(x.left == y.left && x.right == y.right, "Block size changed smoothing");
            }
            std::cout << "PASS " << rate << " Hz: wet-only, preset voicing, size/tone, hard preset reset, allocation/automation, blocks\n";
        }
        for (double rate : {96000.0, 192000.0})
        {
            spatiallab::Engine e; e.prepare(rate, 2, 1, 1);
            double energy = 0;
            for (int i = 0; i < static_cast<int>(rate * 0.1); ++i)
            {
                const auto y = e.process(i == 0 ? 1.0f : 0, 0);
                require(std::isfinite(y.left) && std::isfinite(y.right), "High-rate smoke nonfinite");
                if (i == 0) require(y.left == 0 && y.right == 0, "High-rate direct leak");
                energy += y.left * y.left + y.right * y.right;
            }
            require(energy > 0.01, "High-rate smoke silence");
        }
        std::cout << "PASS high-rate smoke\n";
    }
    catch (const std::exception& e) { watch = false; std::cerr << e.what() << '\n'; return 1; }
}
