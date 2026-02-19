#pragma once
#include <string>
#include <DxLib.h>

class JapaneseFont
{
public:
    JapaneseFont();
    ~JapaneseFont();

    // ƒtƒHƒ“ƒg“Ç‚İ‚İ
    bool Load(const std::string& path, int fontSize);

    // •`‰æ
    void DrawText(const char* text, int x, int y, int color = -1);

private:
    int fontHandle = -1;
};