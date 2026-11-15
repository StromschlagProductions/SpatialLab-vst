# Validation record

Date: 2026-11-15. Development branch: `develop/juce8-room-foundation`.

## Automated coverage

- Native GCC 14.2 / C++17 release build and CTest: passed locally.
- Native debug build with AddressSanitizer and UndefinedBehaviorSanitizer: passed locally.
- DSP tests run at 22.05, 44.1, 48, 96 and 192 kHz. They verify exact dry stereo routing, distinct nonzero room responses, no direct impulse at 100% wet, stereo diffusion from mono input, finite bounded impulse responses, tail decay, width-zero fold-down, reset-to-silence, and repeated extreme parameter changes.
- A test allocation hook measures zero ordinary heap allocations during DSP processing and target updates. It does not instrument the host or OS and is not a proof of hard real-time scheduling.
- Repeated target refresh at different block boundaries produces identical sample results; unchanged targets do not restart ramps.
- Windows CI builds the actual JUCE VST3 and processor tests. The processor suite covers all supported layouts, rejected stereo-to-mono/surround layouts, dry mono expansion, actual callback sizes 0/1/7/64/511/2048, sample-rate reprepare, all parameter values on session recall, invalid binary state and future state-schema rejection.
- GitHub Actions runs the same standalone DSP suite on Linux with ASan/UBSan and retains a Windows VST3 bundle on successful completion. Consult the linked PR checks for the exact tested SHA; a green DSP-only job does not establish that the plugin compiled.

## Before release: manual checks still required

- Load/rescan the complete Windows VST3 bundle in at least two DAWs; verify mono/stereo track and send-bus operation and the generic editor.
- Listen to dialogue, impulses, sustained tones and transient-heavy material in all three rooms. Evaluate coloration, tail density, stereo image and mono fold-down.
- Write and replay automation for every control, especially repeated room switches and dry/wet endpoints. Listen for clicks and unexpected gain changes.
- Save/reopen projects, duplicate plugin instances, bypass/unbypass, stop/start transport, and change sample rate/buffer size.
- Profile CPU/deadlines with multiple instances at small host buffers; three room engines run continuously. No universal CPU budget has been established by the numerical tests.
- Run an independent VST3 validator/pluginval, check long sessions and review licensing before distributing a release.

No listening session, DAW compatibility certification, independent VST3 validation or signed production release is claimed by this milestone. The local Cloud machine lacks JUCE GUI/audio development packages and a Windows runtime; plugin-specific verification therefore runs on the Windows CI runner.
