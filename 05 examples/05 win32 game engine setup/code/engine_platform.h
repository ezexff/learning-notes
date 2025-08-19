#include "engine_types.h"

// NOTE(ezexff): ImGui
#if ENGINE_INTERNAL
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

struct imgui
{
    ImGuiContext *Context;
    ImGuiIO *IO;
    
    ImGuiMemAllocFunc AllocFunc;
    ImGuiMemFreeFunc FreeFunc;
    void *UserData;
    
    HDC WGL; // NOTE(ezexff): Need for render imgui
    
    // NOTE(ezexff): ImGui windows visibility
    bool ShowRendererWindow;
    bool ShowWin32Window;
    bool ShowGameWindow;
    bool ShowDemoWindow;
    
    // NOTE(ezexff): ImGui all windows visibility
    bool ShowImGuiWindows;
};
#endif

// NOTE(ezexff): Need it for reading files
inline u32
SafeTruncateUInt64(u64 Value)
{
    // TODO(casey): Defines for maximum values
    Assert(Value <= 0xFFFFFFFF);
    u32 Result = (u32)Value;
    return(Result);
}

// NOTE(ezexff): Sound
typedef struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    s16 *Samples;
} game_sound_output_buffer;

struct loaded_sound
{
    u32 SampleCount;
    u32 ChannelCount;
    s16 *Samples[2];
    
    void *Free;
};

// NOTE(ezexff): Read files
typedef struct platform_file_handle
{
    b32 NoErrors;
} platform_file_handle;

typedef struct platform_file_group
{
    u32 FileCount;
} platform_file_group;

#define PLATFORM_GET_ALL_FILE_OF_TYPE_BEGIN(name) platform_file_group *name(char *Type)
#define PLATFORM_GET_ALL_FILE_OF_TYPE_END(name) void name(platform_file_group *FileGroup)
#define PLATFORM_OPEN_FILE(name) platform_file_handle *name(platform_file_group *FileGroup)
#define PLATFORM_READ_DATA_FROM_FILE(name) void name(platform_file_handle *Source, u64 Offset, u64 Size, void *Dest)
#define PLATFORM_FILE_ERROR(name) void name(platform_file_handle *Handle, char *Message)
#define PlatformNoFileErrors(Handle) ((Handle)->NoErrors)

typedef PLATFORM_GET_ALL_FILE_OF_TYPE_BEGIN(platform_get_all_files_of_type_begin);
typedef PLATFORM_GET_ALL_FILE_OF_TYPE_END(platform_get_all_files_of_type_end);
typedef PLATFORM_OPEN_FILE(platform_open_next_file);
typedef PLATFORM_READ_DATA_FROM_FILE(platform_read_data_from_file);
typedef PLATFORM_FILE_ERROR(platform_file_error);

// NOTE(ezexff): Platform API
typedef struct platform_api
{
    platform_get_all_files_of_type_begin *GetAllFilesOfTypeBegin;
    platform_get_all_files_of_type_end *GetAllFilesOfTypeEnd;
    platform_open_next_file *OpenNextFile;
    platform_read_data_from_file *ReadDataFromFile;
    platform_file_error *FileError;
} platform_api;

// NOTE(ezexff): Renderer frame with push buffer
struct renderer_frame
{
    // NOTE(ezexff): Client render area
    s32 Width;
    s32 Height;
    
    u8 PushBufferMemory[65536];
    
    u32 MaxPushBufferSize;
    u8 *PushBufferBase;
    u32 PushBufferSize;
    
    u32 MissingResourceCount;
    
#if ENGINE_INTERNAL
    // NOTE(ezexff): OpenGL info
    char *GLVendorStr;
    char *GLRendererStr;
    char *GLVersionStr;
    
    imgui ImGuiHandle;
#endif
};

// NOTE(ezexff): Platform-independent memory
struct game_memory
{
    u64 PermanentStorageSize;
    void *PermanentStorage; // NOTE(ezexff): Clear to zero at startup
    
    u64 TransientStorageSize;
    void *TransientStorage; // NOTE(ezexff): Clear to zero at startup
    
    renderer_frame Frame;
    
    platform_api PlatformAPI;
};

// NOTE(ezexff): Export/import engine functions
#define UPDATE_AND_RENDER_FUNC(name) void name(game_memory *Memory)
#define GET_SOUND_SAMPLES_FUNC(name) void name(game_memory *Memory, game_sound_output_buffer *SoundBuffer)

typedef UPDATE_AND_RENDER_FUNC(update_and_render);
typedef GET_SOUND_SAMPLES_FUNC(get_sound_samples);

// NOTE(ezexff): Export/import renderer functions
#define RENDERER_BEGIN_FRAME(name) void name(renderer_frame *Frame)
#define RENDERER_END_FRAME(name) void name(renderer_frame *Frame)

typedef RENDERER_BEGIN_FRAME(renderer_begin_frame);
typedef RENDERER_END_FRAME(renderer_end_frame);