// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// TitleScene.cpp
// タイトル画面の機能を提供します。
// 
//========================================

#include "TitleScene.h"
#include <cmath>

// @brief コンストラクタ
TitleScene::TitleScene()
{
    // 3D設定
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // BGM 読み込み
    bgm = LoadSoundMem("BGM/Title.mp3");
    PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);

    // 粒子初期化
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].x = rand() % 1280;
        particles[i].y = rand() % 720;
        particles[i].speed = 0.5f + (rand() % 100) / 100.0f;
    }
}

// @brief デストラクタ
TitleScene::~TitleScene(){}

// @brief 更新します
void TitleScene::Update()
{
    // ENTER で次のシーンへ
    if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
    {
        goNext = true;
    }

    // 粒子更新
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        // Y座標を速度分だけ増加させる
        particles[i].y += particles[i].speed;
        if (particles[i].y > 720)
        {
            // 画面下に出たら上に戻す
            particles[i].y = 0;
            particles[i].x = rand() % 1280;
        }
    }
}

// @brief 描画します
void TitleScene::Draw()
{
    ClearDrawScreen();

    // ============================
    // カメラを派手に動かす
    // ============================
    float t = GetNowCount() * 0.002f;

    // カメラを円軌道で動かす
    float camX = 40.0f * cosf(t);
    float camZ = 40.0f * sinf(t);
    float camY = 20.0f + 5.0f * sinf(t * 2);

    // カメラ位置と注視点を設定
    SetCameraPositionAndTarget_UpVecY(
        VGet(camX, camY, camZ),
        VGet(0.0f, 10.0f, 0.0f)
    );

    // ============================
    // キラキラ粒子
    // ============================
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        DrawPixel(particles[i].x, particles[i].y, GetColor(200, 200, 255));
    }

    // ============================
    // タイトルロゴ（拡大縮小）
    // ============================
    float scale = 1.0f + 0.05f * sinf(GetNowCount() * 0.005f);

    // タイトルテキストの幅を取得して中央に配置
    const char* title = "MUSIC GAME!!";
    int w = GetDrawStringWidth(title, strlen(title));
    int x = 640 - (int)(w * scale / 2);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawExtendString(x, 150, scale, scale, title, GetColor(255, 255, 255));
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ============================
    // PRESS ENTER（点滅）
    // ============================
    int alpha = 128 + 127 * sinf(GetNowCount() * 0.01f);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DrawString(500, 600, "PRESS ENTER OR SPACE", GetColor(255, 255, 255));
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
