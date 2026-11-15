#include "RoomEngine.h"
#include <chrono>
#include <iostream>

// Informational release-build benchmark: no machine-dependent pass/fail threshold.
// Includes 64-frame target refresh, all three continuously running rooms, no host/GUI.
int main()
{
    std::cout << "rate,layout,controls,ns_per_frame,single_core_percent,checksum\n";
    for (double rate : {44100.0, 48000.0})
        for (bool mono : {true, false})
            for (bool automated : {false, true})
            {
                spatiallab::Engine engine;
                engine.prepare(rate, 0, 0.5f, 1, 0.5f, 1, 1);
                const int samples = static_cast<int>(rate * 4);
                float checksum = 0;
                const auto start = std::chrono::steady_clock::now();
                for (int i = 0; i < samples; ++i)
                {
                    if (i % 64 == 0)
                    {
                        const int block = i / 64;
                        engine.set(automated ? block % 3 : 0, automated ? static_cast<float>(block % 2) : 0.5f,
                            automated && block % 2 ? 2.0f : 1.0f, 0.5f, 1, 1);
                    }
                    const float left = i % 97 < 48 ? 0.1f : -0.1f;
                    const auto y = engine.process(left, mono ? left : -0.7f * left);
                    checksum += y.left + y.right;
                }
                const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                std::cout << rate << ',' << (mono ? "mono" : "stereo") << ',' << (automated ? "automated" : "steady")
                          << ',' << elapsed * 1e9 / samples << ',' << elapsed * 25 << ',' << checksum << '\n';
            }
}
