// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// TitleScene.cpp
// 繧ｿ繧､繝医Ν逕ｻ髱｢縺ｮ讖溯・繧呈署萓帙＠縺ｾ縺吶・
// 
//========================================

#include "TitleScene.h"
#include "Config.h"
#include <cmath>

// @brief 繧ｳ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ
TitleScene::TitleScene()
{
    // 3D險ｭ螳・
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // BGM 隱ｭ縺ｿ霎ｼ縺ｿ
    bgm = LoadSoundMem("BGM/Title.mp3");
    Config::currentBgmHandle = bgm;
    Config::ApplyVolume();
    PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);

    // 繝輔か繝ｳ繝郁ｪｭ縺ｿ霎ｼ縺ｿ
    titleFontLarge = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 80, 3);
    titleFontSmall = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 32, 3);


    // 邊貞ｭ仙・譛溷喧
    // 蜈ｨ縺ｦ縺ｮ繝代・繝・ぅ繧ｯ繝ｫ縺ｮ蛻晄悄菴咲ｽｮ縺ｨ騾溷ｺｦ繧偵Λ繝ｳ繝繝縺ｫ險ｭ螳壹☆繧・
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].x = rand() % Config::screenWidth;
        particles[i].y = rand() % Config::screenHeight;
        particles[i].speed = 0.5f + (rand() % 100) / 100.0f;
    }
}

// @brief 繝・せ繝医Λ繧ｯ繧ｿ
TitleScene::~TitleScene(){}

// @brief 譖ｴ譁ｰ縺励∪縺・
void TitleScene::Update()
{
    bool nowEnter = CheckHitKey(KEY_INPUT_RETURN) != 0;
    bool nowSpace = CheckHitKey(KEY_INPUT_SPACE) != 0;

    bool enterDown = nowEnter && !prevEnter;
    bool spaceDown = nowSpace && !prevSpace;

    prevEnter = nowEnter;
    prevSpace = nowSpace;

    static int upTimer = 0;
    static int downTimer = 0;

    const int REPEAT_START = 120;       // 髟ｷ謚ｼ縺鈴幕蟋九∪縺ｧ縺ｮ蠕・■譎る俣
    const int REPEAT_INTERVAL = 25;      // 髟ｷ謚ｼ縺玲凾縺ｮ郢ｰ繧願ｿ斐＠髢馴囈

    int up = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
    int down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);

    // 荳顔ｧｻ蜍・
    // 荳翫く繝ｼ縺梧款縺輔ｌ縺ｦ縺・ｋ蝣ｴ蜷医・蜃ｦ逅・
    if (up)
    {
        // 譛蛻昴↓謚ｼ縺励◆迸ｬ髢薙√∪縺溘・髟ｷ謚ｼ縺励＠縺ｦ荳螳壽凾髢薙′邨碁℃縺励◆蠕後↓荳螳夐俣髫斐〒蜃ｦ逅・ｒ陦後≧
        if (upTimer == 0 || (upTimer > REPEAT_START && upTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 2) % 3;  // 荳翫∈
        }
        upTimer++;
    }
    else
    {
        upTimer = 0;
    }

    // 荳狗ｧｻ蜍・
    // 荳九く繝ｼ縺梧款縺輔ｌ縺ｦ縺・ｋ蝣ｴ蜷医・蜃ｦ逅・
    if (down)
    {
        // 譛蛻昴↓謚ｼ縺励◆迸ｬ髢薙√∪縺溘・髟ｷ謚ｼ縺励＠縺ｦ荳螳壽凾髢薙′邨碁℃縺励◆蠕後↓荳螳夐俣髫斐〒蜃ｦ逅・ｒ陦後≧
        if (downTimer == 0 || (downTimer > REPEAT_START && downTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 1) % 3;  // 荳九∈
        }
        downTimer++;
    }
    else
    {
        downTimer = 0;
    }

    // 豎ｺ螳・
    // 豎ｺ螳壹く繝ｼ・医お繝ｳ繧ｿ繝ｼ縺ｾ縺溘・繧ｹ繝壹・繧ｹ・峨′謚ｼ縺輔ｌ縺溷ｴ蜷医・蜃ｦ逅・
    if (enterDown || spaceDown)
    {
        // 驕ｸ謚槭＆繧後※縺・ｋ繝｡繝九Η繝ｼ鬆・岼縺ｫ蠢懊§縺ｦ蜃ｦ逅・ｒ蛻・ｲ舌☆繧・
        if (menuIndex == 0)
        {
            goNext = true; // Start
        }
        else if (menuIndex == 1)
        {
            goSetting = true;
        }
        else if (menuIndex == 2)
        {
            DxLib_End();
            exit(0);
        }
    }

    // 繝代・繝・ぅ繧ｯ繝ｫ譖ｴ譁ｰ・域里蟄假ｼ・
    // 蜈ｨ縺ｦ縺ｮ繝代・繝・ぅ繧ｯ繝ｫ縺ｮ菴咲ｽｮ繧呈峩譁ｰ縺吶ｋ
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].y += particles[i].speed;
        // 逕ｻ髱｢荳狗ｫｯ繧定ｶ翫∴縺溘ｉ荳翫↓謌ｻ縺・
        if (particles[i].y > Config::screenHeight)
        {
            particles[i].y = 0;
            particles[i].x = rand() % Config::screenWidth;
        }
    }
}

// @brief 謠冗判縺励∪縺・
void TitleScene::Draw()
{
    ClearDrawScreen();

    // ============================
    // 繝｡繝九Η繝ｼ謠冗判・医・繝ｼ繧ｫ繝ｼ莉倥″・・
    // ============================
    const char* items[3] = { "START", "SETTING", "EXIT" };

    // 3縺､縺ｮ繝｡繝九Η繝ｼ鬆・岼繧帝・分縺ｫ繝ｫ繝ｼ繝励＠縺ｦ謠冗判縺吶ｋ
    for (int i = 0; i < 3; i++)
    {
        int y = 350 + i * 60;

        // 驕ｸ謚樔ｸｭ縺ｯ鮟・牡縲√◎繧御ｻ･螟悶・逋ｽ
        int color = (i == menuIndex)
            ? GetColor(255, 255, 0)
            : GetColor(255, 255, 255);

        // 繝槭・繧ｫ繝ｼ繧呈緒逕ｻ
        if (i == menuIndex)
        {
            DrawStringToHandle(540, y, ">", color, titleFontSmall);
        }

        // 繝｡繝九Η繝ｼ鬆・岼
        DrawStringToHandle(580, y, items[i], color, titleFontSmall);
    }


    // ============================
    // 繧ｫ繝｡繝ｩ繧呈ｴｾ謇九↓蜍輔°縺・
    // ============================
    float t = GetNowCount() * 0.002f;

    // 繧ｫ繝｡繝ｩ繧貞・霆碁％縺ｧ蜍輔°縺・
    float camX = 40.0f * cosf(t);
    float camZ = 40.0f * sinf(t);
    float camY = 20.0f + 5.0f * sinf(t * 2);

    // 繧ｫ繝｡繝ｩ菴咲ｽｮ縺ｨ豕ｨ隕也せ繧定ｨｭ螳・
    SetCameraPositionAndTarget_UpVecY(
        VGet(camX, camY, camZ),
        VGet(0.0f, 10.0f, 0.0f)
    );

    // ============================
    // 繧ｭ繝ｩ繧ｭ繝ｩ邊貞ｭ・
    // ============================
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        DrawPixel(particles[i].x, particles[i].y, GetColor(200, 200, 255));
    }

    // ============================
    // 繧ｿ繧､繝医Ν繝ｭ繧ｴ・域僑螟ｧ邵ｮ蟆擾ｼ・
    // ============================
    float scale = 1.0f + 0.05f * sinf(GetNowCount() * 0.005f);

    // 繧ｿ繧､繝医Ν繝・く繧ｹ繝医・蟷・ｒ蜿門ｾ励＠縺ｦ縲∫判髱｢荳ｭ螟ｮ(X=(Config::screenWidth / 2))縺ｫ驟咲ｽｮ縺吶ｋ
    const char* title = "MUSIC GAME!!";
    int w = GetDrawStringWidthToHandle(title, strlen(title), titleFontLarge);
    // 繧ｹ繧ｱ繝ｼ繝ｫ繧定・・縺励※譁・ｭ怜・縺ｮ荳ｭ螟ｮ縺ｮX蠎ｧ讓吶ｒ險育ｮ励☆繧・
    int x = (Config::screenWidth / 2) - (int)(w * scale / 2);
    
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawExtendStringToHandle(x, 150, scale, scale, title, GetColor(255, 255, 255), titleFontLarge);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ============================
    // PRESS ENTER・育せ貊・ｼ・
    // ============================
    // 繧ｵ繧､繝ｳ豕｢繧剃ｽｿ縺｣縺ｦ縲∵凾髢鍋ｵ碁℃縺ｨ縺ｨ繧ゅ↓繧｢繝ｫ繝輔ぃ蛟､・磯乗・蠎ｦ・峨ｒ0~255縺ｮ髢薙〒貊代ｉ縺九↓螟牙喧縺輔○繧・
    int alpha = 128 + 127 * sinf(GetNowCount() * 0.01f);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    int pw = GetDrawStringWidthToHandle("PRESS ENTER OR SPACE", strlen("PRESS ENTER OR SPACE"), titleFontSmall);
    DrawStringToHandle((Config::screenWidth / 2) - pw / 2, (int)(Config::screenHeight * 0.83f), "PRESS ENTER OR SPACE", GetColor(255, 255, 255), titleFontSmall);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
