#pragma once

#include <unordered_map>
#include "../System/Log.h"

struct MIX_Mixer;
struct MIX_Track;

namespace BlackBoxEngine
{
    class AudioTrack
    {
        uint64_t m_id = 0;
        MIX_Mixer* m_pAudioMixer = nullptr;
        MIX_Track* m_pTrack = nullptr;

    public:
        AudioTrack( uint64_t id , MIX_Mixer* pAudioMixer );
        ~AudioTrack();

        uint64_t GetId() const;
        void SetAudioFile( const char* pFilepath );
        void Play(
            int64_t loopCount = 0,
            int64_t fadeInMiliseconds = 0,
            int64_t startMiliseconds = 0
            );
        void Stop( int64_t fadeOutFrames = 0);
        void SetVolume( float newVolume );

        int64_t RemainingMiliseconds();

        void Resume();
        void Pause();
    };

    class AudioManager
    {
        MIX_Mixer* m_pAudioMixer = nullptr;

        // sound effect map
        uint64_t m_nextId = 0;
        std::unordered_map< uint64_t, AudioTrack> m_audioMap;
        
        // music track
        AudioTrack* m_pMusicTrack = nullptr;
    public:
        AudioManager();
        ~AudioManager();
        
        bool PlaySound(const char* pFilePath , float volumeOverride = -1, uint64_t milisecondLengthOverride = 0);

        AudioTrack* AddAudioTrack( const char* pAudioFile );
        AudioTrack* AddAudioTrack();
        bool RemoveAudioTrack( AudioTrack* pAudioTrack );

        AudioTrack* GetMusicTrack();
        void RemoveMusicTrack();
        AudioTrack* SetMusicTrack( const char* pAudioFile );

        void Update();
    };
}