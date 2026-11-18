#pragma once
#if defined(__SSE2__) || defined(_M_X64)
#include <xmmintrin.h>
#endif
// Match JUCE ScopedNoDenormals on the supported Windows/Linux x64 targets.
// This prevents inaudible denormal tails from biasing offline CPU measurements.
class AuditFpMode {
public:
 AuditFpMode() noexcept {
#if defined(__SSE2__) || defined(_M_X64)
  previous=_mm_getcsr();_mm_setcsr(previous|0x8040u);
#endif
 }
 ~AuditFpMode() noexcept {
#if defined(__SSE2__) || defined(_M_X64)
  _mm_setcsr(previous);
#endif
 }
 AuditFpMode(const AuditFpMode&)=delete;
 AuditFpMode& operator=(const AuditFpMode&)=delete;
private:
#if defined(__SSE2__) || defined(_M_X64)
 unsigned previous=0;
#endif
};
