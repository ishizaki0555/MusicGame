#pragma once
#include <DxLib.h>

class TitleScene
{
public:
    TitleScene();
    ~TitleScene();

    void Update();
    void Draw();

    bool goNext = false; // ENTER‚ÅŽŸ‚Ö
};
