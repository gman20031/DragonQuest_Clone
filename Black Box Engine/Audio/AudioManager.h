#pragma once


namespace BlackBoxEngine
{
    class AudioManager
    {
    public:
        AudioManager();

        void StopAllSounds();

        void SetMusicTrack();
        void StopMusic();

        void PlaySoundEffect();

        void Update();
    };
}