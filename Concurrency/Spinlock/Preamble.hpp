
#if defined(__i386__) || defined(__x86_64__)
#    include <immintrin.h>
#    define CPU_PAUSE() _mm_pause()
#elif defined(__aarch64__) || defined(__arm__)
#    define CPU_PAUSE() asm volatile("yield")
#else
#    define CPU_PAUSE() static_cast<void>(0)  // No-op
#    warning "Unknown architecture, CPU_PAUSE is a no-op"
#endif
