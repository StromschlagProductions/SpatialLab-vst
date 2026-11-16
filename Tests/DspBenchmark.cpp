#include "RoomEngine.h"
#include <chrono>
#include <iostream>

// Informational release-build benchmark: no machine-dependent pass/fail threshold.
// Includes 64-frame target refresh, one active preallocated preset, no host/GUI.
int main()
{
    std::cout << "rate,room,layout,controls,ns_per_frame,single_core_percent,checksum\n";
    for (double rate : {44100.0, 48000.0})
        for (int preset=0;preset<26;++preset)
        for (bool mono : {true, false})
            for (int mode : {0, 1, 2})
            {
                spatiallab::Engine engine;
                engine.prepare(rate, preset, 0.5f, 0.5f);
                const int samples = static_cast<int>(rate * 4);
                float checksum = 0;
                const auto start = std::chrono::steady_clock::now();
                for (int i = 0; i < samples; ++i)
                {
                    if (i % 64 == 0)
                    {
                        const int block = i / 64;
                        engine.set(mode == 2 ? (preset + block / 32) % 26 : preset,
                            mode == 1 ? static_cast<float>(block % 2) : 0.5f,
                            mode == 1 ? static_cast<float>((block / 2) % 2) : 0.5f);
                    }
                    const float left = i % 97 < 48 ? 0.1f : -0.1f;
                    const auto y = engine.process(left, mono ? left : -0.7f * left);
                    checksum += y.left + y.right;
                }
                const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                std::cout << rate << ',' << preset << ',' << (mono ? "mono" : "stereo") << ',' << (mode == 0 ? "steady" : mode == 1 ? "macro_automation" : "preset_switch")
                          << ',' << elapsed * 1e9 / samples << ',' << elapsed * 25 << ',' << checksum << '\n';
            }
}
