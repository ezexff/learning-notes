#include <windows.h>
#include <gl/gl.h>

#include "engine_platform.h"

#include "immintrin.h"
#include "engine_intrinsics.h"
#include "engine_math.h"

#include "engine_memory.h"
#include "engine_asset.h"

#include "engine_renderer.h"

struct game_state
{
    b32 IsInitialized;
    memory_arena ConstArena;
    
    s32 ToneHz;
    s16 ToneVolume;
    r32 tSine;
    u32 SampleIndex;
    
    loaded_sound LoadedSound;
    
    v4 ClearColor;
};

struct tran_state
{
    b32 IsInitialized;
    memory_arena TranArena;
};