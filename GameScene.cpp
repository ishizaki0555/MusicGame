#include "GameScene.h"

GameScene::GameScene(const NotesData& notesData)
    : LANE_TEX(LoadGraph("Texture/LaneTexture.png"))
    , LINE_TEX(LoadGraph("Texture/LineTexture.png"))
    , NOTE_TEX(LoadGraph("Texture/NoteTexture.png"))
    , LONG_NOTE_TEX(LoadGraph("Texture/LongNoteTexture.png"))
{
    notes = notesData.notes;
    songName = notesData.title;

    musicHandle = LoadSoundMem(notesData.musicPath.c_str());
}

int GameScene::JudgeNote(int diffMs)
{
    diffMs = abs(diffMs);

    if (diffMs <= PERFECT_RANGE) return 0;
    if (diffMs <= GREAT_RANGE) return 1;
    if (diffMs <= GOOD_RANGE) return 2;
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
    // ★ レーン光り処理（SDJK）
    // ============================
    int keys[4] = {
        KEY_INPUT_S,
        KEY_INPUT_D,
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
    // レーン別判定（Z距離ベース）
    // ============================
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    for (int lane = 0; lane < 4; lane++)
    {
        // そのレーンのノーツを全部チェック
        while (nextNoteIndex[lane] < notes.size() &&
            notes[nextNoteIndex[lane]].lane != lane)
        {
            nextNoteIndex[lane]++;
        }

        if (nextNoteIndex[lane] >= notes.size()) continue;

        Note& n = notes[nextNoteIndex[lane]];

        // Z 座標計算
        float dt = n.time - currentTime;
        float z = dt * scrollSpeed;

        // 判定ラインとの差
        float diffZ = z - JUDGE_LINE_Z;

        // 判定ラインを GOOD より手前に通り過ぎたら MISS
        if (diffZ < -GOOD_RANGE)
        {
            float dt = n.time - currentTime;
            float z = dt * scrollSpeed;

            judgeLane = lane;
            judgeZ = z;

            lastJudge = 3;
            judgeDisplayTimer = 30;
            n.judged = true;
            nextNoteIndex[lane]++;
            continue;
        }

        // キー押したら判定（判定ゾーン内のみ）
        if (CheckHitKey(keys[lane]) == 1)
        {
            float ad = fabs(diffZ);

            // 判定ゾーン外 → 何も起きない
            if (ad > GOOD_RANGE)
                continue;

            // 判定ゾーン内 → 判定する
            if (ad <= PERFECT_RANGE)
                lastJudge = 0; // PERFECT
            else if (ad <= GREAT_RANGE)
                lastJudge = 1; // GREAT
            else
                lastJudge = 2; // GOOD

            judgeLane = lane;
            judgeZ = z;

            judgeTextY = 0.0f;
            judgeAlpha = 255;
            judgeHoldTimer = 10;

            judgeDisplayTimer = 30;
            n.judged = true;
            nextNoteIndex[lane]++;
        }
    }
}

void GameScene::DrawCountDown()
{
    if (started) return;

    int sec = countDown / 60 + 1;

    char buf[32];
    sprintf_s(buf, "%d", sec);

    DrawString(600, 300, buf, GetColor(255, 255, 255));
}

void GameScene::DrawJudgeZone()
{
    // レーン全体の横幅
    float x1 = -200;
    float x2 = 200;

    auto drawZone = [&](float z1, float z2, int r, int g, int b)
        {
            VERTEX3D v[6];

            auto setV = [&](int idx, float x, float y, float z)
                {
                    v[idx].pos = VGet(x, y, z);
                    v[idx].norm = VGet(0, 1, 0);
                    v[idx].dif = GetColorU8(r, g, b, 120); // 半透明
                    v[idx].spc = GetColorU8(0, 0, 0, 0);
                    v[idx].u = 0;
                    v[idx].v = 0;
                };

            setV(0, x1, 0, z1);
            setV(1, x2, 0, z1);
            setV(2, x2, 0, z2);

            setV(3, x1, 0, z1);
            setV(4, x2, 0, z2);
            setV(5, x1, 0, z2);

            DrawPolygon3D(v, 2, LANE_TEX, TRUE);
        };

    // PERFECT（黄色）
    drawZone(JUDGE_LINE_Z - PERFECT_RANGE,
        JUDGE_LINE_Z + PERFECT_RANGE,
        255, 255, 0);

    // GREAT（赤）
    drawZone(JUDGE_LINE_Z - GREAT_RANGE,
        JUDGE_LINE_Z - PERFECT_RANGE,
        255, 80, 80);
    drawZone(JUDGE_LINE_Z + PERFECT_RANGE,
        JUDGE_LINE_Z + GREAT_RANGE,
        255, 80, 80);

    // GOOD（緑）
    drawZone(JUDGE_LINE_Z - GOOD_RANGE,
        JUDGE_LINE_Z - GREAT_RANGE,
        80, 255, 120);
    drawZone(JUDGE_LINE_Z + GREAT_RANGE,
        JUDGE_LINE_Z + GOOD_RANGE,
        80, 255, 120);

    // MISS（灰色）※任意
    drawZone(JUDGE_LINE_Z - GOOD_RANGE - 60,
        JUDGE_LINE_Z - GOOD_RANGE,
        120, 120, 120);
    drawZone(JUDGE_LINE_Z + GOOD_RANGE,
        JUDGE_LINE_Z + GOOD_RANGE + 60,
        120, 120, 120);
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
        float z = JUDGE_LINE_Z + 5;

        setV(0, x1, 0, z);
        setV(1, x2, 0, z);
        setV(2, x2, 0, z + FLASH_LINE_Z);

        setV(3, x1, 0, z);
        setV(4, x2, 0, z + FLASH_LINE_Z);
        setV(5, x1, 0, z + FLASH_LINE_Z);

        DrawPolygon3D(v, 2, NOTE_TEX, TRUE);
    }
}

void GameScene::DrawJudgeText()
{
    if (judgeDisplayTimer <= 0) return;
    if (judgeLane < 0) return;

    const char* text = "";
    int baseColor = GetColor(255, 255, 255);

    switch (lastJudge)
    {
    case 0: 
        text = "PERFECT";
        baseColor = GetColor(255, 255, 0);
        score += 5;
        combo++;
        break;
    case 1:
        text = "GREAT";
        baseColor = GetColor(255, 80, 80);
        score += 3;
        combo++;
        break;
    case 2:
        text = "GOOD";
        baseColor = GetColor(80, 255, 120);
        score++;
        combo = 0;
        break;
    case 3:
        text = "MISS";
        baseColor = GetColor(120, 120, 120);
        combo = 0;
        break;
    }

    // レーンのX座標
    float x1 = (judgeLane * laneWidth) - (laneWidth * 2) + 10;
    float x2 = x1 + laneWidth - 20;
    float centerX = (x1 + x2) / 2;

    // 3D → 2D 変換
    VECTOR world = VGet(centerX, 30, judgeZ);
    VECTOR screen = ConvWorldPosToScreenPos(world);

    // 停止時間が残っている間は動かさない
    if (judgeHoldTimer > 0)
    {
        judgeHoldTimer--;
    }
    else
    {
        // 停止後にスライド開始
        judgeTextY -= 1.0f;

        // フェードアウト開始
        judgeAlpha -= 8;
        if (judgeAlpha < 0) judgeAlpha = 0;
    }

    // 色にアルファを適用
    int r, g, b;
    GetColor2(baseColor, &r, &g, &b);
    int color = GetColor(r, g, b);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, judgeAlpha);

    DrawString(
        (int)screen.x - 40,
        (int)(screen.y - 20 + judgeTextY),
        text,
        color
    );

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
            VGet(x, LANE_BASE_Y, LANE_FRONT),
            VGet(x + laneWidth, LANE_BASE_Y, LANE_FRONT),
            VGet(x + laneWidth, LANE_BASE_Y, LANE_DEPTH),
            VGet(x, LANE_BASE_Y, LANE_DEPTH),
            LANE_TEX
        );
    }

    // 判定ライン
    DrawQuad3D(
        VGet(-200, LANE_BASE_Y, JUDGE_LINE_Z),
        VGet(200, LANE_BASE_Y, JUDGE_LINE_Z),
        VGet(200, LANE_BASE_Y + 5, JUDGE_LINE_Z),
        VGet(-200, LANE_BASE_Y + 5, JUDGE_LINE_Z),
        LINE_TEX
    );

    //DrawJudgeZone();

    // カウントダウン表示
    DrawCountDown();

    // カウントダウン中はノーツを描画しない
    if (!started) return;

    // ノーツ描画
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    for (auto& n : notes)
    {
        if (n.judged) continue;

        float dt = n.time - currentTime;
        float z = dt * scrollSpeed;

        if (z < LANE_FRONT || z > LANE_DEPTH) continue;

        float x1 = (n.lane * laneWidth) - (laneWidth * 2) + 10;
        float x2 = x1 + laneWidth - 20;

        DrawQuad3D(
            VGet(x1, 0, z),
            VGet(x2, 0, z),
            VGet(x2, 0, z + noteHeight),
            VGet(x1, 0, z + noteHeight),
            NOTE_TEX
        );
    }

    // レーン光り
    DrawLaneFlash3D();

    // 判定文字
    DrawJudgeText();

    DrawScore();
    DrawCombo();
    DrawSongInfo();
}

void GameScene::DrawCombo()
{
    if (combo <= 0) return;

    char buf[32];
    sprintf_s(buf, "%d", combo);

    // 右側に固定
    DrawString(1100, 300, buf, GetColor(255, 255, 255));
}

void GameScene::DrawScore()
{
    char buf[32];
    sprintf_s(buf, "%07d", score);  // ゼロ埋め
    DrawString(30, 20, buf, GetColor(255, 255, 255));
}

void GameScene::DrawSongInfo()
{
    DrawString(30, 60, songName.c_str(), GetColor(200, 200, 200));
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