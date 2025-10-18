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
        parser.NewChildVariable( "AudioFile", m_pFilePath );
        parser.NewChildVariable( "Volume", m_volume );

    }

    void AudioPlayerComponent::Load( const XMLElementParser parser )
    {
        parser.GetChildVariable( "AudioFile", &m_pFilePath );
        parser.GetChildVariable( "Volume", &m_volume );
        m_pAudioTrack->SetAudioFile( m_pFilePath );
        m_pAudioTrack->SetVolume( m_volume );
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
        m_pAudioManager->GetMusicTrack()->SetVolume( m_volume );
    }

    void MusicPlayer::Start()
    {
        if ( m_playOnStart )
            SetMusicTrack( m_pFilePath );
    }

    void MusicPlayer::Save( XMLElementParser parser )
    {
        parser.NewChildVariable( "RunOnStart", m_playOnStart );
        parser.NewChildVariable( "Volume", m_volume );
        parser.NewChildVariable( "AudioFile", m_pFilePath );
    }

    void MusicPlayer::Load( const XMLElementParser parser )
    {
        parser.GetChildVariable( "RunOnStart", &m_playOnStart );
        parser.GetChildVariable( "Volume", &m_volume );
        parser.GetChildVariable( "AudioFile", &m_pFilePath );
    }

}


