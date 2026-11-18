# Reproduzierbarer Vorher-/Nachher-Vergleich

48 kHz, SIZE/TONE 50 %, Send −6 dB, FX 0 dB. Identische unveränderte Mono-Quelle: SHA-256 `39f0a865bd8c54ced630340b32cc3ca42c3333d3f916a478612ef1e0a36ae0bc`. RMS-Fenster: 0 bis Quellenende, mittlere Kanalenergie. Keine Normalisierung.

| Umgebung (Key) | Wet/Quelle vorher dB | nachher dB | Änderung dB | Monoverlust vorher dB | nachher dB | Ankunft vorher ms | nachher ms | Wet-Peak nachher dBFS | Dry+Wet-Peak bei Send 0 dB* |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| attic | -12.62 | -11.23 | +1.39 | -3.02 | -1.12 | 3.10 | 4.21 | -12.51 | -2.44 |
| bathroom | -11.55 | -6.94 | +4.61 | -3.05 | -1.48 | 1.40 | 2.92 | -9.13 | -0.52 |
| bedroom | -12.59 | -12.77 | -0.18 | -3.02 | -1.32 | 3.71 | 3.15 | -15.90 | -1.66 |
| canyon | -19.52 | -19.70 | -0.18 | -2.07 | -0.16 | 310.00 | 25.21 | -21.67 | -3.89 |
| cathedral | -12.32 | -8.18 | +4.14 | -3.17 | -1.12 | 23.29 | 2.92 | -11.74 | -3.05 |
| cave | -8.26 | -11.17 | -2.91 | -3.03 | -1.41 | 17.29 | 3.81 | -12.71 | -3.47 |
| cellar | -8.34 | -5.89 | +2.45 | -3.03 | -1.09 | 4.90 | 2.02 | -9.99 | -2.62 |
| chapel | -11.91 | -10.82 | +1.08 | -3.11 | -1.28 | 12.71 | 4.10 | -11.13 | -2.20 |
| compact-car | -16.57 | -5.63 | +10.93 | -3.01 | -1.48 | 0.71 | 1.79 | -10.37 | -2.92 |
| courtyard | -20.41 | -14.07 | +6.34 | -1.72 | -0.30 | 31.00 | 4.67 | -16.39 | -3.03 |
| forest | -26.47 | -18.66 | +7.82 | -1.70 | -0.32 | 23.00 | 9.90 | -19.15 | -3.33 |
| hallway | -12.21 | -3.88 | +8.34 | -3.03 | -1.68 | 7.10 | 1.62 | -7.52 | -3.21 |
| kitchen | -6.04 | -6.95 | -0.92 | -3.04 | -1.15 | 2.29 | 3.40 | -9.33 | 0.27 |
| large-car | -15.16 | -7.24 | +7.92 | -3.23 | -1.61 | 0.90 | 1.92 | -10.12 | -2.77 |
| living | -10.86 | -8.70 | +2.16 | -3.02 | -1.20 | 5.29 | 3.27 | -9.25 | -3.61 |
| open-land | -44.39 | -37.66 | +6.73 | -1.67 | -0.02 | 190.00 | 4.67 | -40.52 | -3.96 |
| snowspeeder | -16.20 | -3.11 | +13.09 | -3.00 | -1.41 | 0.40 | 1.46 | -6.80 | -2.38 |
| stairwell | -9.88 | -7.16 | +2.72 | -3.06 | -1.60 | 11.29 | 4.31 | -7.18 | 2.08 |
| street | -19.40 | -13.03 | +6.37 | -2.40 | -0.21 | 19.00 | 4.67 | -13.99 | -1.08 |
| truck-cab | -13.87 | -6.06 | +7.81 | -3.27 | -1.46 | 1.10 | 2.31 | -10.44 | -3.41 |
| tunnel | -20.38 | -9.51 | +10.87 | -1.30 | -0.27 | 27.00 | 4.67 | -11.50 | -0.49 |
| underpass | -20.53 | -12.37 | +8.16 | -0.78 | -0.13 | 7.00 | 4.67 | -14.58 | -1.32 |
| van | -6.40 | -6.08 | +0.32 | -3.04 | -1.34 | 1.29 | 2.00 | -8.43 | -0.78 |
| workshop | -8.28 | -4.68 | +3.61 | -3.04 | -0.93 | 4.10 | 3.98 | -7.97 | 1.09 |
| x-wing | -13.18 | -4.80 | +8.38 | -3.04 | -1.38 | 0.60 | 1.46 | -8.27 | -3.11 |
| y-wing | -9.31 | -4.91 | +4.41 | -3.24 | -1.56 | 0.79 | 2.08 | -8.85 | -2.24 |

*Send-0-Peaks sind aus dem linearen Wet-Signal durch Rücknahme der bekannten −6-dB-Eingangsskalierung berechnet. Sie sind keine zusätzliche Lautheitsangleichung. Positive Summenpeaks sind möglich: Bei Send 0 dB benötigt die DAW-Summe gegebenenfalls Headroom; der Effekt enthält keinen Limiter.

C50 und EDC beziehen sich auf Wet ohne Direktsignal. Ein stufiger Outdoor-Decay rechtfertigt keine einzelne universelle RT60-Zahl. Die Messungen sind keine Hörfreigabe und kein STI-/Sprachverständlichkeitsnachweis.
