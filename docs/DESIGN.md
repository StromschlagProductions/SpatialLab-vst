# DSP and host contract — static send effect

## Exactly three controls

SPATIAL LAB is a static, preset-driven room reverb. Select a room, set the DAW send level, and use Size/Tone only when an adjustment is needed. The editor and host parameter list contain exactly these controls:

| ID | UI name | Range | Default |
| --- | --- | --- | --- |
| `room` | Raum | Living Room, Kitchen, Bathroom (indices 0–2) | Living Room |
| `size` | Größe | 0 smaller, 0.5 neutral, 1 larger | 0.5 |
| `tone` | Klang | 0 darker, 0.5 neutral, 1 brighter | 0.5 |

Changing the room resets Size and Tone to 0.5, through both the editor and host automation, even with the editor closed. Writing the same room again is not a change and does not erase macro automation. Macro automation after a room event takes effect in host delivery order. Parameters are read once per callback, with 50 ms sample ramps for macro-driven coefficients and voicing; this is block-accurate, not sample-offset VST3 automation. There is no cross-room transition engine, distance model, movement simulation or oversampling. Outdoor presets are a future extension; the first milestone contains the three named indoor rooms.

Early reflections, late reverb, pre-delay/delay times, diffusion, spectral damping, stereo image and level calibration are internal preset design values, never individual controls. No Dry/Wet, Output Gain, Early/Late, Decay, Damping, Width or hidden automatable expert parameters are exposed. The output is always 100% wet. Direct signal and send level belong to the DAW. The editor shows a neutral midpoint and supports double-click reset for Size/Tone.

## Signal path and preset voicing

Mono input is duplicated before room processing. Mono output averages the internal stereo wet result. Stereo input channels excite the room independently. Supported layouts are mono → mono, mono → stereo and stereo → stereo; surround and stereo → mono are rejected.

Each preset has six signed early taps with distinct left/right arrivals and an eight-line damped feedback delay network. A Householder matrix scatters delayed signals before per-line attenuation `10^(-3 * delaySeconds / RT60)`. Delay times are fixed per preset and scaled to the actual sample rate during prepare. A one-pole low-pass per line models frequency-dependent decay; all loop gains remain below unity.

Size maps to nominal RT60 multiplier `2^(2*(size-0.5))` (0.5–2). It also changes the internal early/late voicing: early gain `1.2-0.4*size`, late gain `0.65+0.7*size`. Both are unity at neutral. This conveys a smaller/larger room impression without moving delay taps or adding a physical size/distance model. Tone maps preset damping cutoff to `presetCutoff * 2^(2*(tone-0.5))`, clamped below Nyquist. At neutral the preset's authored cutoff is used. The larger and brighter extremes are regression-tested for longer late energy and greater normalized high-frequency energy at both primary rates. These checks do not replace listening.

Only the selected room processes audio. All preset delay memories are allocated in advance. Selecting another room clears its previous tail, snaps to its current macro settings and starts that preset afresh; no old room tail is blended or revived. This intentional hard preset selection can interrupt sound if switched during playback. It is not designed for automated scene transitions; Locations handles those tasks.

## Real-time and parameter ownership

Vectors allocate only in prepare. Processing and room selection use preallocated buffers, bounded loops, lock-free atomic controls and coefficient ramps. No XML, filesystem, logging, locks, host notification, allocation or OS message posting occurs inside the room-reset callback or DSP path. Unchanged macro values reuse coefficients; ring-buffer wraps use conditional indexing rather than runtime division. Preset selection clears a bounded buffer, so its callback cost is higher than a steady block; CPU benchmarks report that separately.

JUCE owns three ranged parameters, rather than an APVTS with obsolete expert parameters. A choice-parameter callback resets the two macro atomics immediately when a different room is written. It increments a lock-free revision. The audio callback consumes that revision too, so even A→B→A between callbacks clears the selected tail. State restore also invalidates any live tail. A message-thread timer reports the **current** macro values to host listeners later; it never writes queued reset values over newer automation. The custom editor polls actual values as well as using JUCE slider attachments for host gestures. No editor or message dispatch is required for the DSP reset itself.

`processBlock` follows the actual host buffer size, including zero and oversized callbacks. JUCE disables denormals during processing. Reset clears memory without resizing. State operations and editor/timer work belong outside the audio callback. The usual host lifecycle must synchronize prepare/reset/state restore with processing.

## State and automation migration

Binary XML root `SpatialLabState`, `schemaVersion=3`, stores room index and Size/Tone values. Restore room first, then the saved macro offsets so session recall preserves adjustments. Invalid roots, unknown versions, out-of-range and nonfinite macro values are rejected. Schema 1/2 prototype states restore only the selected preset, with neutral macros; obsolete mix/gain/early-late/width/decay/damping settings are discarded. Old expert automation IDs are removed, not reused for new meanings; remove those obsolete DAW lanes. Parameter version hints are 1.

Session state stores controls, not live tails. Plugin manufacturer `StPr`, plugin code `SpLb`, bundle ID `com.stromschlagproductions.spatiallab` remain stable. Do not change room indices or current parameter IDs/ranges after distribution without migration.

## Rates, bypass and limits

48 kHz audio drama and 44.1 kHz music receive full regression and CPU coverage. Higher rates (96/192 kHz) get short correctness smoke tests only. No resampling/oversampling is required by this linear design.

The plugin bypass callback advances the engine and mutes the send. A DAW may implement bypass by routing around the plugin; such external routing is outside the callback's control. Mute the return in that case.

The static delays can color sustained tones. Presets are designed acoustic impressions, not measured rooms; listening, host validation and CPU deadline profiling remain required before release. No limiter, physical scene model, measured IR, custom skins or outdoor presets are included in this milestone. The reported host tail is conservatively 6 seconds, not a hard internal truncation; latency is zero.
