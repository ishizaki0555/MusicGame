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

    // パーティクル関連
    static const int PARTICLE_MAX = 60;     // パーティクルの最大数
    int menuIndex = 0;                      // メニューの選択インデックス
    
    // パーティクルの構造体
    struct Particle {
        float x;        // X座標
        float y;        // Y座標
        float speed;    // 落下素行度
    };
    Particle particles[PARTICLE_MAX];       // パーティクルの配列

    float logoScale = 1.0f;                 // ロゴの拡大率
	float bandOffset = 0.0f;			    // バンドのオフセット  
    int pressAlpha = 255;                   // PressEnterのアルファ値
};
