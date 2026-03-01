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
    notes = notesData.judgeNotes;            // ノーツ一覧をコピー
    songName = notesData.title;              // 曲名を保存
    bannerHandle = banner;                   // バナーハンドルを保存

    musicHandle = LoadSoundMem(notesData.musicPath.c_str()); // 楽曲読み込み

    // 効果音の読み込み
    hitSE = LoadSoundMem("Sounds/hit.mp3");
    rongSE = LoadSoundMem("Sounds/rong.mp3");

    // スコアの最大値をノーツ数から計算
    int noteNum = static_cast<int>(notes.size());
    maxScore = noteNum * 5;

    // longBodies 生成箇所
    for (int i = 0; i < notes.size(); i++)
    {
        if (notes[i].type == 2) // start
        {
            for (int j = i + 1; j < notes.size(); j++)
            {
                if (notes[j].type == 3 && notes[j].lane == notes[i].lane)
                {
                    LongBody body;
                    // ここで強制正規化（譜面が 1..4 の場合に備える）
                    int lane = notes[i].lane;
                    if (lane > 3) lane = lane - 1; // 1..4 -> 0..3 の補正
                    body.lane = lane;
                    body.startTime = notes[i].time;
                    body.endTime = notes[j].time;
                    longBodies.push_back(body);
                    break;
                }
            }
        }
    }

    // ノーツ消費フラグを初期化
    noteConsumed.resize(notes.size(), 0);
    for (int i = 0; i < 4; i++) nextNoteIndex[i] = false;
}

// @brief 判定ロジック
// @param diffMs 判定ラインとの差（ミリ秒）
// @return 判定結果（0=PERFECT,1=GREAT,2=GOOD,3=MISS）
int GameScene::Judge(int diffMs)
{
    diffMs = abs(diffMs);

    if (diffMs <= PERFECT_RANGE) return 0;   // PERFECT 判定
    if (diffMs <= GREAT_RANGE)   return 1;   // GREAT 判定
    if (diffMs <= GOOD_RANGE)    return 2;   // GOOD 判定
    return 3;                                // MISS 判定
}

void GameScene::AddJudgeText(int lane, int result, int noteIndex)
{
    JudgeTextInfo jt;

    jt.x = 640;
    jt.y = JUDGE_BASE_Y;
	jt.alpha = 255;
    jt.timer = 30;
    jt.hold = 10;
    jt.judgeType = result;

    // スコア計算
    switch (result)
    {
    case 0: perfectCount++; ratioScore += 5; combo++; break;
    case 1: greatCount++;   ratioScore += 3; combo++; break;
    case 2: goodCount++;    ratioScore += 1; combo = 0; break;
    case 3: missCount++;    combo = 0; break;
    }

    // 現在のノーツを判定済みにして描画を終了する
    if (noteIndex >= 0 && noteIndex < static_cast<int>(noteConsumed.size()))
        noteConsumed[noteIndex] = 1;

	judgeTexts.push_back(jt);
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
    // 判定処理
    // ============================
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    // レーンごとに次の未処理ノーツを確認して判定
    for (int lane = 0; lane < 4; lane++)
    {
        int idx = nextNoteIndex[lane];
        while (idx < static_cast<int>(notes.size()))
        {
            if (notes[idx].lane == lane && !noteConsumed[idx]) break;
            ++idx;
        }
		// 次のノーツが存在する場合、判定を行う
        nextNoteIndex[lane] = idx;

        if (idx >= static_cast<int>(notes.size())) continue; // そのレーンに未処理ノーツなし

        JudgeNote& note = notes[idx];
        float timeLag = fabs(currentTime - note.time);

        // 通常ノーツかロングノーツの始点の場合は押す動作で判定
        if (notes[idx].type == 1 || notes[idx].type == 2)
        {
            // キーが押された瞬間に判定
            if (keyDown[lane])
            {
                if (timeLag <= PERFECT_RANGE)
                {
                    if (note.type == 2) holding[lane] = true;
                    if (note.type == 3) holding[lane] = false;
					PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 0, idx);
                }
                else if (timeLag <= GREAT_RANGE)
                {
                    if (note.type == 2) holding[lane] = true;
                    if (note.type == 3) holding[lane] = false;
                    PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 1, idx);
                }
                else if (timeLag <= GOOD_RANGE)
                {
                    if (note.type == 2) holding[lane] = true;
                    if (note.type == 3) holding[lane] = false;
                    PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 2, idx);
                }
            }
            else
            {
                // 見逃し判定
                if (currentTime > note.time + 0.2f)
                {
                    if (note.type == 2) holding[lane] = false;
                    if (note.type == 3) holding[lane] = false;
                    AddJudgeText(lane, 3, idx);
                    missCount++;
                    combo = 0;
                }
            }
        }
        // ロングノーツの終点の場合は離す動作で判定
        else if (notes[idx].type == 3)
        {
            // キーが離れた瞬間に判定
            if (keyUp[lane])
            {
                if (timeLag <= PERFECT_RANGE)
                {
                    if (note.type == 2) holding[lane] = false;
                    if (note.type == 3) holding[lane] = false;
                    PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 0, idx);
                }
                else if (timeLag <= GREAT_RANGE)
                {
                    if (note.type == 2) holding[lane] = false;
                    if (note.type == 3) holding[lane] = false;
                    PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 1, idx);
                }
                else if (timeLag <= GOOD_RANGE)
                {
                    if (note.type == 2) holding[lane] = false;
                    if (note.type == 3) holding[lane] = false;
                    PlaySoundMem(hitSE, DX_PLAYTYPE_BACK); // ヒット音再生
                    AddJudgeText(lane, 2, idx);
                }
            }
            else
            {
                // 見逃し判定
                if (currentTime > note.time + 0.2f)
                {
                    if (note.type == 2) holding[lane] = false;
                    if (note.type == 3) holding[lane] = false;
                    AddJudgeText(lane, 3, idx);
                    missCount++;
                    combo = 0;
                }
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

void GameScene::DrawCountDown()
{
    if (started) return;   // 開始後は表示しない

    int sec = countDown / 60 + 1; // 残り秒数を計算

    char buf[32];
    sprintf_s(buf, "%d", sec);

    DrawString(600, 300, buf, GetColor(255, 255, 255)); // カウントダウン表示
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
    for (auto& jt : judgeTexts)
    {
        if (jt.timer <= 0) continue;

        const char* text = "";
        int baseColor = GetColor(255, 255, 255);

        switch (jt.judgeType)
        {
        case 0: text = "PERFECT"; baseColor = GetColor(255, 255, 0); break;
        case 1: text = "GREAT";   baseColor = GetColor(255, 80, 80); break;
        case 2: text = "GOOD";    baseColor = GetColor(80, 255, 120); break;
        case 3: text = "MISS";    baseColor = GetColor(0, 0, 0); break;
        }

        // スライド処理
        if (jt.hold > 0)
            jt.hold--;
        else
        {
            jt.y -= 1.0f;
            jt.alpha -= 8;
            if (jt.alpha < 0) jt.alpha = 0;
        }

        jt.timer--;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, jt.alpha);
        DrawString((int)jt.x, (int)jt.y, text, baseColor);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // タイマー切れのテキストを削除
    judgeTexts.erase(
        std::remove_if(judgeTexts.begin(), judgeTexts.end(),
            [](const JudgeTextInfo& jt) { return jt.timer <= 0; }),
        judgeTexts.end()
    );
}

void GameScene::Draw()
{
    DrawBox(0, 0, 1280, 720, GetColor(20, 20, 20), TRUE); // 背景

    VECTOR eye = VGet(0.0f, 220.0f, -110.0f);
    VECTOR target = VGet(0.0f, 50.0f, 100.0f);

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

    // Draw() 内、currentTime を取得した直後に longBodies を先に描画するブロックを追加
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;

    // LongBody（帯）を先に描画（longBodies はコンストラクタで生成済み）
    for (auto& b : longBodies)
    {
        float dtStart = b.startTime - currentTime;
        float dtEnd = b.endTime - currentTime;

        float zStart = dtStart * scrollSpeed;
        float zEnd = dtEnd * scrollSpeed;

        // ホールド中は帯の終点を判定ラインに固定（判定ラインより奥側を描かない）
        if (b.lane >= 0 && b.lane < 4 && holding[b.lane] && zStart <= JUDGE_LINE_Z)
            zStart = JUDGE_LINE_Z;

        // 画面外チェック（帯が完全に画面外なら描画しない）
        if ((zStart < LANE_FRONT && zEnd < LANE_FRONT) || (zStart> LANE_DEPTH && zEnd > LANE_DEPTH))
            continue;

        float xCenter = -2 * laneWidth + laneWidth * b.lane + laneWidth / 2;
        float halfWidth = laneWidth / 3;

        DrawQuad3D(
            VGet(xCenter - halfWidth, 0.1f, zStart),
            VGet(xCenter + halfWidth, 0.1f, zStart),
            VGet(xCenter + halfWidth, 0.1f, zEnd),
            VGet(xCenter - halfWidth, 0.1f, zEnd),
            LONG_NOTE_TEX
        );
    }

    // 既存のノーツ描画（判定用 notes をそのまま描画）
    for (int i = 0; i < notes.size(); i++)
    {
        if (noteConsumed[i]) continue;

        JudgeNote& n = notes[i];

        // 通常ノーツ & ロング開始ノーツ（始点）
        if (n.type == 1 || n.type == 2)
        {
            float dt = n.time - currentTime;
            float z = dt * scrollSpeed;

            if (z < LANE_FRONT || z > LANE_DEPTH) continue;

            float xCenter = -2 * laneWidth + laneWidth * n.lane + laneWidth / 2;
            float halfWidth = laneWidth / 3;

            DrawQuad3D(
                VGet(xCenter - halfWidth, 0.1f, z),
                VGet(xCenter + halfWidth, 0.1f, z),
                VGet(xCenter + halfWidth, 0.1f, z + noteHeight),
                VGet(xCenter - halfWidth, 0.1f, z + noteHeight),
                NOTE_TEX
            );
        }

        // ロング終了ノーツ（終点）
        if (n.type == 3)
        {
            float dt = n.time - currentTime;
            float z = dt * scrollSpeed;

            if (z < LANE_FRONT || z > LANE_DEPTH) continue;

            float xCenter = -2 * laneWidth + laneWidth * n.lane + laneWidth / 2;
            float halfWidth = laneWidth / 3;

            DrawQuad3D(
                VGet(xCenter - halfWidth, 0.1f, z),
                VGet(xCenter + halfWidth, 0.1f, z),
                VGet(xCenter + halfWidth, 0.1f, z + noteHeight),
                VGet(xCenter - halfWidth, 0.1f, z + noteHeight),
                NOTE_TEX
            );
        }
    }

    // レーン発光
    DrawLaneFlash3D();

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
