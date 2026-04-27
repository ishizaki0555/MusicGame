// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// TitleScene.h
// タイトル画面の機能を提供します。
// 
//========================================

#pragma once
#include <DxLib.h>

// タイトルシーンのクラス
class TitleScene
{
public:

    // @brief コンストラクタ
    TitleScene();

    // @brief デストラクタ
    ~TitleScene();

    // @brief 更新します
    void Update();

    // @brief 描画します
    void Draw();

    bool goNext = false;    // ENTERで次へ
    int bgm = -1;           // BGMハンドル

private:

    // UIや演出の管理変数
    static const int PARTICLE_MAX = 60;     // 背景に舞うパーティクルの最大数
    int menuIndex = 0;                      // メニューの選択インデックス (0:START, 1:SETTING, 2:EXIT)
    int titleFontLarge = -1;                // ロゴ描画用の大きなフォントハンドル
    int titleFontSmall = -1;                // メニュー描画用の小さなフォントハンドル
    
    // 背景のキラキラしたパーティクル(粒子)の構造体
    struct Particle {
        float x;        // X座標
        float y;        // Y座標
        float speed;    // 落下速度 (Y軸方向への移動量)
    };
    Particle particles[PARTICLE_MAX];       // パーティクルの配列

    float logoScale = 1.0f;                 // タイトルロゴをアニメーションさせるための拡大率
	float bandOffset = 0.0f;			    // 背景の装飾バンドのオフセット(現在未使用)
    int pressAlpha = 255;                   // "PressEnter"テキストを点滅させるためのアルファ値(透明度)
};
