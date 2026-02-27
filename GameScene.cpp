// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// GameScene.cpp
// ノーツの移動・判定処理、スコア計算、UI描画、レーン演出など
// リズムゲームのメインロジックを実装したシーンです。
// 
//========================================

#include <iostream>
#include "GameScene.h"

// @brief  コンストラクタ
// @param notesData ノーツデータ
// @param banner    バナー画像ハンドル
GameScene::GameScene(const NotesData& notesData, int banner)
    : LANE_TEX(LoadGraph("Texture/LaneTexture.png"))
    , LIGHT_TEX(LoadGraph("Texture/LightTexture.png"))
    , LINE_TEX(LoadGraph("Texture/LineTexture.png"))
    , NOTE_TEX(LoadGraph("Texture/NoteTexture.png"))
    , LONG_NOTE_TEX(LoadGraph("Texture/LongNoteTexture.png"))
{
    notes = notesData.notes;                 // ノーツ一覧をコピー
    songName = notesData.title;              // 曲名を保存
    bannerHandle = banner;                   // バナーハンドルを保存

    musicHandle = LoadSoundMem(notesData.musicPath.c_str()); // 楽曲読み込み

    // スコアの最大値をノーツ数から計算
    int noteNum = static_cast<int>(notes.size());
    maxScore = noteNum * 5;
}

// @brief 判定ロジック
// @param diffMs 判定ラインとの差（ミリ秒）
// @return 判定結果（0=PERFECT,1=GREAT,2=GOOD,3=MISS）
int GameScene::JudgeNote(int diffMs)
{
    diffMs = abs(diffMs);

    if (diffMs <= PERFECT_RANGE) return 0;   // PERFECT 判定
    if (diffMs <= GREAT_RANGE)   return 1;   // GREAT 判定
    if (diffMs <= GOOD_RANGE)    return 2;   // GOOD 判定
    return 3;                                // MISS 判定
}

/// <summary>
/// 毎フレーム更新処理
/// </summary>
void GameScene::Update()
{
    // ============================
    // カウントダウン処理
    // ============================
    // ゲーム開始前かどうか判定
    if (!started)
    {
        countDown--;    // カウントダウンを進める

        // カウントが0になったら開始
        if (countDown <= 0)
        {
            started = true;
            PlaySoundMem(musicHandle, DX_PLAYTYPE_BACK); // 楽曲再生
        }
        return; // 開始前はここで終了
    }

    // ============================
    // レーン入力処理（S D J K）
    // ============================
    int keys[4] = {
        KEY_INPUT_S,
        KEY_INPUT_D,
        KEY_INPUT_J,
        KEY_INPUT_K
    };

    

    // 各レーンのキー入力を確認
    for (int i = 0; i < 4; i++)
    {
        nowKey[i] = (CheckHitKey(keys[i]) != 0);    // 現在のキーを記録
        keyDown[i] = (nowKey[i] && !prevKey[i]);    // キーを押した瞬間
        keyUp[i] = (!nowKey[i] && prevKey[i]);      // キーを離した瞬間
        prevKey[i] = nowKey[i];                     // 前のキーを更新

        // キーが押されたら
        if (CheckHitKey(keys[i]))
            laneFlash[i] = 10;        // レーン発光を開始
        // 発光中なら
        else if (laneFlash[i] > 0)
            laneFlash[i]--;           // 徐々に減衰
    }

    // ============================
    // ノーツ処理（時間ベースの新判定）
    // ============================
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    // レーンごとに処理
    for (int lane = 0; lane < 4; lane++)
    {
        // ロングノーツのホールド判定
        if (holding[lane])
        {
            int holdIndex = holdingNoteIndex[lane];
            if (holdIndex < 0 || holdIndex >= (int)notes.size())
            {
                holding[lane] = false;
                holdingNoteIndex[lane] = -1;
                continue;
            }

            Note& n = notes[holdIndex];

            // 終点時間を過ぎたらKeyUp判定へ移行
            double diffEndSec = currentTime - n.endTime;
            int diffEndMs = (int)(fabs(diffEndSec) * 1000.0);

            // KeyUp判定
            if (keyUp[lane])
            {
                int result = JudgeNote(diffEndMs);
                lastJudge = result;

                switch (result)
                {
                case 0: perfectCount++; ratioScore += 5; combo++; break;
                case 1: greatCount++; ratioScore += 3; combo++; break;
                case 2: goodCount++; ratioScore ++; combo = 0; break;
                case 3: missCount++; ; combo = 0; break;
                }

				SetJudgeDate(lane, n);

                n.judged = true;
                holding[lane] = false;
                holdingNoteIndex[lane] = -1;
            }
            else
            {
                // ホールド中に終点範囲を超えたらMISS
                if (diffEndMs > GOOD_RANGE)
                {
                    lastJudge = 3;
                    missCount++;
                    combo = 0;

					SetJudgeDate(lane, n);

                    n.judged = true;
                    holding[lane] = false;
                    holdingNoteIndex[lane] = -1;
                }
            }
            // 判定が終わったら次のノーツへ
            continue;
        }

        // このレーンの一番手前の未判定ノーツを探す
        int targetIndex = -1;
        for (int i = 0; i < (int)notes.size(); i++)
        {
            if (notes[i].judged) continue;
            if (notes[i].lane != lane) continue;

            targetIndex = i;
            break;
        }

        if (targetIndex == -1) continue; // このレーンに未判定ノーツが無い

        Note& n = notes[targetIndex];

        // ノーツ予定時刻との差（ms）
        double diffSec = currentTime - n.time;
        int diffMs = (int)(diffSec * 1000.0);

        // まだかなり早いなら何もしない（早押しは無視）
        if (diffMs < -GOOD_RANGE) continue;

        // 押さないまま GOOD 範囲を過ぎたら MISS
        if (diffMs > GOOD_RANGE)
        {
            lastJudge = 3;          // MISS
            missCount++;
            combo = 0;

            SetJudgeDate(lane, n);

            n.judged = true;
            continue;
        }

        // このフレームでキーが押された瞬間だけ判定
        if (keyDown[lane])
        {
            int result = JudgeNote(diffMs);
            lastJudge = result;

            switch (result)
            {
            case 0: perfectCount++; ratioScore += 5; combo++; break;
            case 1: greatCount++;   ratioScore += 3; combo++; break;
            case 2: goodCount++;    ratioScore += 1; combo = 0; break;
            case 3: missCount++;    combo = 0;       break;
            }

            SetJudgeDate(lane, n);

            // ロングノーツならホールド開始
            if (n.type == 2 && n.endTime > n.time)
            {
                holding[lane] = true;
                holdingNoteIndex[lane] = targetIndex;
            }
            else
            {
                n.judged = true;
            }
        }
    }

    // ============================
    // スコア計算
    // ============================
    score = (int)(1000000.0f * floor((ratioScore / maxScore) * 1000000.0f) / 1000000.0f);

    // ============================
    // 楽曲終了判定
    // ============================
    if (GetSoundCurrentTime(musicHandle) >= GetSoundTotalTime(musicHandle))
    {
        finished = true; // 楽曲終了
    }
}

void GameScene::SetJudgeDate(int lane, Note& n)
{
	judgeLane = lane;
	judgeZ = (float)((n.time - GetSoundCurrentTime(musicHandle) / 1000.0) * scrollSpeed);
	judgeTextY = 0.0f;
	judgeAlpha = 255;
	judgeHoldTimer = 10;
	judgeDisplayTimer = 30;
}

void GameScene::DrawCountDown()
{
    if (started) return;   // 開始後は表示しない

    int sec = countDown / 60 + 1; // 残り秒数を計算

    char buf[32];
    sprintf_s(buf, "%d", sec);

    DrawString(600, 300, buf, GetColor(255, 255, 255)); // カウントダウン表示
}

void GameScene::DrawJudgeZone()
{
    // レーン全体の幅
    float x1 = -200;
    float x2 = 200;

    // 判定ゾーン描画ラムダ
    auto drawZone = [&](float z1, float z2, int r, int g, int b)
        {
            VERTEX3D v[6];

            auto setV = [&](int idx, float x, float y, float z)
                {
                    v[idx].pos = VGet(x, y, z);
                    v[idx].norm = VGet(0, 1, 0);
                    v[idx].dif = GetColorU8(r, g, b, 120);
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

    // PERFECT
    drawZone(JUDGE_LINE_Z - PERFECT_RANGE,
        JUDGE_LINE_Z + PERFECT_RANGE,
        255, 255, 0);

    // GREAT
    drawZone(JUDGE_LINE_Z - GREAT_RANGE,
        JUDGE_LINE_Z - PERFECT_RANGE,
        255, 80, 80);
    drawZone(JUDGE_LINE_Z + PERFECT_RANGE,
        JUDGE_LINE_Z + GREAT_RANGE,
        255, 80, 80);

    // GOOD
    drawZone(JUDGE_LINE_Z - GOOD_RANGE,
        JUDGE_LINE_Z - GREAT_RANGE,
        80, 255, 120);
    drawZone(JUDGE_LINE_Z + GREAT_RANGE,
        JUDGE_LINE_Z + GOOD_RANGE,
        80, 255, 120);

    // MISS
    drawZone(JUDGE_LINE_Z - GOOD_RANGE - 60,
        JUDGE_LINE_Z - GOOD_RANGE,
        120, 120, 120);
    drawZone(JUDGE_LINE_Z + GOOD_RANGE,
        JUDGE_LINE_Z + GOOD_RANGE + 60,
        120, 120, 120);
}

void GameScene::DrawLaneFlash3D()
{
    for (int i = 0; i < 4; i++)   // 各レーンの発光を描画
    {
        if (laneFlash[i] <= 0) continue; // 発光していないならスキップ

        float x1 = (i * laneWidth) - (laneWidth * 2);
        float x2 = x1 + laneWidth;

        int alpha = laneFlash[i] * 20; // フェードアウト

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

        float z = JUDGE_LINE_Z + 5; // 判定ラインの少し奥

        setV(0, x1, 0, z);
        setV(1, x2, 0, z);
        setV(2, x2, 0, z + FLASH_LINE_Z);

        setV(3, x1, 0, z);
        setV(4, x2, 0, z + FLASH_LINE_Z);
        setV(5, x1, 0, z + FLASH_LINE_Z);

        DrawPolygon3D(v, 2, LIGHT_TEX, TRUE);
    }
}

void GameScene::DrawJudgeText()
{
    if (judgeDisplayTimer <= 0) return; // 表示時間が終わっていたら描画しない
    if (judgeLane < 0) return;          // 判定レーンが無ければ描画しない

    const char* text = "";
    int baseColor = GetColor(255, 255, 255);

    switch (lastJudge)   // 判定種別に応じて文字と色を設定
    {
    case 0:
        text = "PERFECT";
        baseColor = GetColor(255, 255, 0);
        break;
    case 1:
        text = "GREAT";
        baseColor = GetColor(255, 80, 80);
        break;
    case 2:
        text = "GOOD";
        baseColor = GetColor(80, 255, 120);
        break;
    case 3:
        text = "MISS";
        baseColor = GetColor(0, 0, 0);
        break;
    }

    // レーンのX位置を計算
    float centerX = 0;

    // 3D → 2D 座標変換
    VECTOR world = VGet(centerX, 30, judgeZ);
    VECTOR screen = ConvWorldPosToScreenPos(world);

    // 静止時間が残っている間は動かさない
    if (judgeHoldTimer > 0)
    {
        judgeHoldTimer--;
    }
    else
    {
        judgeTextY -= 1.0f;   // 上方向へ移動
        judgeAlpha -= 8;      // 徐々に透明に
        if (judgeAlpha < 0) judgeAlpha = 0;
    }

    int r, g, b;
    GetColor2(baseColor, &r, &g, &b);
    int color = GetColor(r, g, b);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, judgeAlpha);

    DrawString(
        (int)screen.x - 40,
        (int)(JUDGE_BASE_Y + judgeTextY),
        text,
        color
    );

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::Draw()
{
    DrawBox(0, 0, 1280, 720, GetColor(20, 20, 20), TRUE); // 背景

    VECTOR eye = VGet(0.0f, 200.0f, -300.0f);
    VECTOR target = VGet(0.0f, 50.0f, -100.0f);

    SetCameraPositionAndTarget_UpVecY(eye, target); // カメラ設定

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

    // カウントダウン表示
    DrawCountDown();

    if (!started) return; // 開始前はノーツを描画しない

    // ノーツ描画
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    for (auto& n : notes)
    {
        // ===== 通常ノーツ & ロングノーツ始点 =====
        if (n.type == 1 || (n.type == 2 && n.time == n.endTime))
        {
            float dt = n.time - currentTime;
            float z = dt * scrollSpeed + NOTE_OFFSET_Z;

            if (z < LANE_FRONT || z > LANE_DEPTH) continue;

            float xCenter = -2 * laneWidth + laneWidth * n.lane + laneWidth / 2;
            float halfWidth = laneWidth / 3;
            float x1 = xCenter - halfWidth;
            float x2 = xCenter + halfWidth;

            if (!n.judged)
            {
                DrawQuad3D(
                    VGet(x1, 0.1f, z),
                    VGet(x2, 0.1f, z),
                    VGet(x2, 0.1f, z + noteHeight),
                    VGet(x1, 0.1f, z + noteHeight),
                    NOTE_TEX
                );
            }
            continue;
        }
        // ===== ロングノーツ帯 =====
        if (n.type == 2 && n.endTime > n.time)
        {
            float dtStart = n.time - currentTime;
            float dtEnd = n.endTime - currentTime;

            float zStart = dtStart * scrollSpeed + NOTE_OFFSET_Z;
            float zEnd = dtEnd * scrollSpeed + NOTE_OFFSET_Z;

            if ((zStart < LANE_FRONT && zEnd < LANE_FRONT) ||
                (zStart > LANE_DEPTH && zEnd > LANE_DEPTH))
                continue;

            float xCenter = -2 * laneWidth + laneWidth * n.lane + laneWidth / 2;
            float halfWidth = laneWidth / 3;
            float x1 = xCenter - halfWidth;
            float x2 = xCenter + halfWidth;

            DrawQuad3D(
                VGet(x1, 0.1f, zStart),
                VGet(x2, 0.1f, zStart),
                VGet(x2, 0.1f, zEnd),
                VGet(x1, 0.1f, zEnd),
                LONG_NOTE_TEX
            );
        }
    }


    // レーン発光
    DrawLaneFlash3D();

    DrawJudgeZone();

    // 判定文字
    DrawJudgeText();

    // UI
    DrawScore();
    DrawCombo();
    DrawSongInfo();
}

void GameScene::DrawCombo()
{
    if (combo <= 0) return; // コンボが0なら表示しない

    char buf[32];
    sprintf_s(buf, "%d", combo);

    DrawString(1100, 300, buf, GetColor(255, 255, 255)); // コンボ表示
}

void GameScene::DrawScore()
{
    char buf[32];
    sprintf_s(buf, "%07d", score); // 7桁ゼロ埋め

    DrawString(30, 20, buf, GetColor(255, 255, 255)); // スコア表示
}

void GameScene::DrawSongInfo()
{
    std::string sjis = Utf8ToSjis(songName);
    DrawString(30, 60, sjis.c_str(), GetColor(200, 200, 200)); // 曲名表示
}

std::string GameScene::Utf8ToSjis(const std::string& utf8)
{
    // UTF-8 → UTF-16 変換
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // UTF-16 → Shift-JIS 変換
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string sjis(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, nullptr, nullptr);

    return sjis;
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

    // 四角形を三角形2枚に分割して設定
    v[0].pos = p1; v[1].pos = p2; v[2].pos = p3;
    v[3].pos = p1; v[4].pos = p3; v[5].pos = p4;

    for (int i = 0; i < 6; i++)   // 各頂点の属性を設定
    {
        v[i].norm = VGet(0.0f, 1.0f, 0.0f);          // 法線
        v[i].dif = GetColorU8(255, 255, 255, 255);   // 色
        v[i].spc = GetColorU8(0, 0, 0, 0);           // スペキュラ

        // UV座標設定（テクスチャ貼り付け）
        v[i].u = (i == 1 || i == 2 || i == 4) ? 1.0f : 0.0f;
        v[i].v = (i == 2 || i == 3 || i == 4) ? 1.0f : 0.0f;
    }

    DrawPolygon3D(v, 2, tex, TRUE); // 四角形描画
}
