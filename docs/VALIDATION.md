# Validation record

Date: 2026-11-15. Development branch: `develop/juce8-room-foundation`.

## Automated coverage

- Native GCC 14.2 / C++17 release build and CTest: passed locally.
- Native debug build with AddressSanitizer and UndefinedBehaviorSanitizer: passed locally.
- The full DSP suite targets **44.1 and 48 kHz**: distinct nonzero room responses, no direct impulse at any early/late endpoint, isolated early/late fields and original midpoint voicing, stereo diffusion from mono input, finite bounded impulse responses, tail decay, width-zero fold-down, reset-to-silence, and repeated extreme parameter changes. Higher rates (96/192 kHz) receive short correctness smoke tests only.
- A test allocation hook measures zero ordinary heap allocations during DSP processing and target updates. It does not instrument the host or OS and is not a proof of hard real-time scheduling.
- Repeated target refresh at different block boundaries produces identical sample results; unchanged targets do not restart ramps.
- Windows CI builds the actual JUCE VST3 and processor tests. The processor suite covers all supported layouts, rejected stereo-to-mono/surround layouts, wet mono expansion/fold-down against the independent engine, actual callback sizes 0/1/7/64/511/2048, 44.1/48 kHz reprepare, send-bypass silence, all parameter values on session recall, schema-1 migration (including old mix=0 without dry leakage), invalid binary state and future state-schema rejection.
- GitHub Actions runs the same standalone DSP suite on Linux with ASan/UBSan and retains a Windows VST3 bundle on successful completion. Consult the linked PR checks for the exact tested SHA; a green DSP-only job does not establish that the plugin compiled.

## CPU observations (informational)

`SpatialLabDspBenchmark` is a release-build benchmark at 44.1/48 kHz for mono and stereo inputs, with steady controls and with room/balance/decay automation every 64 frames. Each case processes four seconds, includes all three running room engines, and reports ns/frame, average percent of one CPU core and an output checksum. Windows CI saves `dsp-benchmark.csv` with the VST3 artifact. Timing is not a correctness test and has no hardware-dependent failure threshold.

On this Linux Cloud CPU, GCC 14.2 release runs measured approximately **94–107 ns/frame** with steady controls and **110–124 ns/frame** under automation (about **0.42–0.55% of one core** at the primary rates). An ad-hoc same-machine steady-control comparison before/after replacing ring-buffer division and caching unchanged coefficients measured about 197–229 ns/frame before and 96–108 ns/frame after. This is indicative only: short runs include noise, the send architecture changed too, and neither benchmark measures DAW callback deadlines, host/GUI overhead or worst-case scheduling. No audible-quality claim follows from CPU numbers.

## Before release: manual checks still required

- Load/rescan the complete Windows VST3 bundle in at least two DAWs; verify mono/stereo track and send-bus operation and the generic editor.
- Listen to dialogue, impulses, sustained tones and transient-heavy material in all three rooms. Evaluate coloration, tail density, stereo image and mono fold-down.
- Write and replay automation for every control, especially repeated room switches and early/late endpoints. Listen for clicks and unexpected gain changes.
- Save/reopen projects, duplicate plugin instances, bypass/unbypass, stop/start transport, and change sample rate/buffer size.
- Profile CPU/deadlines with multiple instances at small host buffers; three room engines run continuously. No universal CPU budget has been established by the numerical tests.
- Run an independent VST3 validator/pluginval, check long sessions and review licensing before distributing a release.

No listening session, DAW compatibility certification, independent VST3 validation or signed production release is claimed by this milestone. The local Cloud machine lacks JUCE GUI/audio development packages and a Windows runtime; plugin-specific verification therefore runs on the Windows CI runner.
