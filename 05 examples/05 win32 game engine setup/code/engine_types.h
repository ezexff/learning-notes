#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 b32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef intptr_t intptr;
typedef uintptr_t uintptr;
typedef intptr_t smm;
typedef uintptr_t umm;

typedef size_t memory_index;

// NOTE(ezexff): Floating point fix for no CRT release compile (MSVC)
#if !(ENGINE_INTERNAL)
extern "C"
{
    int _fltused;
}
#pragma function(memset)
void *
memset(void *_Dst, int _Val, size_t _Size)
{
    unsigned char Val = *(unsigned char *)&_Val;
    unsigned char *At = (unsigned char *)_Dst;
    while(_Size--)
    {
        *At++ = Val;
    }
    return(_Dst);
}
#endif

typedef float r32;
typedef double r64;

#define flag8(type) u8
#define flag16(type) u16
#define flag32(type) u32
#define flag64(type) u64

#define enum8(type) u8
#define enum16(type) u16
#define enum32(type) u32
#define enum64(type) u64

#define U16Max 65535
#define S32Min ((s32)0x80000000)
#define S32Max ((s32)0x7fffffff)
#define U32Min 0
#define U32Max ((u32)-1)
#define U64Max ((u64)-1)
#define F32Max FLT_MAX
#define F32Min -FLT_MAX

#if !defined(internal)
#define internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Pi32_2 1.57079632679f // pi/2 TODO(me): remove?
#define Pi32_4 0.78539816340f // pi/4 TODO(me): remove?
#define Tau32 6.28318530717958647692f

#if ENGINE_INTERNAL
#define Assert(Expression)                                                                                             \
if(!(Expression))                                                                                                  \
{                                                                                                                  \
*(int *)0 = 0;                                                                                                 \
}
#else
#define Assert(Expression)
#endif

#define InvalidCodePath Assert(!"InvalidCodePath")
#define InvalidDefaultCase                                                                                             \
default: {                                                                                                         \
InvalidCodePath;                                                                                               \
}                                                                                                                  \
break

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
// TODO(me): swap, min, max (add if need)

#define AlignPow2(Value, Alignment) ((Value + ((Alignment)-1)) & ~((Alignment)-1))
#define Align4(Value) ((Value + 3) & ~3)
#define Align8(Value) ((Value + 7) & ~7)
#define Align16(Value) ((Value + 15) & ~15)