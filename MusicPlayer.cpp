// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// MusicPlayer.cpp
// 楽曲の読み込み・再生・停止・状態取得を行うサウンド管理クラスです。
// 
//========================================

#include "MusicPlayer.h"

// @brief コンストラクタ
MusicPlayer::MusicPlayer()
{
}

// @brief デストラクタ（読み込んだサウンドを解放）
MusicPlayer::~MusicPlayer()
{
    // サウンドが読み込まれているか確認
    if (handle != -1)
    {
        // サウンドハンドルを解放
        DeleteSoundMem(handle);
    }
}

// @brief 音声ファイルを読み込みます
// @param path 読み込む音声ファイルのパス
// @return 読み込みに成功したかどうか
bool MusicPlayer::Load(const std::string& path)
{
    // 既に読み込まれている場合は解放
    if (handle != -1)
    {
        DeleteSoundMem(handle);
        handle = -1;
    }

    handle = LoadSoundMem(path.c_str()); // 新しい音声を読み込む
    return (handle != -1);               // 成功したかどうか返す
}

// @brief 音声を再生します
// @param loop true の場合ループ再生、false の場合は一度だけ再生
void MusicPlayer::Play(bool loop)
{
    // 読み込み失敗時は何もしない
    if (handle == -1) return;

    int playType = loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK; // 再生モード選択
    PlaySoundMem(handle, playType);      // 再生開始
}

// @brief 再生中の音声を停止します
void MusicPlayer::Stop()
{
    // 読み込まれている場合のみ停止
    if (handle != -1)
    {
        StopSoundMem(handle);
    }
}

// @brief 現在音声が再生中かどうかを返します
// @return 再生中なら true、停止中なら false
bool MusicPlayer::IsPlaying() const
{
    // 読み込まれていない場合は false
    if (handle == -1) return false;
    // 再生中かどうか判定
    return (CheckSoundMem(handle) == 1);
}

// @brief 現在の再生時間を秒単位で取得します
// @return 再生時間（秒）
double MusicPlayer::GetTime() const
{
    // 読み込まれていない場合は 0 秒
    if (handle == -1) return 0.0;

    // DXLib はミリ秒で返すため、秒に変換して返す
    return GetSoundCurrentTime(handle) / 1000.0;
}
