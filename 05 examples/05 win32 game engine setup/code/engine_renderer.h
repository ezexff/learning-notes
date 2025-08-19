// NOTE(ezexff): Platform-independent renderer
enum render_group_entry_type
{
    RenderGroupEntryType_render_entry_bitmap,
    RenderGroupEntryType_render_entry_clear,
};

struct render_group_entry_header
{
    render_group_entry_type Type;
};

struct render_entry_clear
{
    v4 Color;
};

/*struct render_entry_texture
{
    u32 Texture; // OpenGl id
    v2 P;
    v2 Dim;
    b32 FlipVertically;
    r32 Repeat;
};*/