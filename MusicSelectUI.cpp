#include "MusicSelectUI.h"
#include "JsonCreator.h"
#include <DxLib.h>
#include <codecvt> 
#include <locale>
#include <windows.h>

MusicSelectUI::MusicSelectUI(const fs::path& folder)
    : folderPath(folder)
    , longPressValue(50)
{
    selectSE = LoadSoundMem("Sounds/select.mp3");
}

MusicSelectUI::~MusicSelectUI()
{
}

void MusicSelectUI::LoadFont(const std::string& fontPath, int size)
{
    fontHandleSmall = CreateFontToHandle(fontPath.c_str(), size, 3);
    fontHandleLarge = CreateFontToHandle(fontPath.c_str(), size + 12, 3);

    fontTable[0] = fontHandleSmall;
    fontTable[1] = fontHandleSmall;
    fontTable[2] = fontHandleLarge;
    fontTable[3] = fontHandleSmall;
    fontTable[4] = fontHandleSmall;
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

std::string Utf8ToSjis(const std::string& utf8)
{
    // UTF-8 → UTF-16
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

    // UTF-16 → Shift-JIS
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

    // 曲の決定
    static int enterTimer = 0;
    static int spaceTimer = 0;

    int enter = CheckHitKey(KEY_INPUT_RETURN);
    int space = CheckHitKey(KEY_INPUT_SPACE);

    bool decide = false;

    // Enterキーの入力の検知
    if (enter)
    {
        if (enterTimer == 0) decide = true;
        enterTimer++;
    }
    else enterTimer = 0;

    // Spaceキーの入力の検知
    if (space)
    {
        if (spaceTimer == 0) decide = true;
        spaceTimer++;
    }
    else spaceTimer = 0;

    //選曲の決定処理
    if (decide)
    {
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
    int prev2 = (selectedIndex - 2 + musicList.size()) % musicList.size();
    int prev1 = (selectedIndex - 1 + musicList.size()) % musicList.size();
    int now = selectedIndex;
    int next1 = (selectedIndex + 1) % musicList.size();
    int next2 = (selectedIndex + 2) % musicList.size();

    // 中央補正（大フォント）
    int centerOffset = 10;

    int baseY = leftBoxY + 150;   // 中央の基準位置

    int indices[5] = { prev2, prev1, now, next1, next2 };

    for (int i = 0; i < 5; i++)
    {
        int idx = indices[i];
        int font = fontTable[i];
        float y = baseY + posTable[i];

        // 背景ボックス（曲ごとに独立）
        int boxW = (font == fontHandleLarge) ? 500 : 350;
        int boxH = (font == fontHandleLarge) ? 70 : 50;  // 中央だけ大きい
        int boxX = centerX - boxW / 2;
        int boxY = y - boxH / 2;

        // 色（中央だけ濃くするなども可能）
        int r = (i == 2) ? 200 : 180;
        int g = (i == 2) ? 200 : 180;
        int b = (i == 2) ? 255 : 220;
        int a = 255;

        DrawBoxEx(boxX, boxY, boxW, boxH, r, g, b, a);

        int barWidth = (font == fontHandleLarge) ? 460 : 300;
        int leftX = centerX - barWidth / 2;

        if (i == 2)
        {
            // 選択中だけスクロール
            DrawTextLeftSlide(
                musicList[idx].title,
                leftX,
                y - 10,
                barWidth,
                GetColor(0, 0, 0),
                font
            );
        }
        else
        {
            // 非選択は左揃え＋クリッピングのみ
            DrawTextLeftClip(
                musicList[idx].title,
                leftX,
                y - 10,
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
