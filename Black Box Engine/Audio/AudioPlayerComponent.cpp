#include "AudioPlayerComponent.h"

#include "../BlackBoxManager.h"

namespace BlackBoxEngine
{
    AudioPlayerComponent::AudioPlayerComponent( Actor* pActor )
        : Component( pActor )
    { 
        m_pAudioManager = BlackBoxManager::Get()->m_pAudioManager;
        m_pAudioTrack = m_pAudioManager->AddAudioTrack();
    }

    AudioTrack* AudioPlayerComponent::GetAudioTrack()
    {
        return m_pAudioTrack;
    }
    
    void AudioPlayerComponent::Save( XMLElementParser parser )
    {
        parser.NewChildVariable( "audioFile", m_pFilePath );
    }

    void AudioPlayerComponent::Load( const XMLElementParser parser )
    {
        parser.GetChildVariable( "audioFile", &m_pFilePath );
        m_pAudioTrack->SetAudioFile( m_pFilePath );
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // Music Player
    ////////////////////////////////////////////////////////////////////////////////////////

    MusicPlayer::MusicPlayer( Actor* pActor )
        : Component(pActor)
    {
        m_pAudioManager = BlackBoxManager::Get()->m_pAudioManager;
    }

    void MusicPlayer::SetMusicTrack( const char* pMusicFilePath ) const
    {
        if ( !pMusicFilePath )
        {
            BB_LOG( LogType::kWarning, "Attempted to set music track with null filepath" );
            return;
        }

        m_pAudioManager->SetMusicTrack( pMusicFilePath );
    }

    void MusicPlayer::Save( XMLElementParser parser )
    {
        parser.NewChildVariable( "RunOnStart", m_playOnStart );
        parser.NewChildVariable( "audioFile", m_pFilePath );
    }

    void MusicPlayer::Load( const XMLElementParser parser )
    {
        parser.GetChildVariable( "RunOnStart", &m_playOnStart );
        parser.GetChildVariable( "audioFile", &m_pFilePath );
    }

}


