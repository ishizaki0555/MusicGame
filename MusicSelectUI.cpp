#include "MusicSelectUI.h"
#include "JsonCreator.h"
#include <DxLib.h>
#include <codecvt> 
#include <locale>
#include <algorithm>
#include <cmath>

#define NOMINMAX
#include <windows.h>

MusicSelectUI::MusicSelectUI(const fs::path& folder)
    : folderPath(folder)
    , longPressValue(50)
{
    // 交換音の取得
    selectSE = LoadSoundMem("Sounds/select.mp3");

    // リザルト画面のBGMを取得
    bgmHandle = LoadSoundMem("Sounds/select.wav");
}

MusicSelectUI::~MusicSelectUI()
{
}

void MusicSelectUI::LoadFont(const std::string& fontPath, int size)
{
    fontHandleSmall = CreateFontToHandle(fontPath.c_str(), size, 3);
    fontHandleLarge = CreateFontToHandle(fontPath.c_str(), size + 12, 3);

}

void MusicSelectUI::LoadMusicList()
{
    JsonCreator::CreateMusicJson(folderPath);

    auto json = JsonLoader::Load((folderPath / "musics.json"));
    if (json.empty() || !json.contains("musics"))
        return;

    for (auto& item : json["musics"])
    {
        MusicInfo info;
        info.folder = item["folder"];
        info.file = item["file"];

        fs::path easy = folderPath / fs::u8path(info.folder) / "Easy.json";
        fs::path normal = folderPath / fs::u8path(info.folder) / "Normal.json";
        fs::path hard = folderPath / fs::u8path(info.folder) / "Hard.json";
        fs::path extra = folderPath / fs::u8path(info.folder) / "Extra.json";

        if (fs::exists(easy)) info.easyChart = "Easy.json";
        if (fs::exists(normal)) info.normalChart = "Normal.json";
        if (fs::exists(hard)) info.hardChart = "Hard.json";
        if (fs::exists(extra)) info.extraChart = "Extra.json";

        if (fs::exists(easy))
        {
            auto chart = JsonLoader::Load(easy);
            info.title = chart.value("name", info.folder);
            info.bpm = chart.value("BPM", 0);
        }
        else
        {
            info.title = info.folder;
        }

        static const std::vector<std::string> exts = {
            ".png", ".jpg", ".jpeg", ".bmp"
        };

        for (auto& ext : exts)
        {
            fs::path bannerPath = folderPath / fs::u8path(info.folder) / ("banner" + ext);
            if (fs::exists(bannerPath))
            {
                info.banner = bannerPath.string();
                info.bannerHandle = LoadGraph(bannerPath.string().c_str());
                break;
            }
        }

        musicList.push_back(info);
    }
}

void MusicSelectUI::DrawBoxEx(int x, int y, int w, int h, int r, int g, int b, int a)
{
    int color = GetColor(r, g, b);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
    DrawBox(x, y, x + w, y + h, color, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

// @brief UTF-8の文字列をShift-JISに変換します
// @param utf8 UTF-8でエンコードされた文字列
// @return Shift-8でエンコードされた文字列
std::string Utf8ToSjis(const std::string& utf8)
{
    // UTF-8をUTF-16に変換する
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // UTF-16をShift-JISに変換する
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string sjis(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, nullptr, nullptr);

    return sjis;
}

void MusicSelectUI::DrawTextEx(const std::string& text, int x, int y, int color, int font)
{
    if (font == -1) font = fontHandleSmall;

    std::string sjis = Utf8ToSjis(text);
    DrawStringToHandle(x, y, sjis.c_str(), color, font);
}

void MusicSelectUI::DrawTextCenter(const std::string& text, int centerX, int y, int color, int font)
{
    if (font == -1) font = fontHandleSmall;

    std::string sjis = Utf8ToSjis(text);

    int w = GetDrawStringWidthToHandle(sjis.c_str(), sjis.size(), font);
    int x = centerX - w / 2;

    DrawStringToHandle(x, y, sjis.c_str(), color, font);
}

void MusicSelectUI::DrawTextLeftSlide(
    const std::string& text,
    int leftX,
    int y,
    int barWidth,
    int color,
    int font)
{
    if (font == -1) font = fontHandleSmall;

    std::string sjis = Utf8ToSjis(text);
    int textWidth = GetDrawStringWidthToHandle(sjis.c_str(), sjis.size(), font);

    int clipX1 = leftX;
    int clipX2 = leftX + barWidth;

    SetDrawArea(clipX1, y - 5, clipX2, y + 50);

    int drawX = leftX;

    if (textWidth > barWidth)
    {
        int loopWidth = textWidth + 60;
        int offset = titleScroll % loopWidth;

        drawX = leftX - offset;

        // ループ用の2つ目
        DrawStringToHandle(drawX + loopWidth, y, sjis.c_str(), color, font);
    }

    DrawStringToHandle(drawX, y, sjis.c_str(), color, font);

    SetDrawAreaFull();
}


void MusicSelectUI::DrawTextLeftClip(
    const std::string& text,
    int leftX,
    int y,
    int barWidth,
    int color,
    int font)
{
    if (font == -1) font = fontHandleSmall;

    std::string sjis = Utf8ToSjis(text);
    int textWidth = GetDrawStringWidthToHandle(sjis.c_str(), sjis.size(), font);

    // クリッピング範囲（バーの幅）
    int clipX1 = leftX;
    int clipX2 = leftX + barWidth;

    SetDrawArea(clipX1, y - 5, clipX2, y + 50);

    // 左揃え
    DrawStringToHandle(leftX, y, sjis.c_str(), color, font);

    // クリッピング解除
    SetDrawAreaFull();
}

void MusicSelectUI::GetDifficultyColor(Difficulty diff, int& r, int& g, int& b)
{
    switch (diff)
    {
    case Difficulty::Easy:   r = 80;  g = 200; b = 80;  break;
    case Difficulty::Normal: r = 220; g = 220; b = 80;  break;
    case Difficulty::Hard:   r = 220; g = 80;  b = 80;  break;
    case Difficulty::Extra:  r = 150; g = 80;  b = 220; break;
    }
}

void MusicSelectUI::Update()
{
    // スクロール制御
    const int WAIT_TIME = 120;      // 約2秒（60FPS想定）
    const int SCROLL_SPEED = 1;     // スクロール速度

    if (!sceneStarted)
    {
        sceneStarted = true;
        PlaySoundMem(bgmHandle, DX_PLAYTYPE_LOOP);

        prevEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0);
        prevSpace = (CheckHitKey(KEY_INPUT_SPACE) != 0);
    }

    switch (scrollState)
    {
    case ScrollState::WaitStart:
        scrollWaitTimer++;
        if (scrollWaitTimer >= WAIT_TIME)
        {
            scrollWaitTimer = 0;
            scrollState = ScrollState::Scroll;
        }
        break;

    case ScrollState::Scroll:
        titleScroll += SCROLL_SPEED;

        // ループ幅（DrawTextLeftSlide と合わせる）
        {
            int textWidth = GetDrawStringWidthToHandle(
                Utf8ToSjis(musicList[selectedIndex].title).c_str(),
                Utf8ToSjis(musicList[selectedIndex].title).size(),
                fontHandleLarge
            );

            int loopWidth = textWidth + 60;

            // 1ループ終わったら停止へ
            if (titleScroll >= loopWidth)
            {
                titleScroll = 0;
                scrollState = ScrollState::WaitEnd;
            }
        }
        break;

    case ScrollState::WaitEnd:
        scrollWaitTimer++;
        if (scrollWaitTimer >= WAIT_TIME)
        {
            scrollWaitTimer = 0;
            scrollState = ScrollState::Scroll;
        }
        break;
    }

    int mouse = GetMouseInput();

    // 楽曲更新
    static int upTimer = 0;
    static int downTimer = 0;
    static int wTimer = 0;
    static int sTimer = 0;

    int up = CheckHitKey(KEY_INPUT_UP);
    int down = CheckHitKey(KEY_INPUT_DOWN);
    int keyW = CheckHitKey(KEY_INPUT_W);
    int keyS = CheckHitKey(KEY_INPUT_S);

    // UP
    if (up || keyW)
    {
        if (upTimer == 0 || (upTimer > longPressValue && upTimer % 10 == 0))
        {
			PlaySoundMem(selectSE, DX_PLAYTYPE_BACK);
            selectedIndex = (selectedIndex - 1 + musicList.size()) % musicList.size();
        }
        upTimer++;
    }
    else upTimer = 0;

    // DOWN
    if (down || keyS)
    {
        if (downTimer == 0 || (downTimer > longPressValue && downTimer % 10 == 0))
        {
            PlaySoundMem(selectSE, DX_PLAYTYPE_BACK);
            selectedIndex = (selectedIndex + 1) % musicList.size();
        }
        downTimer++;
    }
    else downTimer = 0;


    // 難易度選択
    static int leftTimer = 0;
    static int rightTimer = 0;
    static int qTimer = 0;
    static int eTimer = 0;

    int left = CheckHitKey(KEY_INPUT_LEFT);
    int right = CheckHitKey(KEY_INPUT_RIGHT);
    int keyQ = CheckHitKey(KEY_INPUT_Q);
    int keyE = CheckHitKey(KEY_INPUT_E);

    int diffIndex = (int)selectedDifficulty;

    // キーボード入力から難易度を変更
    // Qキーか左矢印
    if (left || keyQ)
    {
        int& t = left ? leftTimer : qTimer;

        if (t == 0 || (t > longPressValue && t % 10 == 0))
        {
            PlaySoundMem(selectSE, DX_PLAYTYPE_BACK);
            diffIndex = (diffIndex - 1 + 4) % 4;
        }
        t++;
    }
    else
    {
        leftTimer = 0;
        qTimer = 0;
    }

    // Eキーか右矢印
    if (right || keyE)
    {
        int& t = right ? rightTimer : eTimer;

        if (t == 0 || (t > longPressValue && t % 10 == 0))
        {
            PlaySoundMem(selectSE, DX_PLAYTYPE_BACK);
            diffIndex = (diffIndex + 1) % 4;
        }
        t++;
    }
    else
    {
        rightTimer = 0;
        eTimer = 0;
    }


    selectedDifficulty = (Difficulty)diffIndex;

    // 決定キーの現在状態
    bool nowEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0);
    bool nowSpace = (CheckHitKey(KEY_INPUT_SPACE) != 0);

    // 押した瞬間だけ true になる
    bool enterDown = (nowEnter && !prevEnter);
    bool spaceDown = (nowSpace && !prevSpace);

    // 次フレームのために保存
    prevEnter = nowEnter;
    prevSpace = nowSpace;

    bool decide = (enterDown || spaceDown);

    //選曲の決定処理
    if (decide)
    {
        StopSoundMem(bgmHandle);

        if (!musicList.empty())
        {
            auto& info = musicList[selectedIndex];

            std::string chartFile;
            switch (selectedDifficulty)
            {
            case Difficulty::Easy:   chartFile = info.easyChart; break;
            case Difficulty::Normal: chartFile = info.normalChart; break;
            case Difficulty::Hard:   chartFile = info.hardChart; break;
            case Difficulty::Extra:  chartFile = info.extraChart; break;
            }

            if (!chartFile.empty())
            {
                fs::path chartPath = folderPath / fs::u8path(info.folder) / chartFile;
                NotesData data;
                data.LoadFromJson(JsonLoader::Load(chartPath.string()));
                data.musicPath = (folderPath / fs::u8path(info.folder) / info.file).string();
                data.folderPath = (folderPath / fs::u8path(info.folder)).string();

                createdScene = new GameScene(data, info.bannerHandle);
            }
        }
    }
    if (!musicList.empty())
    {
        float diff = selectedIndex - currentViewIndex;
        float halfSize = musicList.size() / 2.0f;
        while (diff > halfSize) diff -= musicList.size();
        while (diff < -halfSize) diff += musicList.size();

        currentViewIndex += diff * 0.15f; 

        if (currentViewIndex < 0.0f) currentViewIndex += musicList.size();
        if (currentViewIndex >= musicList.size()) currentViewIndex -= musicList.size();
    }
}
void MusicSelectUI::Draw()
{
    if (musicList.empty()) return;

    auto& info = musicList[selectedIndex];

    if (info.bannerHandle != -1)
    {
        // 画面サイズに合わせてバナーを引き伸ばす
        DrawExtendGraph(
            0, 0,
            1280, 720,
            info.bannerHandle,
            TRUE
        );
    }

    // ぼかし風にするための半透明黒
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
    DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ============================
    // 左側：背景ボックス
    // ============================
    int leftBoxX = 80;
    int leftBoxY = 140;
    int leftBoxW = 350;
    int leftBoxH = 500;

    // 中央X
    int centerX = leftBoxX + leftBoxW / 2 + 50;

    // 行間
    int leftY = leftBoxY + 180;
    int row = 55;

    // インデックス
    int baseY = leftBoxY + 150;

    int centerIdx = (int)std::floor(currentViewIndex);

    for (int i = -3; i <= 4; i++)
    {
        long long virtualIdx = (long long)centerIdx + i;
        int idx = (virtualIdx % (long long)musicList.size() + musicList.size()) % musicList.size();

        float dist = (float)virtualIdx - currentViewIndex;
        float focus = std::max(0.0f, 1.0f - std::abs(dist));

        float baseSpacing = 70.0f;
        float yOffset = dist * baseSpacing;

        float easeDist = std::min(std::abs(dist), 1.0f);
        if (dist > 0.0f) {
            yOffset += 10.0f * easeDist;
        } else if (dist < 0.0f) {
            yOffset -= 10.0f * easeDist;
        }

        float finalY = baseY + yOffset;

        if (finalY < leftBoxY - 50 || finalY > leftBoxY + leftBoxH + 50) continue;

        int boxW = 350 + (int)(150.0f * focus);
        int boxH = 50 + (int)(20.0f * focus);
        int boxX = centerX - boxW / 2;
        int boxY = (int)finalY - boxH / 2;

        int r = (int)(180.0f + 20.0f * focus);
        int g = (int)(180.0f + 20.0f * focus);
        int b = (int)(220.0f + 35.0f * focus);
        int a = 255;

        if (std::abs(dist) > 2.0f) {
            a = (int)(255.0f - (std::abs(dist) - 2.0f) * 128.0f);
            if (a < 0) a = 0;
            if (a > 255) a = 255;
        }

        DrawBoxEx(boxX, boxY, boxW, boxH, r, g, b, a);

        int font = (focus > 0.5f) ? fontHandleLarge : fontHandleSmall;
        int barWidth = 300 + (int)(160.0f * focus); 
        int leftX = centerX - barWidth / 2;

        if (idx == selectedIndex && std::abs(dist) < 0.1f) 
        {
            DrawTextLeftSlide(
                musicList[idx].title,
                leftX,
                (int)finalY - 10,
                barWidth,
                GetColor(0, 0, 0),
                font
            );
        }
        else
        {
            DrawTextLeftClip(
                musicList[idx].title,
                leftX,
                (int)finalY - 10,
                barWidth,
                GetColor(0, 0, 0),
                font
            );
        }
    }

    // ============================
    // 右側：曲名（大きく）
    // ============================
    int mainX = 600;

    DrawTextEx(info.title, mainX, 40, GetColor(255, 255, 255), fontHandleSmall);

    // ============================
    // バナー
    // ============================
    int bannerY = 120;
    int bannerW = 500;
    int bannerH = 250;

    if (info.bannerHandle != -1)
    {
        DrawExtendGraph(
            mainX,
            bannerY,
            mainX + bannerW,
            bannerY + bannerH,
            info.bannerHandle,
            TRUE
        );
    }

    // ============================
    // 曲情報（BPMなど）
    // ============================
    DrawTextEx("BPM: " + std::to_string(info.bpm), mainX, bannerY + bannerH + 20, GetColor(255, 255, 255), fontHandleSmall);

    // ============================
    // 難易度ボタン
    // ============================
    int diffY = bannerY + bannerH + 80;
    int diffW = 140;
    int diffH = 60;
    int diffSpacing = 150;

    Difficulty diffs[4] = { Difficulty::Easy, Difficulty::Normal, Difficulty::Hard, Difficulty::Extra };
    const char* labels[4] = { "EASY", "NORMAL", "HARD", "EXTRA" };

    int dx = mainX;
    for (int i = 0; i < 4; i++)
    {
        int r, g, b;
        GetDifficultyColor(diffs[i], r, g, b);

        bool selected = (selectedDifficulty == diffs[i]);
        int alpha = selected ? 255 : 180;

        DrawBoxEx(dx, diffY, diffW, diffH, r, g, b, alpha);
        DrawTextEx(labels[i], dx + 30, diffY + 20, GetColor(0, 0, 0), fontHandleSmall);

        dx += diffSpacing;
    }

    // ============================
    // PLAY ボタン
    // ============================
    int playY = diffY + 100;
    int playW = 200;
    int playH = 60;

    DrawBoxEx(mainX, playY, playW, playH, 80, 150, 255, 255);
    DrawTextEx("PLAY", mainX + 60, playY + 20, GetColor(0, 0, 0), fontHandleSmall);
}

void MusicSelectUI::ResetInputState()
{
    prevEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0);
    prevSpace = (CheckHitKey(KEY_INPUT_SPACE) != 0);
}

