#pragma once
#include <string>

namespace Config
{
    // 画面解像度
    extern int screenWidth;
    extern int screenHeight;

    // ゲーム設定
    extern float noteSpeed;

    // 音量設定 (0.0f ~ 1.0f)
    extern float masterVolume;
    extern float bgmVolume;
    extern float seVolume;

    // 内部解像度（固定）
    constexpr int INTERNAL_WIDTH = 1280;
    constexpr int INTERNAL_HEIGHT = 720;

    // 設定のロード・セーブ
    void Load();
    void Save();

    // 現在再生中のBGMハンドル（リアルタイム音量反映用）
    extern int currentBgmHandle;

    // 音量変更の適用関数
    void ApplyVolume();
}
