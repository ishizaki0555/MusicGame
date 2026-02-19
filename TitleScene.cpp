#include "TitleScene.h"
#include <cmath>

TitleScene::TitleScene()
{
    // 3D初期化
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
    // ENTERで次のシーンへ
    if (CheckHitKey(KEY_INPUT_RETURN))
    {
        goNext = true;
    }
}

void TitleScene::Draw()
{
    // 背景クリア
    ClearDrawScreen();

    // カメラをゆっくり回す
    static float angle = 0.0f;
    angle += 0.01f;

    float camX = 30.0f * cosf(angle);
    float camZ = 30.0f * sinf(angle);

    SetCameraPositionAndTarget_UpVecY(
        VGet(camX, 20.0f, camZ),
        VGet(0.0f, 10.0f, 0.0f)
    );

    // タイトル文字を3D空間に描く
    

    // 2Dで「PRESS ENTER」を描く
    DrawString(500, 600, "PRESS ENTER", GetColor(255,255,255));
}