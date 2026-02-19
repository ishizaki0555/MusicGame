#pragma once
#include <string>
#include <DxLib.h>

class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();

    // 音楽ファイルを読み込む
    bool Load(const std::string& path);

    // 再生
    void Play(bool loop = false);

    // 停止
    void Stop();

    // 再生中かどうか
    bool IsPlaying() const;

    // 現在の再生時間（秒）
    double GetTime() const;

private:
    int handle = -1;   // DXLib のサウンドハンドル
};