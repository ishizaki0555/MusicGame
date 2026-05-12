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

    // フォント読み込み
    titleFontLarge = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 80, 3);
    titleFontSmall = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 32, 3);


    // 粒子初期化
    // 全てのパーティクルの初期位置と速度をランダムに設定する
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
    static int upTimer = 0;
    static int downTimer = 0;

    const int REPEAT_START = 120;       // 長押し開始までの待ち時間
    const int REPEAT_INTERVAL = 25;      // 長押し時の繰り返し間隔

    int up = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
    int down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);

    // 上移動
    // 上キーが押されている場合の処理
    if (up)
    {
        // 最初に押した瞬間、または長押しして一定時間が経過した後に一定間隔で処理を行う
        if (upTimer == 0 || (upTimer > REPEAT_START && upTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 2) % 3;  // 上へ
        }
        upTimer++;
    }
    else
    {
        upTimer = 0;
    }

    // 下移動
    // 下キーが押されている場合の処理
    if (down)
    {
        // 最初に押した瞬間、または長押しして一定時間が経過した後に一定間隔で処理を行う
        if (downTimer == 0 || (downTimer > REPEAT_START && downTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 1) % 3;  // 下へ
        }
        downTimer++;
    }
    else
    {
        downTimer = 0;
    }

    // 決定
    // 決定キー（エンターまたはスペース）が押された場合の処理
    if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
    {
        // 選択されているメニュー項目に応じて処理を分岐する
        if (menuIndex == 0)
        {
            goNext = true; // Start
        }
        else if (menuIndex == 1)
        {
            
        }
        else if (menuIndex == 2)
        {
            DxLib_End();
            exit(0);
        }
    }

    // パーティクル更新（既存）
    // 全てのパーティクルの位置を更新する
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].y += particles[i].speed;
        // 画面下端を越えたら上に戻す
        if (particles[i].y > 720)
        {
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
    // メニュー描画（マーカー付き）
    // ============================
    const char* items[3] = { "START", "SETTING", "EXIT" };

    // 3つのメニュー項目を順番にループして描画する
    for (int i = 0; i < 3; i++)
    {
        int y = 350 + i * 60;

        // 選択中は黄色、それ以外は白
        int color = (i == menuIndex)
            ? GetColor(255, 255, 0)
            : GetColor(255, 255, 255);

        // マーカーを描画
        if (i == menuIndex)
        {
            DrawStringToHandle(540, y, ">", color, titleFontSmall);
        }

        // メニュー項目
        DrawStringToHandle(580, y, items[i], color, titleFontSmall);
    }


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

    // タイトルテキストの幅を取得して、画面中央(X=640)に配置する
    const char* title = "MUSIC GAME!!";
    int w = GetDrawStringWidthToHandle(title, strlen(title), titleFontLarge);
    // スケールを考慮して文字列の中央のX座標を計算する
    int x = 640 - (int)(w * scale / 2);
    
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawExtendStringToHandle(x, 150, scale, scale, title, GetColor(255, 255, 255), titleFontLarge);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ============================
    // PRESS ENTER（点滅）
    // ============================
    // サイン波を使って、時間経過とともにアルファ値（透明度）を0~255の間で滑らかに変化させる
    int alpha = 128 + 127 * sinf(GetNowCount() * 0.01f);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    int pw = GetDrawStringWidthToHandle("PRESS ENTER OR SPACE", strlen("PRESS ENTER OR SPACE"), titleFontSmall);
    DrawStringToHandle(640 - pw / 2, 600, "PRESS ENTER OR SPACE", GetColor(255, 255, 255), titleFontSmall);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
