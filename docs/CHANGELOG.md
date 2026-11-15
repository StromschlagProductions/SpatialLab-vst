# Changes

## Send-only revision — development

- Prioritized 48 kHz audio drama and 44.1 kHz music for full automated regression and CPU profiling; higher rates have short smoke coverage. No oversampling.
- Removed the direct path and Dry/Wet parameter. Output is always wet; renamed Output to Wet Output Level and added smoothed Early/Late Balance (neutral midpoint retains room voicing).
- Added schema-2 state with migration of schema-1 wet controls; obsolete mix is discarded rather than reinterpreted as a new automation ID.
- Plugin bypass callback now mutes the send output while advancing room state.
- Replaced runtime circular-buffer division and avoided coefficient recalculation when controls are unchanged.
- Added endpoint isolation, wet mono/stereo routing, legacy-state/bypass regression checks, and a primary-rate CPU benchmark captured in Windows CI artifacts.

## 0.1.0 — initial prototype (superseded where noted above)

- Added JUCE 8.0.6/CMake Windows x64 VST3 target with fixed plugin identity.
- Implemented Living Room, Kitchen and Bathroom: signed early reflections and damped eight-line feedback networks.
- Added independent stereo dry routing, mono expansion/fold-down and explicit supported bus layouts.
- Added six stable APVTS parameters, smooth control changes and versioned session state.
- Added a generic JUCE editor for the first functional milestone.
- Added native DSP tests, JUCE processor integration tests, Windows artifact CI and Linux sanitizer CI.
- Documented build/installation, parameter contract, DSP ownership and pre-release validation limits.
