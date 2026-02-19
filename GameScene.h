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

private:
    std::vector<Note> notes;

    float laneWidth = 100.0f;
    float noteHeight = 20.0f;
    float scrollSpeed = 300.0f;

    const float laneDepth = 2500.0f;
    const float laneFront = -300.0f;
    const float laneThickness = 10.0f;
    const float noteThickness = 5.0f;
    const float laneBaseY = 0;
    const float judgeLineZ = -170.0f;
    const float LfashLineZ = 2700.0f;

    int musicHandle = -1;

    const int laneTex;
    const int lineTex;
    const int noteTex;
    const int longNoteTex;

    // 判定関連
    int nextNoteIndex = 0;          // 次に判定するノーツ
    int judgeDisplayTimer = 0;      // 判定文字の表示時間
    int lastJudge = -1;             // 最後の判定結果

    // カウントダウン関連
    int countDown = 180;            // ３秒(60FPS想定)
    int started = false;            // 開始判定

    // レーン発光用
    int laneFlash[4] = { 0, 0, 0, 0 };

    int JudgeNote(int diffMs);      // 判定ロジック
    void DrawJudgeText();           // 判定文字描画
    void DrawLaneFlash3D();         // レーン発光
    void DrawCountDown();           // カウントダウン

    void DrawQuad3D(
        const VECTOR& p1,
        const VECTOR& p2,
        const VECTOR& p3,
        const VECTOR& p4,
        int tex
    );
};