#include "GameScene.h"

GameScene::GameScene(const NotesData& notesData)
    : laneTex(LoadGraph("Texture/LaneTexture.png"))
    , lineTex(LoadGraph("Texture/LineTexture.png"))
    , noteTex(LoadGraph("Texture/NoteTexture.png"))
    , longNoteTex(LoadGraph("Texture/LongNoteTexture.png"))
{
    notes = notesData.notes;

    musicHandle = LoadSoundMem(notesData.musicPath.c_str());
}

int GameScene::JudgeNote(int diffMs)
{
    diffMs = abs(diffMs);

    if (diffMs <= 30) return 0;
    if (diffMs <= 60) return 1;
    if (diffMs <= 100) return 2;
    return 3;
}

void GameScene::Update()
{
    // ============================
    // ★ カウントダウン処理
    // ============================
    if (!started)
    {
        countDown--;
        if (countDown <= 0)
        {
            started = true;
            PlaySoundMem(musicHandle, DX_PLAYTYPE_BACK);
        }
        return;
    }

    // ============================
    // ★ レーン光り処理（DFJK）
    // ============================
    int keys[4] = {
        KEY_INPUT_D,
        KEY_INPUT_F,
        KEY_INPUT_J,
        KEY_INPUT_K
    };

    for (int i = 0; i < 4; i++)
    {
        if (CheckHitKey(keys[i]))
            laneFlash[i] = 10; // 光る時間
        else if (laneFlash[i] > 0)
            laneFlash[i]--;
    }

    // ============================
    // ★ 判定処理
    // ============================
    if (nextNoteIndex >= notes.size()) return;

    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;
    Note& n = notes[nextNoteIndex];

    int diffMs = (int)((n.time - currentTime) * 1000);

    // 自動MISS
    if (diffMs < -120)
    {
        lastJudge = 3;
        judgeDisplayTimer = 30;
        nextNoteIndex++;
        return;
    }

    // スペース or レーンキーで判定
    if (CheckHitKey(KEY_INPUT_SPACE) == 1 ||
        CheckHitKey(KEY_INPUT_D) == 1 ||
        CheckHitKey(KEY_INPUT_F) == 1 ||
        CheckHitKey(KEY_INPUT_J) == 1 ||
        CheckHitKey(KEY_INPUT_K) == 1)
    {
        lastJudge = JudgeNote(diffMs);
        judgeDisplayTimer = 30;
        nextNoteIndex++;
    }

    if (judgeDisplayTimer > 0)
        judgeDisplayTimer--;
}

void GameScene::DrawCountDown()
{
    if (started) return;

    int sec = countDown / 60 + 1;

    char buf[32];
    sprintf_s(buf, "%d", sec);

    DrawString(600, 300, buf, GetColor(255, 255, 255));
}

void GameScene::DrawLaneFlash3D()
{
    for (int i = 0; i < 4; i++)
    {
        if (laneFlash[i] <= 0) continue;

        float x1 = (i * laneWidth) - (laneWidth * 2);
        float x2 = x1 + laneWidth;

        int alpha = laneFlash[i] * 20; // フェードアウト

        // 半透明白
        COLOR_U8 col = GetColorU8(255, 255, 255, alpha);

        VERTEX3D v[6];

        auto setV = [&](int idx, float x, float y, float z)
            {
                v[idx].pos = VGet(x, y, z);
                v[idx].norm = VGet(0, 1, 0);
                v[idx].dif = col;
                v[idx].spc = GetColorU8(0, 0, 0, 0);
                v[idx].u = 0;
                v[idx].v = 0;
            };

        // 判定ライン付近に薄い板を置く
        float z = judgeLineZ + 5;

        setV(0, x1, 0, z);
        setV(1, x2, 0, z);
        setV(2, x2, 0, z + LfashLineZ);

        setV(3, x1, 0, z);
        setV(4, x2, 0, z + LfashLineZ);
        setV(5, x1, 0, z + LfashLineZ);

        DrawPolygon3D(v, 2, noteTex, TRUE);
    }
}

void GameScene::DrawJudgeText()
{
    if (judgeDisplayTimer <= 0) return;

    const char* text = "";
    int color = GetColor(255, 255, 255);

    switch (lastJudge)
    {
    case 0: text = "PERFECT"; color = GetColor(80, 200, 255); break;
    case 1: text = "GREAT";   color = GetColor(80, 255, 120); break;
    case 2: text = "GOOD";    color = GetColor(255, 200, 80); break;
    case 3: text = "MISS";    color = GetColor(255, 80, 80); break;
    }

    DrawString(600, 300, text, color);
}

void GameScene::Draw()
{
    DrawBox(0, 0, 1280, 720, GetColor(20, 20, 20), TRUE);

    VECTOR eye = VGet(0.0f, 200.0f, -350.0f);
    VECTOR target = VGet(0.0f, 0.0f, 0.0f);

    SetCameraPositionAndTarget_UpVecY(eye, target);

    // レーン描画
    for (int i = 0; i < 4; i++)
    {
        float x = (i * laneWidth) - (laneWidth * 2);

        DrawQuad3D(
            VGet(x, laneBaseY, laneFront),
            VGet(x + laneWidth, laneBaseY, laneFront),
            VGet(x + laneWidth, laneBaseY, laneDepth),
            VGet(x, laneBaseY, laneDepth),
            laneTex
        );
    }

    // 判定ライン
    DrawQuad3D(
        VGet(-200, laneBaseY, judgeLineZ),
        VGet(200, laneBaseY, judgeLineZ),
        VGet(200, laneBaseY + 5, judgeLineZ),
        VGet(-200, laneBaseY + 5, judgeLineZ),
        lineTex
    );

    // カウントダウン表示
    DrawCountDown();

    // カウントダウン中はノーツを描画しない
    if (!started) return;

    // ノーツ描画
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    for (auto& n : notes)
    {
        float dt = n.time - currentTime;
        float z = dt * scrollSpeed;

        if (z < laneFront || z > laneDepth) continue;

        float x1 = (n.lane * laneWidth) - (laneWidth * 2) + 10;
        float x2 = x1 + laneWidth - 20;

        DrawQuad3D(
            VGet(x1, 0, z),
            VGet(x2, 0, z),
            VGet(x2, 0, z + noteHeight),
            VGet(x1, 0, z + noteHeight),
            noteTex
        );
    }

    // レーン光り
    DrawLaneFlash3D();

    // 判定文字
    DrawJudgeText();
}

void GameScene::DrawQuad3D(
    const VECTOR& p1,
    const VECTOR& p2,
    const VECTOR& p3,
    const VECTOR& p4,
    int tex
)
{
    VERTEX3D v[6];

    v[0].pos = p1; v[1].pos = p2; v[2].pos = p3;
    v[3].pos = p1; v[4].pos = p3; v[5].pos = p4;

    for (int i = 0; i < 6; i++)
    {
        v[i].norm = VGet(0.0f, 1.0f, 0.0f);
        v[i].dif = GetColorU8(255, 255, 255, 255);
        v[i].spc = GetColorU8(0, 0, 0, 0);

        v[i].u = (i == 1 || i == 2 || i == 4) ? 1.0f : 0.0f;
        v[i].v = (i == 2 || i == 3 || i == 4) ? 1.0f : 0.0f;
    }

    DrawPolygon3D(v, 2, tex, TRUE);
}