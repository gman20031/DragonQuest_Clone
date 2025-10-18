#pragma once

#include "AudioManager.h"
#include "../Actors/Component.h"

namespace BlackBoxEngine
{
    class AudioPlayerComponent : public Component
    {
        GENERATE_ID( "AudioPlayerComponent" );

        const char* m_pFilePath = nullptr;

        AudioManager* m_pAudioManager = nullptr;
        AudioTrack* m_pAudioTrack = nullptr;

        float m_volume = 1.0f;

    public:
        AudioPlayerComponent( Actor* pActor );
    
        AudioTrack* GetAudioTrack();
        void SetVolume( float volume ) { m_volume = volume; };
        float GetVolume() const { return m_volume; }

        virtual void Save(XMLElementParser parser ) override;
        virtual void Load(const XMLElementParser parser ) override;
    };

    // The music player will set the music track that is run 
    class MusicPlayer : public Component
    {
        GENERATE_ID( "MusicPlayer" );

        bool m_playOnStart = true;
        float m_volume = 1.0f;
        const char* m_pFilePath = nullptr;

        AudioManager* m_pAudioManager = nullptr;

    public:
        MusicPlayer( Actor* pActor );

        void SetMusicTrack( const char* pMusicFilePath ) const;
        void SetVolume( float volume ) { m_volume = volume; }
        float GetVolume() const { return m_volume; }

        virtual void Start() override;

        virtual void Save( XMLElementParser parser ) override;
        virtual void Load( const XMLElementParser parser ) override;
    };

}


