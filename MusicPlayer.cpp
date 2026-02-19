#include "MusicPlayer.h"

MusicPlayer::MusicPlayer()
{
}

MusicPlayer::~MusicPlayer()
{
    if (handle != -1)
    {
        DeleteSoundMem(handle);
    }
}

bool MusicPlayer::Load(const std::string& path)
{
    if (handle != -1)
    {
        DeleteSoundMem(handle);
        handle = -1;
    }

    handle = LoadSoundMem(path.c_str());
    return (handle != -1);
}

void MusicPlayer::Play(bool loop)
{
    if (handle == -1) return;

    int playType = loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
    PlaySoundMem(handle, playType);
}

void MusicPlayer::Stop()
{
    if (handle != -1)
    {
        StopSoundMem(handle);
    }
}

bool MusicPlayer::IsPlaying() const
{
    if (handle == -1) return false;
    return (CheckSoundMem(handle) == 1);
}

double MusicPlayer::GetTime() const
{
    if (handle == -1) return 0.0;

    // DXLib ÇÕÉ~ÉäïbÇ≈ï‘Ç∑ Å® ïbÇ…ïœä∑
    return GetSoundCurrentTime(handle) / 1000.0;
}