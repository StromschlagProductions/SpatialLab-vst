# Validation record

Date: 2026-11-15. Branch: `develop/juce8-room-foundation`. Consult PR #1 checks for the exact tested SHA.

## Primary rates: 44.1 and 48 kHz

The complete standalone DSP suite runs at both primary rates, checking:

- Distinct nonzero room responses, no direct impulse, stereo diffusion from mono, finite bounded impulses and tail decay.
- Internal early and late fields remain present, and neutral macros retain the authored preset voicing.
- Larger Size increases late-tail energy; brighter Tone increases normalized high-frequency (first-difference) tail energy, for every room.
- Reset and hard room selection clear tails without blending or reviving an old room.
- Rapid macro automation and repeated room selection stay bounded and perform zero measured ordinary heap allocations.
- Repeated unchanged targets at different block boundaries produce identical sample smoothing.

The JUCE processor suite checks exactly three exposed parameters; all supported mono/stereo bus layouts and rejected unsupported layouts; wet routing against the independent engine; callback sizes 0/1/7/64/511/2048; both primary rates and all rooms; send-bypass silence; immediate neutral macro reset on a host-thread room change with no open editor; unchanged-room writes preserving automation; session recall of adjusted macros; schema-1/2 migration to neutral presets; invalid/future state rejection; and editor creation/layout.

96/192 kHz get short finite-output/delay smoke tests, not full tuning or CPU coverage. No oversampling is used.

Local native Release/CTest runs use GCC 14.2/C++17. Local ASan/UBSan runs need `ASAN_OPTIONS=detect_leaks=0` because LeakSanitizer cannot inspect threads inside the Cloud sandbox (ptrace restriction). Linux CI retains the full ASan/UBSan and leak configuration. This is an environment limit, not a disabled CI check.

The allocation hook instruments ordinary C++ heap allocations inside standalone DSP, not the host/OS. Neither it nor numerical tests prove hard real-time scheduling or audible quality.

## CPU observations (informational)

`SpatialLabDspBenchmark` measures 44.1/48 kHz, mono/stereo, four seconds per case: steady controls, macro automation every 64 frames, and preset changes every 2048 frames. One selected preset processes audio; all memories are preallocated. It reports ns/frame and average percent of one core. Windows CI uploads `dsp-benchmark.csv` with the VST3 bundle and provenance. No machine-dependent timing failure threshold is used.

After the static single-active-room revision, local GCC Release samples measured roughly **34–40 ns/frame** (about **0.15–0.20% of one core**) in these short scenarios. The superseded three-engine prototype measured roughly 94–124 ns/frame on the same Cloud machine. These observations include noise and architecture/voicing changes; they do not measure DAW scheduling deadlines, editor/host overhead or the worst individual preset-switch callback.

## Before release: still required

- Rescan/load the whole Windows VST3 bundle in at least two DAWs; check return-bus mono/stereo behavior, the three-control editor and its neutral midpoint/reset behavior.
- Listen to dialogue at 48 kHz and music at 44.1 kHz, plus impulses, sustained tones and transients in every room. Evaluate coloration, density, naturalness, stereo image and mono fold-down.
- Record/play back Size/Tone automation. Select presets from both the editor and host automation with the editor closed; verify neutral reset and later macro automation. Confirm scene transitions are delegated to Locations rather than expecting a room crossfade.
- Save/reopen adjusted sessions, duplicate instances, bypass/unbypass, stop/start, change sample rate/buffer size. Note that the plugin mutes its bypass callback, while some DAWs route around it externally.
- Profile deadlines with many instances at small buffers, including the bounded memory clear on preset selection; average microbenchmark CPU is not a deadline guarantee.
- Run an independent VST3 validator/pluginval and long sessions; review distribution licensing.

No listening session, DAW certification, independent VST3 validation or signed production release is claimed by this milestone.
