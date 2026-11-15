# DSP and host contract

## Signal path

The plugin is exclusively a send effect. No direct signal is mixed into the output at any parameter setting. Mono input is duplicated before room processing. Mono output averages the internal stereo wet result; stereo-to-mono and surround bus configurations are deliberately rejected. Stereo input channels excite the room independently.

`Room` returns separate early and late stereo fields. `Engine` crossfades rooms, then applies Early/Late Balance: `wet = 2 * ((1 - balance) * early + balance * late)`. At 0.5 the original `early + late` voicing is preserved; 0 isolates early reflections, 1 isolates the late field. The factors of two at endpoints are deliberate, not an equal-power or loudness-normalized blend. Balance is smoothed; changing it never clears the late tail, which continues evolving even when inaudible. Width operates on the combined wet field, followed by Wet Output Level.
Each room has six signed early taps, different left/right arrival times and a late eight-line feedback delay network. Delay times are fixed for each room and scaled to the current sample rate during prepare. A Householder matrix scatters delayed signals; its orthogonality preserves energy before per-line attenuation. Feedback is `10^(-3 * delaySeconds / RT60)`, with a one-pole low-pass in every line. Maximum requested nominal RT60 is 2.7 s, and all feedback gains remain below unity. Damping makes high-frequency decay shorter than the nominal low-frequency RT60. The design is static and deterministic; no modulation, oversampling or measured impulse response is used.

Three rooms run continuously and their wet outputs crossfade over 50 ms. This keeps new room tails available without clearing buffers, reallocating, or changing delay lengths. It also costs three room engines at all times. Early/Late balance, width, wet output gain, damping pole and feedback gains have 50 ms sample ramps. Wet output gain is smoothed in the linear domain. Parameters are read once per host callback; this initial implementation is block-accurate automation followed by per-sample smoothing, not sample-offset VST3 automation.

## Real-time ownership

All vectors allocate in `prepareToPlay`, which is the only place that sets sample-rate-dependent buffer sizes. `processBlock` reads cached lock-free atomic parameters and updates bounded ramps and preallocated delay buffers. No state trees, XML, filesystem, locks, logging or heap allocation occur in that path. Unchanged decay/damping values reuse their existing coefficients; exponentials are calculated only when those parameters change. Circular-buffer indices use conditional wraps instead of runtime integer division. Processing iterates the actual host buffer size and needs no scratch allocation even when a callback exceeds the advertised block size. JUCE disables denormals during processing. Reset clears delay memory without resizing it and must run under the host's normal processing lifecycle synchronization.

State serialization uses APVTS outside the audio callback. The root is `SpatialLabState`, with `schemaVersion=2`. Schema 1 is migrated: retain room/decay/damping/width/output, remove `mix`, and add `earlyLate=0.5`. Unknown versions and invalid binary/XML are ignored. Session recall stores controls, not the current tail. It does not implement live tail transfer, programs or factory-preset files. The Room parameter selects a DSP profile; it does not overwrite the other automated controls.

## Stable automation ABI

All current IDs have JUCE parameter version hint 1. The send-only revision intentionally removes the unreleased prototype `mix` ID and introduces `earlyLate` with a different ID, avoiding reinterpretation of old automation as early/late balance. Schema migration handles saved control state, not old `mix` automation lanes; those lanes must be removed in the DAW. The existing `output` ID/range is retained and its label now explicitly says Wet Output Level. Do not otherwise rename IDs, reorder choices, change ranges or change normalization without a migration plan.

| ID | Host name | Range / choices | Default |
| --- | --- | --- | --- |
| `room` | Room | Living Room, Kitchen, Bathroom (indices 0–2) | Living Room |
| `earlyLate` | Early / Late Balance | 0–1 (0 early, 1 late) | 0.5 |
| `decay` | Decay multiplier | 0.5–2 | 1 |
| `damping` | Damping | 0–1 | 0.5 |
| `width` | Stereo width | 0–1 | 1 |
| `output` | Wet Output Level (dB) | -24–6 | 0 |

Plugin manufacturer code `StPr`, plugin code `SpLb`, bundle ID `com.stromschlagproductions.spatiallab`. These determine host identity and must remain stable after distribution.

## Rate and CPU priorities

48 kHz (audio drama) and 44.1 kHz (music) receive the complete DSP/processor regression suite and mono/stereo CPU benchmarks with steady controls and automation at 64-frame boundaries. All delay times use the actual sample rate; no resampling or oversampling is present. Higher rates (96/192 kHz) receive short finite-output/delay smoke tests only. They must not drive default tuning, UI choices or optimizations at the expense of the primary rates.

The plugin's bypass callback advances the engine and clears the output. This intentionally mutes the send rather than returning dry audio. A DAW may implement bypass outside the plugin; that routing is beyond the callback's control.

## Known limits

Static delays can produce coloration on sustained tones; listening and spectral evaluation remain necessary before a release. No low-cut, source placement, pre-delay control, modulation or limiter is present. A nominal RT60 is an attenuation target, not a measured room specification. A conservative six-second host tail is not a hard truncation: internal feedback continues as long as the host processes. Factory skins and production artwork are intentionally outside this first audio milestone.
