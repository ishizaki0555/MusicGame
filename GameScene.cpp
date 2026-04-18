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
// @param autoPlay  オートプレイフラグ
GameScene::GameScene(const NotesData& notesData, int banner, bool autoPlay)
    : LANE_TEX(LoadGraph("Texture/LaneTexture.png"))
    , LIGHT_TEX(LoadGraph("Texture/LightTexture.png"))
    , LINE_TEX(LoadGraph("Texture/LineTexture.png"))
    , NOTE_TEX(LoadGraph("Texture/NoteTexture.png"))
    , LONG_NOTE_TEX(LoadGraph("Texture/LongNoteTexture.png"))
{
    m_isAutoPlay = autoPlay;
    m_notesData = notesData;                 // ノーツデータ全体を保存
    notes = notesData.judgeNotes;            // ノーツ一覧をコピー
    songName = notesData.title;              // 曲名を保存
    bannerHandle = banner;                   // バナーハンドルを保存

    // 楽曲読み込み
    musicHandle = LoadSoundMem(notesData.musicPath.c_str());

    // 効果音の読み込み
    hitSE = LoadSoundMem("Sounds/hit.mp3");
    rongSE = LoadSoundMem("Sounds/rong.mp3");

    // スコアの最大値をノーツ数から計算
    int noteNum = static_cast<int>(notes.size());
    maxScore = noteNum * 5;

    // longBodies 生成箇所
    for (int i = 0; i < notes.size(); i++)
    {
        // ロングノーツの開始ノーツを見つける
        if (notes[i].type == 2) // start
        {
            // 同じレーンでType3のノーツを探す
            for (int j = i + 1; j < notes.size(); j++)
            {
                // 譜面によってはロングノーツの終点が同じレーンでない場合もあるため、レーンもチェックする
                if (notes[j].type == 3 && notes[j].lane == notes[i].lane)
                {
                    LongBody body;
                    body.lane = notes[i].lane;
                    body.startTime = notes[i].time;
                    body.endTime = notes[j].time;
                    body.startBeat = notes[i].beat;
                    body.endBeat = notes[j].beat;
                    longBodies.push_back(body);
                    break;
                }
            }
        }
    }

    // ノーツ消費フラグを初期化
    noteConsumed.resize(notes.size(), 0);
    for (int i = 0; i < 6; i++) nextNoteIndex[i] = 0;

    // ポーズメニュー用フォントの読み込み
    pauseFontLarge = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 60, 3);
    pauseFontSmall = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 40, 3);
    fontMusicName = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 36, 3);
    judgeFont = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 48, 3);
}

// @brief 判定ロジック
// @param diffMs 判定ラインとの差（ミリ秒）
// @return 判定結果（0=PERFECT,1=GREAT,2=GOOD,3=MISS）
int GameScene::Judge(int diffMs)
{
    // 差の絶対値を撮る
    diffMs = abs(diffMs);

    // 判定範囲に応じて結果を返す
    if (diffMs <= PERFECT_RANGE) return 0;   // PERFECT 判定
    if (diffMs <= GREAT_RANGE)   return 1;   // GREAT 判定
    if (diffMs <= GOOD_RANGE)    return 2;   // GOOD 判定
    return 3;                                // MISS 判定
}

// @brief 判定文字の追加
// @param lane レーン
// @param judgeType 判定の種類（0=PERFECT,1=GREAT,2=GOOD,3=MISS）
// @param noteIndex 判定したノーツのインデックス
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

// @brief 更新します
void GameScene::Update()
{
    // ============================
    // ポーズ関連処理
    // ============================
    bool nowEsc = (CheckHitKey(KEY_INPUT_ESCAPE) != 0);
    if (nowEsc && !prevEscapeKey) {
        if (!isPaused && !isResuming && started && !finished) {
            isPaused = true;
            pausedTimeMs = GetSoundCurrentTime(musicHandle);
            StopSoundMem(musicHandle);
            pauseMenuIndex = 0;
            prevUpKey = (CheckHitKey(KEY_INPUT_UP) != 0) || (CheckHitKey(KEY_INPUT_W) != 0);
            prevDownKey = (CheckHitKey(KEY_INPUT_DOWN) != 0) || (CheckHitKey(KEY_INPUT_S) != 0);
            prevReturnKey = (CheckHitKey(KEY_INPUT_RETURN) != 0) || (CheckHitKey(KEY_INPUT_SPACE) != 0);
        } else if (isPaused) {
            isPaused = false;
            isResuming = true;
            resumeCountDown = 180;
        }
    }
    prevEscapeKey = nowEsc;

    if (isPaused) {
        bool nowUp = (CheckHitKey(KEY_INPUT_UP) != 0) || (CheckHitKey(KEY_INPUT_W) != 0);
        bool nowDown = (CheckHitKey(KEY_INPUT_DOWN) != 0) || (CheckHitKey(KEY_INPUT_S) != 0);
        bool nowReturn = (CheckHitKey(KEY_INPUT_RETURN) != 0) || (CheckHitKey(KEY_INPUT_SPACE) != 0);

        if (nowUp && !prevUpKey) pauseMenuIndex = (pauseMenuIndex - 1 + 3) % 3;
        if (nowDown && !prevDownKey) pauseMenuIndex = (pauseMenuIndex + 1) % 3;

        if (nowReturn && !prevReturnKey) {
            if (pauseMenuIndex == 0) {
                isPaused = false;
                isResuming = true;
                resumeCountDown = 180;
            } else if (pauseMenuIndex == 1) {
                isRetry = true;
            } else if (pauseMenuIndex == 2) {
                isRetire = true;
            }
        }
        prevUpKey = nowUp;
        prevDownKey = nowDown;
        prevReturnKey = nowReturn;
        return; // ポーズ中はこれ以上ゲームを更新しない
    }

    if (isResuming) {
        resumeCountDown--;
        if (resumeCountDown <= 0) {
            isResuming = false;
            SetSoundCurrentTime(pausedTimeMs, musicHandle);
            PlaySoundMem(musicHandle, DX_PLAYTYPE_BACK);
        }
        return; // カウントダウン中も更新しない
    }

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
    // レーン入力処理（A S D J K L）
    // ============================
    int keys[6] = {
        KEY_INPUT_A,
        KEY_INPUT_S,
        KEY_INPUT_D,
        KEY_INPUT_J,
        KEY_INPUT_K,
        KEY_INPUT_L
    };

    // 各レーンのキー入力を確認
    for (int i = 0; i < 6; i++)
    {
        nowKey[i] = (CheckHitKey(keys[i]) != 0);    // 現在のキーを記録
        keyDown[i] = (nowKey[i] && !prevKey[i]);    // キーを押した瞬間
        keyUp[i] = (!nowKey[i] && prevKey[i]);      // キーを離した瞬間
        prevKey[i] = nowKey[i];                     // 前のキーを更新

        // オートプレイ時のキー入力無効化
        if (m_isAutoPlay)
        {
            nowKey[i] = false;
            keyDown[i] = false;
            keyUp[i] = false;
        }

        // キーが押されたら
        if (CheckHitKey(keys[i]) && !m_isAutoPlay)
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
    for (int lane = 0; lane < 6; lane++)
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

        // オートプレイでの自動判定
        if (m_isAutoPlay && currentTime >= note.time)
        {
            if (note.type == 1 || note.type == 2)
                keyDown[lane] = true;
            else if (note.type == 3)
                keyUp[lane] = true;
                
            laneFlash[lane] = 10;
        }

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
                else if (holding[lane])
                {
                    // 早すぎる離上はMISS判定
                    holding[lane] = false;
                    AddJudgeText(lane, 3, idx);
                    missCount++;
                    combo = 0;
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

// @brief カウントダウン描画
void GameScene::DrawCountDown()
{
    if (started) return;   // 開始後は表示しない

    int sec = countDown / 60 + 1; // 残り秒数を計算

    char buf[32];
    sprintf_s(buf, "%d", sec);

    int w = GetDrawStringWidthToHandle(buf, strlen(buf), pauseFontLarge);
    DrawStringToHandle(640 - w / 2, 300, buf, GetColor(255, 255, 255), pauseFontLarge); // カウントダウン表示
}

// @brief レーン発光描画
void GameScene::DrawLaneFlash3D()
{
    for (int i = 0; i < 6; i++)   // 各レーンの発光を描画
    {
        if (laneFlash[i] <= 0) continue; // 発光していないならスキップ

        float x1 = (i * laneWidth) - (laneWidth * 3);
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

// @brief 判定文字描画
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

        int w = GetDrawStringWidthToHandle(text, strlen(text), judgeFont);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, jt.alpha);
        DrawStringToHandle((int)jt.x - w / 2, (int)jt.y, text, baseColor, judgeFont);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // タイマー切れのテキストを削除
    judgeTexts.erase(
        std::remove_if(judgeTexts.begin(), judgeTexts.end(),
            [](const JudgeTextInfo& jt) { return jt.timer <= 0; }),
        judgeTexts.end()
    );
}

// @brief 描画します
void GameScene::Draw()
{
    DrawBox(0, 0, 1280, 720, GetColor(20, 20, 20), TRUE); // 背景

    VECTOR eye = VGet(0.0f, 350.0f, -150.0f);
    VECTOR target = VGet(0.0f, 50.0f, 150.0f);

    SetCameraPositionAndTarget_UpVecY(eye, target); // カメラ設定

    // レーン描画
    for (int i = 0; i < 6; i++)
    {
        float x = (i * laneWidth) - (laneWidth * 3);

        DrawQuad3D(
            VGet(x, LANE_BASE_Y, LANE_FRONT),
            VGet(x + laneWidth, LANE_BASE_Y, LANE_FRONT),
            VGet(x + laneWidth, LANE_BASE_Y, LANE_DEPTH),
            VGet(x, LANE_BASE_Y, LANE_DEPTH),
            LANE_TEX
        );
    }

    // ============================
    // レーン区切りライン描画
    // ============================
    for (int i = 0; i <= 6; i++)
    {
        float x = (i * laneWidth) - (laneWidth * 3);

        // ラインの太さ（細い板）
        float lineThickness = 2.0f;

        DrawQuad3D(
            VGet(x - lineThickness / 2, LANE_BASE_Y, LANE_FRONT),
            VGet(x + lineThickness / 2, LANE_BASE_Y, LANE_FRONT),
            VGet(x + lineThickness / 2, LANE_BASE_Y, LANE_DEPTH),
            VGet(x - lineThickness / 2, LANE_BASE_Y, LANE_DEPTH),
            LINE_TEX   // 既存のラインテクスチャを使用
        );
    }

    // 判定ライン
    DrawQuad3D(
        VGet(-300, LANE_BASE_Y, JUDGE_LINE_Z),
        VGet(300, LANE_BASE_Y, JUDGE_LINE_Z),
        VGet(300, LANE_BASE_Y + 5, JUDGE_LINE_Z),
        VGet(-300, LANE_BASE_Y + 5, JUDGE_LINE_Z),
        LINE_TEX
    );

    // ============================
    // キーガイド描画
    // ============================
    const char* keyNames[6] = { "A", "S", "D", "J", "K", "L" };

    // レーンより少し広い間隔で配置する
    float guideSpacing = laneWidth * 1.3f;
    float startX = -guideSpacing * 2.5f;     // 左端の開始位置

    for (int i = 0; i < 6; i++)
    {
        float x = startX + guideSpacing * i;
        VECTOR screenPos = ConvWorldPosToScreenPos(VGet(x, 0, JUDGE_LINE_Z + 100));

        // 画面下部にキーガイドを描画
        int w = GetDrawStringWidthToHandle(keyNames[i], strlen(keyNames[i]), pauseFontSmall);
        DrawStringToHandle(
            (int)screenPos.x - w / 2,
            670,
            keyNames[i],
            GetColor(255, 255, 255),
            pauseFontSmall
        );
    }

    // カウントダウン表示
    DrawCountDown();

    // レーン発光
    DrawLaneFlash3D();

    // 判定文字
    DrawJudgeText();

    // UI
    DrawScore();
    DrawCombo();
    DrawSongInfo();

    if (!started) return; // 開始前はノーツを描画しない

    // currentTime を取得した直後に longBodies を先に描画するブロックを追加
    double currentTime = GetSoundCurrentTime(musicHandle) / 1000.0;
    float currentBeat = m_notesData.GetBeatFromTime(currentTime);
    
    // スクロール速度の基準をビートに合わせる
    float baseBpm = m_notesData.bpm > 0 ? m_notesData.bpm : 120.0f;
    float scrollSpeedPerBeat = scrollSpeed * (60.0f / baseBpm);

    // LongBody（帯）を先に描画
    for (auto& b : longBodies)
    {
        float dBeatStart = b.startBeat - currentBeat;
        float dBeatEnd = b.endBeat - currentBeat;

        float zStart = dBeatStart * scrollSpeedPerBeat;
        float zEnd = dBeatEnd * scrollSpeedPerBeat;

        // ホールド中は帯の終点を判定ラインに固定（判定ラインより奥側を描かない）
        if (b.lane >= 0 && b.lane < 6 && holding[b.lane] && zStart <= JUDGE_LINE_Z)
            zStart = JUDGE_LINE_Z;

        // 終点が始点より手前に来てしまったら（裏返るのを防ぐ）、描画しない
        if (zEnd <= zStart)
            continue;

        // 画面外チェック（帯が完全に画面外なら描画しない）
        if ((zStart < LANE_FRONT && zEnd < LANE_FRONT) || (zStart> LANE_DEPTH && zEnd > LANE_DEPTH))
            continue;

        float xCenter = -3 * laneWidth + laneWidth * b.lane + laneWidth / 2;
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
        // ノーツが判定されていたら描画しない
        if (noteConsumed[i]) continue;

        // ノーツの情報を取得
        JudgeNote& n = notes[i];

        // 通常ノーツ & ロング開始ノーツ（始点）
        if (n.type == 1 || n.type == 2)
        {
            // ノーツの位置を計算
            float dBeat = n.beat - currentBeat;
            float z = dBeat * scrollSpeedPerBeat;

            // 画面外チェック（ノーツが画面外なら描画しない）
            if (z < LANE_FRONT || z > LANE_DEPTH) continue;

            // レーンの中央の位置を計算
            float xCenter = -3 * laneWidth + laneWidth * n.lane + laneWidth / 2;
            float halfWidth = laneWidth / 3;

            // ノーツの描画
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
            float dBeat = n.beat - currentBeat;
            float z = dBeat * scrollSpeedPerBeat;

            if (z < LANE_FRONT || z > LANE_DEPTH) continue;

            float xCenter = -3 * laneWidth + laneWidth * n.lane + laneWidth / 2;
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

    // ============================
    // ポーズ中のUI描画
    // ============================
    if (isResuming) {
        int sec = resumeCountDown / 60 + 1;
        char buf[32];
        sprintf_s(buf, "%d", sec);
        DrawStringToHandle(640 - 15, 300, buf, GetColor(255, 255, 255), pauseFontLarge);
    }
    else if (isPaused) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        int centerX = 640;
        const char* title = "PAUSE";
        int tWidth = GetDrawStringWidthToHandle(title, strlen(title), pauseFontLarge);
        DrawStringToHandle(centerX - tWidth / 2, 200, title, GetColor(255, 255, 255), pauseFontLarge);

        const char* menuItems[3] = { "Cancel", "Retry", "Retire" };
        for (int i = 0; i < 3; i++) {
            int mWidth = GetDrawStringWidthToHandle(menuItems[i], strlen(menuItems[i]), pauseFontSmall);
            int color = (i == pauseMenuIndex) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
            DrawStringToHandle(centerX - mWidth / 2, 350 + i * 80, menuItems[i], color, pauseFontSmall);
        }
    }
}

// @brief コンボ数を描画します
void GameScene::DrawCombo()
{
    if (combo <= 0) return; // コンボが0なら表示しない

    char buf[32];
    sprintf_s(buf, "%d", combo);

    int w = GetDrawStringWidthToHandle(buf, strlen(buf), judgeFont);
    DrawStringToHandle(1200 - w, 300, buf, GetColor(255, 255, 255), judgeFont); // コンボ表示
}

// @brief スコアを描画します
void GameScene::DrawScore()
{
    char buf[32];
    sprintf_s(buf, "%07d", score); // 7桁ゼロ埋め

    DrawStringToHandle(30, 20, buf, GetColor(255, 255, 255), fontMusicName); // スコア表示
}

// @brief 曲情報を描画します
void GameScene::DrawSongInfo()
{
    std::string sjis = Utf8ToSjis(songName);
    // バナー描画
    DrawExtendGraph(20, 60, 320, 200, GetBannerHandle(), TRUE);

    // 曲名表示
    DrawStringToHandle(30, 220, sjis.c_str(), GetColor(200, 200, 200), fontMusicName);
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

// @brief 3D四角形描画
// @param p1 頂点１
// @param p2 頂点２
// @param p3 頂点３
// @param p4 頂点４
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
