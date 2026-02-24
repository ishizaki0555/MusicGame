// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// MusicPlayer.h
// 楽曲の読み込み・再生・停止・状態取得を行うサウンド管理クラスです。
// 
//========================================

#pragma once
#include <string>
#include <DxLib.h>

class MusicPlayer
{
public:
    // @brief コンストラクタ
    MusicPlayer();

    // @brief デストラクタ
    ~MusicPlayer();

    // @brief 音声ファイルを読み込みます
    // @param path 読み込む音声ファイルのパス
    // @return 読み込みに成功したかどうか
    bool Load(const std::string& path);

    // @brief 音声を再生します
    // @param loop true の場合ループ再生、false の場合は一度だけ再生
    void Play(bool loop = false);

    // @brief 再生中の音声を停止します
    void Stop();

    // @brief 現在音声が再生中かどうかを返します
    // @return 再生中なら true、停止中なら false
    bool IsPlaying() const;

    // @brief 現在の再生時間を秒単位で取得します
    // @return 再生時間（秒）
    double GetTime() const;

private:
    int handle = -1;   // DXLib のサウンドハンドル
};
