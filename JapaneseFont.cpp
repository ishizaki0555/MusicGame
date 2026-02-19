#include "JapaneseFont.h"

JapaneseFont::JapaneseFont()
{
}

JapaneseFont::~JapaneseFont()
{
    if (fontHandle != -1)
    {
        DeleteFontToHandle(fontHandle);
    }
}

bool JapaneseFont::Load(const std::string& path, int fontSize)
{
    // DXLib はフォントファイルを直接指定できる
    fontHandle = CreateFontToHandle(path.c_str(), fontSize, 3);

    return (fontHandle != -1);
}

void JapaneseFont::DrawText(const char* text, int x, int y, int color)
{
    if (fontHandle == -1) return;

    if (color == -1)
        color = GetColor(0, 0, 0);

    DrawStringToHandle(x, y, text, color, fontHandle);
}