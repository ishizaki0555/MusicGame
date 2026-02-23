#pragma once
#include <DxLib.h>

class TitleScene
{
public:
    TitleScene();
    ~TitleScene();

    void Update();
    void Draw();

    bool goNext = false; // ENTERで次へ
    int bgm = -1;

private:

    // パーティクル関連
    static const int PARTICLE_MAX = 60;
    struct Particle {
        float x;
        float y;
        float speed;
    };
    Particle particles[PARTICLE_MAX];

    float logoScale = 1.0f;
    float bandOffset = 0.0f;
    int pressAlpha = 255;
};
