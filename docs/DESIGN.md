# DSP and host contract

## Signal path

Each channel retains its independent dry input. Mono input is duplicated before room processing. Mono output averages the internal stereo result; stereo-to-mono and surround bus configurations are deliberately rejected. Stereo dry input is never summed before mixing.

Each room has six signed early taps, different left/right arrival times and a late eight-line feedback delay network. Delay times are fixed for each room and scaled to the current sample rate during prepare. A Householder matrix scatters delayed signals; its orthogonality preserves energy before per-line attenuation. Feedback is `10^(-3 * delaySeconds / RT60)`, with a one-pole low-pass in every line. Maximum requested nominal RT60 is 2.7 s, and all feedback gains remain below unity. Damping makes high-frequency decay shorter than the nominal low-frequency RT60. The design is static and deterministic; no modulation, oversampling or measured impulse response is used.

Three rooms run continuously and their wet outputs crossfade over 50 ms. This keeps new room tails available without clearing buffers, reallocating, or changing delay lengths. It also costs three room engines at all times, including when dry. Mix, width, output gain, damping pole and feedback gains have 50 ms sample ramps. Linear dry/wet mixing preserves exact unity dry at mix = 0 and an isolated wet return at mix = 1. Output gain is smoothed in the linear domain. Parameters are read once per host callback; this initial implementation is block-accurate automation followed by per-sample smoothing, not sample-offset VST3 automation.

## Real-time ownership

All vectors allocate in `prepareToPlay`, which is the only place that sets sample-rate-dependent buffer sizes. `processBlock` reads cached lock-free atomic parameters and updates bounded ramps and preallocated delay buffers. No state trees, XML, filesystem, locks, logging or heap allocation occur in that path. Processing iterates the actual host buffer size and needs no scratch allocation even when a callback exceeds the advertised block size. JUCE disables denormals during processing. Reset clears delay memory without resizing it and must run under the host's normal processing lifecycle synchronization.

State serialization uses APVTS outside the audio callback. The root is `SpatialLabState`, with `schemaVersion=1`. Unknown versions and invalid binary/XML are ignored. Session recall stores controls, not the current tail. It does not implement live tail transfer, programs or factory-preset files. The Room parameter selects a DSP profile; it does not overwrite the other automated controls.

## Stable automation ABI

All IDs have JUCE parameter version hint 1. Do not rename IDs, reorder choices, change ranges or change normalization without a migration plan.

| ID | Host name | Range / choices | Default |
| --- | --- | --- | --- |
| `room` | Room | Living Room, Kitchen, Bathroom (indices 0–2) | Living Room |
| `mix` | Dry / Wet | 0–1 | 0.25 |
| `decay` | Decay multiplier | 0.5–2 | 1 |
| `damping` | Damping | 0–1 | 0.5 |
| `width` | Stereo width | 0–1 | 1 |
| `output` | Output (dB) | -24–6 | 0 |

Plugin manufacturer code `StPr`, plugin code `SpLb`, bundle ID `com.stromschlagproductions.spatiallab`. These determine host identity and must remain stable after distribution.

## Known limits

Static delays can produce coloration on sustained tones; listening and spectral evaluation remain necessary before a release. No low-cut, source placement, pre-delay control, modulation or limiter is present. A nominal RT60 is an attenuation target, not a measured room specification. A conservative six-second host tail is not a hard truncation: internal feedback continues as long as the host processes. Factory skins and production artwork are intentionally outside this first audio milestone.
