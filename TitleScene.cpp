#include "TitleScene.h"
#include <cmath>

TitleScene::TitleScene()
{
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

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
    // ENTER で次へ
    if (CheckHitKey(KEY_INPUT_RETURN))
    {
        goNext = true;
    }

    // 粒子更新
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].y += particles[i].speed;
        if (particles[i].y > 720)
        {
            particles[i].y = 0;
            particles[i].x = rand() % 1280;
        }
    }
}

void TitleScene::Draw()
{
    ClearDrawScreen();

    // ============================
    // カメラを派手に動かす
    // ============================
    float t = GetNowCount() * 0.002f;

    float camX = 40.0f * cosf(t);
    float camZ = 40.0f * sinf(t);
    float camY = 20.0f + 5.0f * sinf(t * 2);

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

    const char* title = "MY RHYTHM GAME";
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
    DrawString(500, 600, "PRESS ENTER", GetColor(255, 255, 255));
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
