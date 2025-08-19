#define PushRenderElement(Frame, type) (type *)PushRenderElement_(Frame, sizeof(type), RenderGroupEntryType_##type)
void *
PushRenderElement_(renderer_frame *Frame, u32 Size, render_group_entry_type Type)
{
    void *Result = 0;
    
    Size += sizeof(render_group_entry_header);
    
    if((Frame->PushBufferSize + Size) < Frame->MaxPushBufferSize)
    {
        render_group_entry_header *Header = (render_group_entry_header *)(Frame->PushBufferBase + Frame->PushBufferSize);
        Header->Type = Type;
        Result = (u8 *)Header + sizeof(*Header);
        Frame->PushBufferSize += Size;
    }
    else
    {
        InvalidCodePath;
    }
    
    return(Result);
}

void
PushClear(renderer_frame *Frame, v4 Color)
{
    render_entry_clear *Entry = PushRenderElement(Frame, render_entry_clear);
    if(Entry)
    {
        Entry->Color = Color;
    }
}