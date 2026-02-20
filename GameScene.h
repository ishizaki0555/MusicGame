#pragma once

#include <vector>
#include <DxLib.h>
#include "Notes.h"

class GameScene
{
public:
    GameScene(const NotesData& notes);

    void Update();
    void Draw();

    void DrawCombo();
    void DrawScore();
    void DrawSongInfo();

private:
    std::vector<Note> notes;

    float laneWidth = 100.0f;
    float noteHeight = 20.0f;
    float scrollSpeed = 1600.0f;

    const float LANE_DEPTH = 2500.0f;
    const float LANE_FRONT = -300.0f;
    const float LANE_THICKNESS = 10.0f;
    const float NOTE_THICKNESS = 5.0f;
    const float LANE_BASE_Y = 0;
    const float JUDGE_LINE_Z = -210.0f;
    const float FLASH_LINE_Z = 2700.0f;

    int musicHandle = -1;

    const int LANE_TEX;
    const int LINE_TEX;
    const int NOTE_TEX;
    const int LONG_NOTE_TEX;

    const float PERFECT_RANGE = 30.0f;
    const float GREAT_RANGE = 60.0f;
    const float GOOD_RANGE = 100.0f;

    // 判定関連
    int nextNoteIndex[4] = { 0, 0, 0, 0 };          // 次に判定するノーツ
    int judgeDisplayTimer = 0;                      // 判定文字の表示時間
    int lastJudge = -1;                             // 最後の判定結果

    int judgeLane = -1;                             // 判定が出たレーン
    float judgeZ = 0.0f;                            // 判定が出たノーツのZ位置
    float judgeTextY = 0.0f;
    int judgeAlpha = 255;
    int judgeHoldTimer = 500;

    // UI関連
    int score = 0;
    int combo = 0;
    std::string songName;

    // カウントダウン関連
    int countDown = 180;                            // ３秒(60FPS想定)
    int started = false;                            // 開始判定

    // レーン発光用
    int laneFlash[4] = { 0, 0, 0, 0 };

    int JudgeNote(int diffMs);          // 判定ロジック
    void DrawJudgeText();               // 判定文字描画
    void DrawLaneFlash3D();             // レーン発光
    void DrawCountDown();               // カウントダウン

    void DrawJudgeZone();

    void DrawQuad3D(
        const VECTOR& p1,
        const VECTOR& p2,
        const VECTOR& p3,
        const VECTOR& p4,
        int tex
    );
};