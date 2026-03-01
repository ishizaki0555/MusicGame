#pragma once
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <DxLib.h>
#include "GameScene.h"
#include "JsonLoader.h"

namespace fs = std::filesystem;

enum class Difficulty
{
    Easy = 0,
    Normal,
    Hard,
    Extra
};

enum class ScrollState
{
    WaitStart,
    Scroll,
    WaitEnd
};

class MusicSelectUI
{
public:

    struct MusicInfo
    {
        std::string folder;
        std::string file;
        std::string banner;
        int bpm = 0;
        int offset = 0;

        std::string easyChart;
        std::string normalChart;
        std::string hardChart;
        std::string extraChart;

        std::string title;
        int bannerHandle = -1;
    };

    MusicSelectUI(const fs::path& folderPath);
    ~MusicSelectUI();

    void LoadMusicList();
    void LoadFont(const std::string& fontPath, int size);

    void Update();
    void Draw();

    GameScene* createdScene = nullptr;

private:
    void DrawBoxEx(int x, int y, int w, int h, int r, int g, int b, int a);
    void DrawTextEx(const std::string& text, int x, int y, int color, int font);
    void DrawTextCenter(const std::string& text, int centerX, int y, int color, int font);

    void DrawTextLeftClip(const std::string& text, int leftX, int y, int barWidth, int color, int font);
    void DrawTextLeftSlide(const std::string& text, int leftX, int y, int barWidth, int color, int font);

    void DrawBanner(const MusicInfo& info);
    static void GetDifficultyColor(Difficulty diff, int& r, int& g, int& b);

private:
    fs::path folderPath;
    std::vector<MusicInfo> musicList;

    int fontHandleSmall = -1;
    int fontHandleLarge = -1;

    // 5 行の相対位置（中央を基準にしたオフセット）
    float posTable[5] = {
        -140.0f,    // さらに前の曲
        -70.0f,     // 前の曲
         0.0f,      // 今の曲（中央）
         70.0f,     // 次の曲
         140.0f     // さらに次の曲
    };

    // 5 行のフォントサイズ（中央だけ大きい）
    int fontTable[5] = {
        fontHandleSmall,
        fontHandleSmall,
        fontHandleLarge,   // 中央だけ大きい
        fontHandleSmall,
        fontHandleSmall
    };

    int selectedIndex = 0;
    Difficulty selectedDifficulty = Difficulty::Easy;

    float scrollOffset = 0.0f;
    const int longPressValue;

    int selectSE = -1;      // 選曲変更音

    int titleScroll = 0;
    int titleScrollTimer = 0;

    ScrollState scrollState = ScrollState::WaitStart;
    int scrollWaitTimer = 0;
};