void
OpenGLInit(renderer_frame *Frame)
{
    // NOTE(ezexff): Init push buffer
    Frame->MaxPushBufferSize = sizeof(Frame->PushBufferMemory);
    Frame->PushBufferBase = Frame->PushBufferMemory;
}

void
OpenGLBeginFrame(renderer_frame *Frame)
{
    glViewport(0, 0, Frame->Width, Frame->Height);
}

void
OpenGLEndFrame(renderer_frame *Frame)
{
    // NOTE(ezexff): Draw info from push buffer
    for(u32 BaseAddress = 0;
        BaseAddress < Frame->PushBufferSize;
        )
    {
        render_group_entry_header *Header = (render_group_entry_header *)
        (Frame->PushBufferBase + BaseAddress);
        BaseAddress += sizeof(*Header);
        
        void *Data = (u8 *)Header + sizeof(*Header);
        switch(Header->Type)
        {
            case RenderGroupEntryType_render_entry_clear:
            {
                render_entry_clear *Entry = (render_entry_clear *)Data;
                
                glClearColor(Entry->Color.r, Entry->Color.g, Entry->Color.b, Entry->Color.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                BaseAddress += sizeof(*Entry);
            }break;
            
            InvalidDefaultCase;
        }
    }
    
    // NOTE(ezexff): Red rectangle
    {
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        
        float OffsetFromCenter = 0.5f;
        glVertex2f(-OffsetFromCenter, -OffsetFromCenter);
        glVertex2f(OffsetFromCenter, -OffsetFromCenter);
        glVertex2f(OffsetFromCenter, OffsetFromCenter);
        glVertex2f(-OffsetFromCenter, OffsetFromCenter);
        
        glEnd();
    }
    
    // NOTE(ezexff): Clear push buffer
    while(Frame->PushBufferSize--)
    {
        Frame->PushBufferMemory[Frame->PushBufferSize] = 0;
    }
    Frame->PushBufferSize = 0;
}