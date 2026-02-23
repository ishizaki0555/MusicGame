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

class GameScene
{
public:
    GameScene(const NotesData& notes, int banner);   // ノーツデータとバナーを受け取って初期化

    void Update();                                   // 毎フレームの更新処理
    void Draw();                                     // 毎フレームの描画処理

    void DrawCombo();                                // コンボ表示
    void DrawScore();                                // スコア表示
    void DrawSongInfo();                             // 曲名表示

    int GetScore() const { return score; }           // スコア取得
    int GetMaxCombo() const { return combo; }        // 最大コンボ取得
    int GetPerfect() const { return perfectCount; }  // Perfect数取得
    int GetGreat() const { return greatCount; }      // Great数取得
    int GetGood() const { return goodCount; }        // Good数取得
    int GetMiss() const { return missCount; }        // Miss数取得
    int GetBannerHandle() const { return bannerHandle; } // バナーハンドル取得
    std::string GetSongName() const { return songName; } // 曲名取得

    std::string Utf8ToSjis(const std::string& utf8); // UTF-8 → Shift-JIS 変換

    bool finished = false;                           // 楽曲終了フラグ
    bool IsFinished() const { return finished; }     // 終了判定

private:
    std::vector<Note> notes;                         // ノーツ一覧

    float laneWidth = 100.0f;                        // レーン幅
    float noteHeight = 20.0f;                        // ノーツの奥行き
    float scrollSpeed = 1600.0f;                     // ノーツのスクロール速度

    const float LANE_DEPTH = 2500.0f;                // レーン奥行き
    const float LANE_FRONT = -300.0f;                // レーン手前位置
    const float LANE_THICKNESS = 10.0f;              // レーンの厚み
    const float NOTE_THICKNESS = 5.0f;               // ノーツの厚み
    const float LANE_BASE_Y = 0;                     // レーンのY座標
    const float JUDGE_LINE_Z = -210.0f;              // 判定ラインのZ座標
    const float FLASH_LINE_Z = 2700.0f;              // レーン発光の奥行き

    int musicHandle = -1;                            // 楽曲ハンドル

    const int LANE_TEX;                              // レーンテクスチャ
    const int LINE_TEX;                              // 判定ラインテクスチャ
    const int NOTE_TEX;                              // ノーツテクスチャ
    const int LONG_NOTE_TEX;                         // ロングノーツテクスチャ

    const float PERFECT_RANGE = 30.0f;               // PERFECT 判定範囲
    const float GREAT_RANGE = 60.0f;                 // GREAT 判定範囲
    const float GOOD_RANGE = 100.0f;                 // GOOD 判定範囲

    // 判定関連
    int nextNoteIndex[4] = { 0, 0, 0, 0 };           // 各レーンの次に判定するノーツ
    int judgeDisplayTimer = 0;                       // 判定文字の表示タイマー
    int lastJudge = -1;                              // 最後の判定結果

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
    std::string songName;                            // 曲名

    int bannerHandle = -1;                           // バナー画像ハンドル

    // カウントダウン関連
    int countDown = 180;                             // 開始前カウントダウン（3秒）
    int started = false;                             // 開始フラグ

    // レーン発光
    int laneFlash[4] = { 0, 0, 0, 0 };               // レーン発光タイマー

    int JudgeNote(int diffMs);                       // 判定ロジック
    void DrawJudgeText();                            // 判定文字描画
    void DrawLaneFlash3D();                          // レーン発光描画
    void DrawCountDown();                            // カウントダウン描画

    void DrawJudgeZone();                            // 判定ゾーン描画（デバッグ用）

    void DrawQuad3D(                                 // 3D四角形描画
        const VECTOR& p1,
        const VECTOR& p2,
        const VECTOR& p3,
        const VECTOR& p4,
        int tex
    );
};
