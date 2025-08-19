#include "engine.h"

#include "engine_renderer.cpp"

extern "C" UPDATE_AND_RENDER_FUNC(UpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if(!GameState->IsInitialized)
    {
        InitializeArena(&GameState->ConstArena, Memory->PermanentStorageSize - sizeof(game_state),
                        (u8 *)Memory->PermanentStorage + sizeof(game_state));
        memory_arena *ConstArena = &GameState->ConstArena;
        
        GameState->ToneHz = 512;
        GameState->ToneVolume = 3000;
        GameState->tSine = 0;
        GameState->SampleIndex = 0;
        
        GameState->LoadedSound = LoadFirstWAV(&GameState->ConstArena, &Memory->PlatformAPI);
        
        GameState->ClearColor = {1.0f, 1.0f, 0.0f, 0.0f};
        
        GameState->IsInitialized = true;
    }
    
    Assert(sizeof(tran_state) <= Memory->TransientStorageSize);
    tran_state *TranState = (tran_state *)Memory->TransientStorage;
    if(!TranState->IsInitialized)
    {
        InitializeArena(&TranState->TranArena, Memory->TransientStorageSize - sizeof(tran_state), (u8 *)Memory->TransientStorage + sizeof(tran_state));
        memory_arena *TranArena = &TranState->TranArena;
        
        TranState->IsInitialized = true;
    }
    
    PushClear(&Memory->Frame, GameState->ClearColor);
    
    // NOTE(ezexff): ImGui game window
    {
#if ENGINE_INTERNAL
        imgui *ImGuiHandle = &Memory->Frame.ImGuiHandle;
        ImGui::SetCurrentContext(ImGuiHandle->Context);
        ImGui::SetAllocatorFunctions(ImGuiHandle->AllocFunc, ImGuiHandle->FreeFunc, ImGuiHandle->UserData);
        
        if(ImGuiHandle->ShowImGuiWindows && ImGuiHandle->ShowGameWindow)
        {
            ImGui::Begin("Game");
            ImGui::Text("Debug window for game layer...");
            
            if(ImGui::CollapsingHeader("Memory"))
            {
                ImGui::SeparatorText("ConstArena");
                ImGui::Text("Size = %d MB or %d KB or %d bytes",
                            GameState->ConstArena.Size / Megabytes(1),
                            GameState->ConstArena.Size / Kilobytes(1),
                            GameState->ConstArena.Size);
                ImGui::Text("Used = %d MB or %d KB or %d bytes",
                            GameState->ConstArena.Used / Megabytes(1),
                            GameState->ConstArena.Used / Kilobytes(1),
                            GameState->ConstArena.Used);
                ImGui::Spacing();
                ImGui::SeparatorText("TranArena");
                ImGui::Text("Size = %d MB or %d KB or %d bytes",
                            TranState->TranArena.Size / Megabytes(1),
                            TranState->TranArena.Size / Kilobytes(1),
                            TranState->TranArena.Size);
                ImGui::Text("Used = %d MB or %d KB or %d bytes",
                            TranState->TranArena.Used / Megabytes(1),
                            TranState->TranArena.Used / Kilobytes(1),
                            TranState->TranArena.Used);
                ImGui::Text("TempCount = %d", TranState->TranArena.TempCount);
            }
            
            if(ImGui::CollapsingHeader("Audio"))
            {
                ImGui::Text("Sine wave mixer");
                ImGui::SliderInt("tone hz", &GameState->ToneHz, 20, 20000);
                int ToneVolume = (int)GameState->ToneVolume;
                ImGui::SliderInt("tone volume", &ToneVolume, 1, 20000);
                GameState->ToneVolume = (s16)ToneVolume;
                
                if (ImGui::Button("Play loaded sound from start"))
                {
                    GameState->SampleIndex = 0;
                }
            }
            
            if(ImGui::CollapsingHeader("Render"))
            {
                ImGui::Text("Background");
                ImGui::ColorEdit4("clear color", (float*)&GameState->ClearColor);
            }
            ImGui::End();
        }
#endif
    }
}

extern "C" GET_SOUND_SAMPLES_FUNC(GetSoundSamples)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    tran_state *TranState = (tran_state *)Memory->TransientStorage;
    
    loaded_sound *LoadedSound = &GameState->LoadedSound;
    u32 SampleCount = LoadedSound->SampleCount;
    u32 ChannelCount = LoadedSound->ChannelCount;
    
    if(GameState->SampleIndex < LoadedSound->SampleCount)
    {
        s16 *SampleOut = SoundBuffer->Samples;
        for(int SampleIndex = 0;
            SampleIndex < SoundBuffer->SampleCount;
            ++SampleIndex)
        {
            u32 SoundSampleIndex = (GameState->SampleIndex + SampleIndex) % LoadedSound->SampleCount;
            s16 SampleValue = LoadedSound->Samples[0][SoundSampleIndex];
            *SampleOut++ = SampleValue;
            *SampleOut++ = SampleValue;
        }
        
        GameState->SampleIndex += SoundBuffer->SampleCount;
    }
    else
    {
        int WavePeriod = SoundBuffer->SamplesPerSecond / GameState->ToneHz;
        
        s16 *SampleOut = SoundBuffer->Samples;
        for(int SampleIndex = 0;
            SampleIndex < SoundBuffer->SampleCount;
            ++SampleIndex)
        {
            r32 SineValue = Sin(GameState->tSine);
            s16 SampleValue = (s16)(SineValue * GameState->ToneVolume);
            *SampleOut++ = SampleValue;
            *SampleOut++ = SampleValue;
            
            GameState->tSine += 2.0f * Pi32 * 1.0f / (r32)WavePeriod;
            if(GameState->tSine > 2.0f * Pi32)
            {
                GameState->tSine -= 2.0f * Pi32;
            }
        }
    }
}