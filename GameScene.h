// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// GameScene.h
// ゲームプレイ中のノーツ描画・判定・スコア計算・UI表示など
// リズムゲームのメイン処理を担当するシーンです。
// 
//========================================

#pragma once

#include <vector>
#include <DxLib.h>
#include "Notes.h"

// ゲームプレイ中のシーン
class GameScene
{
public:

    // @brief コンストラクタ
    // @param ntoes ノーツデータ
    // @param banner バナー画像
    // @param autoPlay オートプレイフラグ
    GameScene(const NotesData& notes, int banner, bool autoPlay = false);   // ノーツデータとバナーを受け取って初期化

    // @brief 更新します
    void Update();

    // @brief 描画します
    void Draw();

    // @brief コンボ数を描画します
    void DrawCombo();

    // @brief スコアを描画します
    void DrawScore();

    // @brief 曲情報を描画します
    void DrawSongInfo();

    int GetScore() const { return score; }           // スコア取得
    int GetMaxCombo() const { return maxCombo; }        // 最大コンボ取得
    int GetPerfect() const { return perfectCount; }  // Perfect数取得
    int GetGreat() const { return greatCount; }      // Great数取得
    int GetGood() const { return goodCount; }        // Good数取得
    int GetMiss() const { return missCount; }        // Miss数取得
    int GetBannerHandle() const { return bannerHandle; } // バナーハンドル取得
    std::string GetSongName() const { return songName; } // 曲名取得

    // ノーツの種類
    struct NoteType
    {
        int const NORMAL_NOTE = 1;       // 通常ノーツ
        int const LONG_NOTE = 2;          // ロングノーツ
    };

	// ロングノーツの描画用データ
    struct LongBody 
    {
		int lane;               // レーン
		float startTime;        // 開始時間
		float endTime;          // 終了時間
        float startBeat;        // 開始拍数
        float endBeat;          // 終了拍数
    };

    // 判定文字の描画用データ
    struct JudgeTextInfo
    {
        float x;        // X座標
        float y;        // Y座標
        float z;        // Z座標
        int alpha;      // 透明度
        int timer;      // 表示時間
		int hold;	    // 静止時間
        int judgeType;  // 判定の種類
    };

    std::vector<JudgeTextInfo> judgeTexts;

    std::string Utf8ToSjis(const std::string& utf8); // UTF-8 → Shift-JIS 変換

    bool finished = false;                           // 楽曲終了フラグ
    bool IsFinished() const { return finished; }     // 終了判定
    bool IsRetire() const { return isRetire; }       // リタイア判定
    bool IsRetry() const { return isRetry; }         // リトライ判定
    const NotesData& GetNotesData() const { return m_notesData; } // ノーツデータ取得
    bool GetIsAutoPlay() const { return m_isAutoPlay;} // オートプレイ状態取得

private:
    bool m_isAutoPlay = false;                       // オートプレイ状態
    NotesData m_notesData;                           // ノーツデータ全体
    std::vector<JudgeNote> notes;                    // ノーツ一覧
	std::vector<LongBody> longBodies;                // ロングノーツの描画用データ
	std::vector<int> noteConsumed;				     // ノーツ消費フラグ

    float laneWidth = 100.0f;                        // レーン幅
    float noteHeight = 20.0f;                        // ノーツの奥行き
    float scrollSpeed = 1600.0f;                     // ノーツのスクロール速度

    const float LANE_DEPTH = 2500.0f;                // レーン奥行き
    const float LANE_FRONT = -300.0f;                // レーン手前位置
    const float LANE_THICKNESS = 10.0f;              // レーンの厚み
    const float NOTE_THICKNESS = 5.0f;               // ノーツの厚み
    const float LANE_BASE_Y = 0;                     // レーンのY座標
    const float JUDGE_LINE_Z = 0.0f;                 // 判定ラインのZ座標
    const float FLASH_LINE_Z = 2700.0f;              // レーン発光の奥行き

    int musicHandle = -1;                            // 楽曲ハンドル

    // テクスチャハンドル
    const int LANE_TEX;                              // レーンテクスチャ
    const int LIGHT_TEX;                             // ライとテクスチャ
    const int LINE_TEX;                              // 判定ラインテクスチャ
    const int NOTE_TEX;                              // ノーツテクスチャ
    const int LONG_NOTE_TEX;                         // ロングノーツテクスチャ

    // 判定範囲(ミリ秒)
    const float PERFECT_RANGE = 0.10f;                // PERFECT 判定範囲
    const float GREAT_RANGE = 0.12f;                  // GREAT 判定範囲
    const float GOOD_RANGE = 0.13f;                   // GOOD 判定範囲

    // 判定関連
    int nextNoteIndex[6] = { 0, 0, 0, 0, 0, 0 };              // 各レーンの次に判定するノーツ
    bool holding[6] = { false, false, false, false, false, false };   // ロングノーツを押しているかどうか
    int holdingNoteIndex[6] = { -1, -1, -1, -1, -1, -1 };       // どのノーツをホールド中か
    int judgeDisplayTimer = 0;                          // 判定文字の表示タイマー
    int lastJudge = -1;                                 // 最後の判定結果

    // 効果音関連
    int hitSE = -1;                                     // 通常ノーツヒット音
	int rongSE = -1;    							    // ロングノーツヒット音      

    // キー設定
    bool prevKey[6] = {false, false, false, false, false, false};
    bool nowKey[6];
    bool keyDown[6];
    bool keyUp[6];

    // 各判定カウント
    int perfectCount = 0;                            // Perfect 数
    int greatCount = 0;                              // Great 数
    int goodCount = 0;                               // Good 数
    int missCount = 0;                               // Miss 数

    int judgeLane = -1;                              // 判定が出たレーン
    float judgeZ = 0.0f;                             // 判定ノーツのZ位置
    float judgeTextY = 0.0f;                         // 判定文字のYオフセット
    int judgeAlpha = 255;                            // 判定文字の透明度
    int judgeHoldTimer = 500;                        // 判定文字の静止時間

    // スコア関連
    float ratioScore = 0.0f;                         // スコア計算用の比率
    float maxScore = 1000000.0f;                     // 最大スコア

    // UI関連
    int score = 0;                                   // 表示用スコア
    int combo = 0;                                   // 現在のコンボ
    int maxCombo = 0;                                // 最大コンボ
    std::string songName;                            // 曲名
    const int JUDGE_BASE_Y = 300;                    // 判定のテキストのY固定座標

    int bannerHandle = -1;                           // バナー画像ハンドル
    int fontMusicName = -1;                          // 曲名表示用フォントハンドル
    int judgeFont = -1;                              // 判定テキスト表示用フォントハンドル

    int countDown = 180;                             // 開始前カウントダウン（3秒）
    int started = false;                             // 開始フラグ

    // ポーズ関連
    bool isPaused = false;                           // ポーズ中かどうか
    int pauseMenuIndex = 0;                          // ポーズメニューの選択インデックス
    bool isResuming = false;                         // ポーズ解除後のカウントダウン中か
    int resumeCountDown = 0;                         // ポーズ解除のカウントダウン
    int pausedTimeMs = 0;                            // ポーズした瞬間の楽曲時間
    bool isRetire = false;                           // リタイアしたか
    bool isRetry = false;                            // リトライしたか
    bool prevEscapeKey = false;                      // 前回のESCキー
    bool prevUpKey = false;                          // 前回のUPキー
    bool prevDownKey = false;                        // 前回のDOWNキー
    bool prevReturnKey = false;                      // 前回のエンターキー

    int pauseFontLarge = -1;                         // ポーズメニュー用大フォント
    int pauseFontSmall = -1;                         // ポーズメニュー用小フォント

    // レーン発光
    int laneFlash[6] = { 0, 0, 0, 0, 0, 0 };               // レーン発光タイマー

    // @brief 判定ロジック
    // @param diffMs 判定ラインとの差(ミリ秒)
    int Judge(int diffMs);

    // @brief 判定文字の追加
    // @param lane レーン
	// @param judgeType 判定の種類（0=PERFECT,1=GREAT,2=GOOD,3=MISS）
    // @param noteIndex 判定したノーツのインデックス
	void AddJudgeText(int lane, int judgeType, int noteIndex);
    
    // @brief 判定文字描画
    void DrawJudgeText();

    // @brief レーン発光描画
    void DrawLaneFlash3D();

    // @brief カウントダウン描画
    void DrawCountDown();

    // @brief 3D四角形描画
    // @param p1 頂点１
	// @param p2 頂点２
	// @param p3 頂点３
	// @param p4 頂点４
    void DrawQuad3D(
        const VECTOR& p1,   // 頂点１
        const VECTOR& p2,   // 頂点２
        const VECTOR& p3,   // 頂点３
        const VECTOR& p4,   // 頂点４
        int tex
    );
};
