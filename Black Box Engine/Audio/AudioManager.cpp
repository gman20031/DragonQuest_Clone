#include "AudioManager.h"

#include <SDL3_mixer/SDL_mixer.h>
#include "../System/Delay.h"

namespace BlackBoxEngine
{
    AudioTrack::AudioTrack( uint64_t id, MIX_Mixer* pAudioMixer )
        : m_pAudioMixer(pAudioMixer)
        , m_id(id)
    {
        if ( !pAudioMixer )
        {
            BB_LOG( LogType::kWarning, "Mixer device passed in is nullptr, ", SDL_GetError() );
            return;
        }

        m_pTrack = MIX_CreateTrack( m_pAudioMixer );
        if ( !m_pTrack )
        {
            BB_LOG( LogType::kWarning, "Track failed to generate, ", SDL_GetError() );
            return;
        }
    }

    AudioTrack::~AudioTrack()
    {
        MIX_DestroyTrack( m_pTrack );
    }

    uint64_t AudioTrack::GetId() const
    {
        return m_id;
    }

    void AudioTrack::SetAudioFile( const char* pFilepath )
    {
        MIX_Audio* pAudioFile = MIX_LoadAudio( m_pAudioMixer, pFilepath, true );
        if ( !pAudioFile )
        {
            BB_LOG( LogType::kWarning, "Audio file failed to load, ", SDL_GetError() );
            return;
        }

        if ( !MIX_SetTrackAudio( m_pTrack, pAudioFile ) )
        {
            BB_LOG( LogType::kWarning, "Audio track was not set properly , ", SDL_GetError() );
            return;
        }
    }

    void AudioTrack::Play( 
        int64_t loopCount,
        int64_t fadeInMiliseconds,
        int64_t startMiliseconds
    )
    {
        SDL_PropertiesID id = MIX_GetTrackProperties(m_pTrack);
        if ( !SDL_SetNumberProperty( id, MIX_PROP_PLAY_LOOPS_NUMBER, loopCount ) )
            BB_LOG( LogType::kWarning, "error setting loop count ", SDL_GetError() );
        if ( ! SDL_SetNumberProperty( id, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMiliseconds ) )
            BB_LOG( LogType::kWarning, "error setting fade in time ", SDL_GetError() );
        if ( ! SDL_SetNumberProperty( id, MIX_PROP_PLAY_START_MILLISECOND_NUMBER, startMiliseconds ) )
            BB_LOG( LogType::kWarning, "error setting start time ", SDL_GetError() );
        MIX_PlayTrack( m_pTrack, id );
    }

    void AudioTrack::Stop( int64_t fadeOutFrames )
    {
        MIX_StopTrack( m_pTrack, fadeOutFrames );
    }

    void AudioTrack::SetVolume( float newVolume )
    {
        MIX_SetTrackGain( m_pTrack, newVolume );
    }

    int64_t AudioTrack::RemainingMiliseconds()
    {
        return MIX_TrackFramesToMS( m_pTrack , MIX_GetTrackRemaining( m_pTrack ) );
    }

    void AudioTrack::Resume()
    {
        MIX_ResumeTrack( m_pTrack );
    }

    AudioManager::AudioManager()
    {
        SDL_Init( SDL_INIT_AUDIO );
        MIX_Init();
        
        m_pAudioMixer = MIX_CreateMixerDevice( SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr );

        if ( !m_pAudioMixer )
        {
            BB_LOG( LogType::kWarning, "Audio Mixer not created , ", SDL_GetError() );
            return;
        }

        m_pMusicTrack = new AudioTrack( 0, m_pAudioMixer );
    }

    AudioManager::~AudioManager()
    {
        MIX_DestroyMixer( m_pAudioMixer );
        MIX_StopAllTracks( m_pAudioMixer , 0);

        if ( m_pMusicTrack )
            delete m_pMusicTrack;

        MIX_Quit();
        SDL_QuitSubSystem( SDL_INIT_AUDIO );
    }

    bool AudioManager::PlaySound( const char* pFilePath , float volumeOverride, uint64_t milisecondLengthOverride )
    {
        AudioTrack* pSoundEffect = new AudioTrack( 0, m_pAudioMixer );
        pSoundEffect->SetAudioFile( pFilePath );
        pSoundEffect->Play();
        
        uint64_t milisecondCallback = pSoundEffect->RemainingMiliseconds();
        if ( milisecondLengthOverride > 0 )
            milisecondCallback = milisecondLengthOverride;
        if ( volumeOverride >= 0 )
            pSoundEffect->SetVolume( volumeOverride );

        DelayedCallbackManager::AddCallback( 
            [pSoundEffect ]()
            {
                pSoundEffect->Stop();
                delete pSoundEffect;
            }
            , std::chrono::milliseconds(milisecondCallback)
        );

        return true;
    }

    AudioTrack* AudioManager::SetMusicTrack( const char* pAudioFile )
    {
        m_pMusicTrack->SetAudioFile( pAudioFile );
        m_pMusicTrack->Play(-1);
        return m_pMusicTrack;
    }

    AudioTrack* AudioManager::GetMusicTrack()
    {
        if ( !m_pMusicTrack )
            BB_LOG( LogType::kWarning, "Attempted to get music track without first setting it" );
        return m_pMusicTrack;
    }

    void AudioManager::RemoveMusicTrack()
    {
        m_pMusicTrack->Stop( 0 );
    }

    AudioTrack* AudioManager::AddAudioTrack( const char* pAudioFile )
    {
        AudioTrack track( m_nextId, m_pAudioMixer );
        auto pair = m_audioMap.emplace( m_nextId, std::move( track ) );
        ++m_nextId;
        if ( !pair.second )
            BB_LOG( LogType::kWarning, "Emplacing new audio file failed" );
        else
            pair.first->second.SetAudioFile( pAudioFile );
        return &pair.first->second;
    }

    AudioTrack* AudioManager::AddAudioTrack()
    {
        AudioTrack track( m_nextId, m_pAudioMixer );
        auto pair = m_audioMap.emplace( m_nextId, std::move( track ) );
        ++m_nextId;
        if ( !pair.second )
            BB_LOG( LogType::kWarning, "Emplacing new audio file failed" );
        return &pair.first->second;
    }

    bool AudioManager::RemoveAudioTrack( AudioTrack* pAudioTrack )
    {
        auto it = m_audioMap.find( pAudioTrack->GetId() );
        if ( it == m_audioMap.end() )
        {
            return false;
        }
        m_audioMap.erase( it );
        return true;
    }

}
