#pragma once

#if defined(__GNUC__)
#define FINLINE                inline __attribute__((always_inline))
#define NOINLINE               __attribute__((noinline))
#define EXPECT_TAKEN(a)        __builtin_expect(!!(a), true)
#define EXPECT_NOT_TAKEN(a)    __builtin_expect(!!(a), false)
#elif defined(__MSVC__)
#define FINLINE                __forceinline
#define NOINLINE               __declspec(noinline)
#define MM_ALIGN16             __declspec(align(16))
#define EXPECT_TAKEN(a)        (a)
#define EXPECT_NOT_TAKEN(a)    (a)
#else
#error Unsupported compiler!
#endif