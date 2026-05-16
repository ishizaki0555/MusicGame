// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// MusicSelectUI.h
// 選曲画面の機能を提供します。
// 
//========================================

#pragma once
#include <vector>
#include <string>
#include <math.h>
#include <map>
#include <filesystem>
#include <DxLib.h>
#include "GameScene.h"
#include "JsonLoader.h"

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace fs = std::filesystem;

// 難易度の種類
enum class Difficulty
{
    Easy = 0,       // 難易度イージー
    Normal,         // 難易度ノーマル
    Hard,           // 難易度ハード
    Extra           // 難易度エクストラ
};

// スクロールの状態
enum class ScrollState
{
    WaitStart,      // スクロール開始待ち
    Scroll,         // スクロール中
    WaitEnd         // スクロール終了待ち
};

// 選曲画面のUIクラス
class MusicSelectUI
{
public:

    // 楽曲情報をまとめた構造体
    struct MusicInfo
    {
        std::string folder;         // 楽曲のフォルダ名
        std::string file;           // 楽曲の音源ファイル名
        std::string banner;         // 楽曲のバナー画像ファイル名
        int offset = 0;             // オフセット(ms)

        std::string easyChart;      // イージー譜面ファイル名
        std::string normalChart;    // ノーマル譜面ファイル名
        std::string hardChart;      // ハード譜面ファイル名
        std::string extraChart;     // エクストラ譜面ファイル名

        int bpmEasy = 0;
        int bpmNormal = 0;
        int bpmHard = 0;
        int bpmExtra = 0;

        std::string title;          // 曲名
        int bannerHandle = -1;      // バナー画像のハンドル

        bool HasDifficulty(Difficulty diff) const {
            if (diff == Difficulty::Easy) return !easyChart.empty();
            if (diff == Difficulty::Normal) return !normalChart.empty();
            if (diff == Difficulty::Hard) return !hardChart.empty();
            if (diff == Difficulty::Extra) return !extraChart.empty();
            return false;
        }

        int GetBPM(Difficulty diff) const {
            if (diff == Difficulty::Easy) return bpmEasy;
            if (diff == Difficulty::Normal) return bpmNormal;
            if (diff == Difficulty::Hard) return bpmHard;
            if (diff == Difficulty::Extra) return bpmExtra;
            return 0;
        }
    };

    // @brief コンストラクタ
    // @param folderPath 楽曲が格納されたフォルダパス
    MusicSelectUI(const fs::path& folderPath);

    // @brief デストラクタ
    ~MusicSelectUI();

    // @brief 楽曲リストを読み込みます
    void LoadMusicList();
    
    // @brief フォントを読み込みます
    // @param fontPath フォントのファイルパス
    // @param size フォントサイズ
    void LoadFont(const std::string& fontPath, int size);

    // @brief 更新します
    void Update();

    // @brief 描画します
    void Draw();

    // @brief 入力状態をリセットします
    void ResetInputState();
    bool isAutoPlay = false;

    GameScene* createdScene = nullptr;      // 選曲から生成されたゲームシーン

    int selectSE = -1;      // 選曲変更音
    int bgmHandle = -1;     // 選曲画面BGM

    bool prevEnter = false;         // 前フレームのEnterキーの状態
    bool prevSpace = false;         // 前フレームのSpaceキーの状態
    bool sceneStarted = false;      // シーン開始フラグ

private:

    // @brief ボックスを描画します
    // @param x 左上のX座標
    // @param y 左上のY座標
    // @param w 幅
    // @param h 高さ
    // @param r 赤成分
    // @param g 緑成分
    // @param b 青成分
    // @param a アルファ値
    void DrawBoxEx(int x, int y, int w, int h, int r, int g, int b, int a);

    // @brief テキストを描画します
    // @param text 描画するテキスト
    // @param x 描画するX座標
	// @param y 描画するY座標
    // @param color 描画する色
    // @param font 描画するフォントのハンドル
    void DrawTextEx(const std::string& text, int x, int y, int color, int font);

    // @brief テキストを中央揃えで描画します
    // @param text 描画するテキスト
    // @param centerX 描画する中央のX座標
    // @param y 描画するY座標
    // @param color 描画する色
    // @param font 描画するフォントのハンドル
    void DrawTextCenter(const std::string& text, int centerX, int y, int color, int font);

    // @brief テキストを左揃えで描画します
    // @param text 描画するテキスト
    // @param leftX 描画する左端のX座標
    // @param y 描画するY座標
    // @param barWidth テキストがはみ出す場合のクリッピング幅
    // @param color 描画する色
    // @param font 描画するフォントのハンドル
    void DrawTextLeftClip(const std::string& text, int leftX, int y, int barWidth, int color, int font);

    //@brief テキストを左揃えで描画します
    // @param text 描画するテキスト
    // @param leftX 描画する左端のX座標
    // @param y 描画するY座標
    // @param barWidth テキストがはみ出すときのクリッピング幅
    // @param color 描画する色
    // @param font 描画するフォントのハンドル
    void DrawTextLeftSlide(const std::string& text, int leftX, int y, int barWidth, int color, int font);

    // @brief 難易度の応じた色を取得します
    // @param diff 難易度
    // @param r 赤成分
    // @param g 緑成分
    // @param b 青成分
    // @return 難易度に応じた色をr, g, bにセットします
    static void GetDifficultyColor(Difficulty diff, int& r, int& g, int& b);

private:
    fs::path folderPath;                // 楽曲が格納されたフォルダのパス
    std::vector<MusicInfo> musicList;   // 楽曲情報のリスト

    int fontHandleSmall = -1;           // 小さいフォントのハンドル
	int fontHandleLarge = -1;           // 大きいフォントのハンドル

    // 5 行の相対位置
    float posTable[5] = {
        -140.0f,    // さらに前の曲
        -70.0f,     // 前の曲
         0.0f,      // 今の曲（中央）
         70.0f,     // 次の曲
         140.0f     // さらに次の曲
    };

    // 5 行のフォントサイズ（中央だけ大きい）
    int fontTable[5] = {
        fontHandleSmall,    // さらに前の曲
        fontHandleSmall,    // 前の曲
        fontHandleLarge,    // 中央だけ大きい
		fontHandleSmall,	// 次の曲
		fontHandleSmall     // さらに次の曲
    };

    float currentViewIndex = 0.0f;
    int selectedIndex = 0;                              // 選曲中のインデックス
    Difficulty selectedDifficulty = Difficulty::Easy;   // 選択中の難易度

    float scrollOffset = 0.0f;                          // スクロールオフセット
    const int longPressValue;   		                // 長押しとみなすフレーム数

    int titleScroll = 0;                                // タイトルのスクロール量
    int titleScrollTimer = 0;                           // タイトルのスクロールタイマー

    ScrollState scrollState = ScrollState::WaitStart;   // スクロールの状態
    int scrollWaitTimer = 0;                            // スクロールの状態を切り替えるためのタイマー
};
