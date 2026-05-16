// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// TitleScene.cpp
// 郢ｧ・ｿ郢ｧ・､郢晏現ﾎ晞包ｽｻ鬮ｱ・｢邵ｺ・ｮ隶匁ｺｯ繝ｻ郢ｧ蜻育ｽｲ關灘ｸ呻ｼ邵ｺ・ｾ邵ｺ蜷ｶﾂ繝ｻ
// 
//========================================

#include "TitleScene.h"
#include "Config.h"
#include <cmath>

// @brief 郢ｧ・ｳ郢晢ｽｳ郢ｧ・ｹ郢晏現ﾎ帷ｹｧ・ｯ郢ｧ・ｿ
TitleScene::TitleScene()
{
    // 3D髫ｪ・ｭ陞ｳ繝ｻ
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // BGM 髫ｱ・ｭ邵ｺ・ｿ髴趣ｽｼ邵ｺ・ｿ
    bgm = LoadSoundMem("BGM/Title.mp3");
    Config::currentBgmHandle = bgm;
    Config::ApplyVolume();
    PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);

    // 郢晁ｼ斐°郢晢ｽｳ郢晞メ・ｪ・ｭ邵ｺ・ｿ髴趣ｽｼ邵ｺ・ｿ
    titleFontLarge = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 80, 3);
    titleFontSmall = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 32, 3);


    // 驍願ｲ橸ｽｭ莉吶・隴帶ｺｷ蝟ｧ
    // 陷茨ｽｨ邵ｺ・ｦ邵ｺ・ｮ郢昜ｻ｣繝ｻ郢昴・縺・ｹｧ・ｯ郢晢ｽｫ邵ｺ・ｮ陋ｻ譎・ｄ闖ｴ蜥ｲ・ｽ・ｮ邵ｺ・ｨ鬨ｾ貅ｷ・ｺ・ｦ郢ｧ蛛ｵﾎ帷ｹ晢ｽｳ郢敖郢晢｣ｰ邵ｺ・ｫ髫ｪ・ｭ陞ｳ螢ｹ笘・ｹｧ繝ｻ
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].x = rand() % Config::INTERNAL_WIDTH;
        particles[i].y = rand() % Config::INTERNAL_HEIGHT;
        particles[i].speed = 0.5f + (rand() % 100) / 100.0f;
    }
}

// @brief 郢昴・縺帷ｹ晏現ﾎ帷ｹｧ・ｯ郢ｧ・ｿ
TitleScene::~TitleScene(){}

// @brief 隴厄ｽｴ隴・ｽｰ邵ｺ蜉ｱ竏ｪ邵ｺ繝ｻ
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

    const int REPEAT_START = 120;       // 鬮滂ｽｷ隰夲ｽｼ邵ｺ驤ｴ蟷戊沂荵昶穐邵ｺ・ｧ邵ｺ・ｮ陟輔・笆隴弱ｋ菫｣
    const int REPEAT_INTERVAL = 25;      // 鬮滂ｽｷ隰夲ｽｼ邵ｺ邇ｲ蜃ｾ邵ｺ・ｮ驛｢・ｰ郢ｧ鬘假ｽｿ譁撰ｼ鬮｢鬥ｴ蝗・

    int up = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
    int down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);

    // 闕ｳ鬘費ｽｧ・ｻ陷阪・
    // 闕ｳ鄙ｫ縺冗ｹ晢ｽｼ邵ｺ譴ｧ谺ｾ邵ｺ霈費ｽ檎ｸｺ・ｦ邵ｺ繝ｻ・玖撻・ｴ陷ｷ蛹ｻ繝ｻ陷・ｽｦ騾・・
    if (up)
    {
        // 隴崢陋ｻ譏ｴ竊楢ｬ夲ｽｼ邵ｺ蜉ｱ笳・ｿｸ・ｬ鬮｢阮卍竏壺穐邵ｺ貅倥・鬮滂ｽｷ隰夲ｽｼ邵ｺ蜉ｱ・邵ｺ・ｦ闕ｳﾂ陞ｳ螢ｽ蜃ｾ鬮｢阮吮ｲ驍ｨ遒≫с邵ｺ蜉ｱ笳・募ｾ娯・闕ｳﾂ陞ｳ螟蝉ｿ｣鬮ｫ譁舌定怎・ｦ騾・・・帝勗蠕娯鴬
        if (upTimer == 0 || (upTimer > REPEAT_START && upTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 2) % 3;  // 闕ｳ鄙ｫ竏・
        }
        upTimer++;
    }
    else
    {
        upTimer = 0;
    }

    // 闕ｳ迢暦ｽｧ・ｻ陷阪・
    // 闕ｳ荵昴￥郢晢ｽｼ邵ｺ譴ｧ谺ｾ邵ｺ霈費ｽ檎ｸｺ・ｦ邵ｺ繝ｻ・玖撻・ｴ陷ｷ蛹ｻ繝ｻ陷・ｽｦ騾・・
    if (down)
    {
        // 隴崢陋ｻ譏ｴ竊楢ｬ夲ｽｼ邵ｺ蜉ｱ笳・ｿｸ・ｬ鬮｢阮卍竏壺穐邵ｺ貅倥・鬮滂ｽｷ隰夲ｽｼ邵ｺ蜉ｱ・邵ｺ・ｦ闕ｳﾂ陞ｳ螢ｽ蜃ｾ鬮｢阮吮ｲ驍ｨ遒≫с邵ｺ蜉ｱ笳・募ｾ娯・闕ｳﾂ陞ｳ螟蝉ｿ｣鬮ｫ譁舌定怎・ｦ騾・・・帝勗蠕娯鴬
        if (downTimer == 0 || (downTimer > REPEAT_START && downTimer % REPEAT_INTERVAL == 0))
        {
            menuIndex = (menuIndex + 1) % 3;  // 闕ｳ荵昶・
        }
        downTimer++;
    }
    else
    {
        downTimer = 0;
    }

    // 雎趣ｽｺ陞ｳ繝ｻ
    // 雎趣ｽｺ陞ｳ螢ｹ縺冗ｹ晢ｽｼ繝ｻ蛹ｻ縺顔ｹ晢ｽｳ郢ｧ・ｿ郢晢ｽｼ邵ｺ・ｾ邵ｺ貅倥・郢ｧ・ｹ郢晏｣ｹ繝ｻ郢ｧ・ｹ繝ｻ蟲ｨ窶ｲ隰夲ｽｼ邵ｺ霈費ｽ檎ｸｺ貅ｷ・ｰ・ｴ陷ｷ蛹ｻ繝ｻ陷・ｽｦ騾・・
    if (enterDown || spaceDown)
    {
        // 鬩包ｽｸ隰壽ｧｭ・・ｹｧ蠕娯ｻ邵ｺ繝ｻ・狗ｹ晢ｽ｡郢昜ｹ斟礼ｹ晢ｽｼ鬯・・蟯ｼ邵ｺ・ｫ陟｢諛環ｧ邵ｺ・ｦ陷・ｽｦ騾・・・定崕繝ｻ・ｲ闊娯・郢ｧ繝ｻ
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

    // 郢昜ｻ｣繝ｻ郢昴・縺・ｹｧ・ｯ郢晢ｽｫ隴厄ｽｴ隴・ｽｰ繝ｻ蝓滄㈹陝・∞・ｼ繝ｻ
    // 陷茨ｽｨ邵ｺ・ｦ邵ｺ・ｮ郢昜ｻ｣繝ｻ郢昴・縺・ｹｧ・ｯ郢晢ｽｫ邵ｺ・ｮ闖ｴ蜥ｲ・ｽ・ｮ郢ｧ蜻亥ｳｩ隴・ｽｰ邵ｺ蜷ｶ・・
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        particles[i].y += particles[i].speed;
        // 騾包ｽｻ鬮ｱ・｢闕ｳ迢暦ｽｫ・ｯ郢ｧ螳夲ｽｶ鄙ｫ竏ｴ邵ｺ貅假ｽ芽叉鄙ｫ竊楢ｬ鯉ｽｻ邵ｺ繝ｻ
        if (particles[i].y > Config::INTERNAL_HEIGHT)
        {
            particles[i].y = 0;
            particles[i].x = rand() % Config::INTERNAL_WIDTH;
        }
    }
}

// @brief 隰蜀怜愛邵ｺ蜉ｱ竏ｪ邵ｺ繝ｻ
void TitleScene::Draw()
{
    ClearDrawScreen();

    // ============================
    // 郢晢ｽ｡郢昜ｹ斟礼ｹ晢ｽｼ隰蜀怜愛繝ｻ蛹ｻ繝ｻ郢晢ｽｼ郢ｧ・ｫ郢晢ｽｼ闔牙･窶ｳ繝ｻ繝ｻ
    // ============================
    const char* items[3] = { "START", "SETTING", "EXIT" };

    // 3邵ｺ・､邵ｺ・ｮ郢晢ｽ｡郢昜ｹ斟礼ｹ晢ｽｼ鬯・・蟯ｼ郢ｧ蟶晢｣ｰ繝ｻ蛻・ｸｺ・ｫ郢晢ｽｫ郢晢ｽｼ郢晏干・邵ｺ・ｦ隰蜀怜愛邵ｺ蜷ｶ・・
    for (int i = 0; i < 3; i++)
    {
        int y = 350 + i * 60;

        // 鬩包ｽｸ隰壽ｨ費ｽｸ・ｭ邵ｺ・ｯ魄溘・迚｡邵ｲ竏壺落郢ｧ蠕｡・ｻ・･陞滓じ繝ｻ騾具ｽｽ
        int color = (i == menuIndex)
            ? GetColor(255, 255, 0)
            : GetColor(255, 255, 255);

        // 郢晄ｧｭ繝ｻ郢ｧ・ｫ郢晢ｽｼ郢ｧ蜻育ｷ帝包ｽｻ
        if (i == menuIndex)
        {
            DrawStringToHandle(540, y, ">", color, titleFontSmall);
        }

        // 郢晢ｽ｡郢昜ｹ斟礼ｹ晢ｽｼ鬯・・蟯ｼ
        DrawStringToHandle(580, y, items[i], color, titleFontSmall);
    }


    // ============================
    // 郢ｧ・ｫ郢晢ｽ｡郢晢ｽｩ郢ｧ蜻茨ｽｴ・ｾ隰・ｹ昶・陷崎ｼ板ｰ邵ｺ繝ｻ
    // ============================
    float t = GetNowCount() * 0.002f;

    // 郢ｧ・ｫ郢晢ｽ｡郢晢ｽｩ郢ｧ雋槭・髴・｢・ｼ・ｸｺ・ｧ陷崎ｼ板ｰ邵ｺ繝ｻ
    float camX = 40.0f * cosf(t);
    float camZ = 40.0f * sinf(t);
    float camY = 20.0f + 5.0f * sinf(t * 2);

    // 郢ｧ・ｫ郢晢ｽ｡郢晢ｽｩ闖ｴ蜥ｲ・ｽ・ｮ邵ｺ・ｨ雎包ｽｨ髫穂ｹ溘○郢ｧ螳夲ｽｨ・ｭ陞ｳ繝ｻ
    SetCameraPositionAndTarget_UpVecY(
        VGet(camX, camY, camZ),
        VGet(0.0f, 10.0f, 0.0f)
    );

    // ============================
    // 郢ｧ・ｭ郢晢ｽｩ郢ｧ・ｭ郢晢ｽｩ驍願ｲ橸ｽｭ繝ｻ
    // ============================
    for (int i = 0; i < PARTICLE_MAX; i++)
    {
        DrawPixel(particles[i].x, particles[i].y, GetColor(200, 200, 255));
    }

    // ============================
    // 郢ｧ・ｿ郢ｧ・､郢晏現ﾎ晉ｹ晢ｽｭ郢ｧ・ｴ繝ｻ蝓溷ヱ陞滂ｽｧ驍ｵ・ｮ陝・得・ｼ繝ｻ
    // ============================
    float scale = 1.0f + 0.05f * sinf(GetNowCount() * 0.005f);

    // 郢ｧ・ｿ郢ｧ・､郢晏現ﾎ晉ｹ昴・縺冗ｹｧ・ｹ郢晏現繝ｻ陝ｷ繝ｻ・定愾髢・ｾ蜉ｱ・邵ｺ・ｦ邵ｲ竏ｫ蛻､鬮ｱ・｢闕ｳ・ｭ陞滂ｽｮ(X=(Config::INTERNAL_WIDTH / 2))邵ｺ・ｫ鬩溷調・ｽ・ｮ邵ｺ蜷ｶ・・
    const char* title = "MUSIC GAME!!";
    int w = GetDrawStringWidthToHandle(title, strlen(title), titleFontLarge);
    // 郢ｧ・ｹ郢ｧ・ｱ郢晢ｽｼ郢晢ｽｫ郢ｧ螳堋繝ｻ繝ｻ邵ｺ蜉ｱ窶ｻ隴√・・ｭ諤懊・邵ｺ・ｮ闕ｳ・ｭ陞滂ｽｮ邵ｺ・ｮX陟趣ｽｧ隶灘生・帝坎閧ｲ・ｮ蜉ｱ笘・ｹｧ繝ｻ
    int x = (Config::INTERNAL_WIDTH / 2) - (int)(w * scale / 2);
    
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawExtendStringToHandle(x, 150, scale, scale, title, GetColor(255, 255, 255), titleFontLarge);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // ============================
    // PRESS ENTER繝ｻ閧ｲ縺幄ｲ翫・・ｼ繝ｻ
    // ============================
    // 郢ｧ・ｵ郢ｧ・､郢晢ｽｳ雎包ｽ｢郢ｧ蜑・ｽｽ・ｿ邵ｺ・｣邵ｺ・ｦ邵ｲ竏ｵ蜃ｾ鬮｢骰具ｽｵ遒≫с邵ｺ・ｨ邵ｺ・ｨ郢ｧ繧・・郢ｧ・｢郢晢ｽｫ郢晁ｼ斐＜陋滂ｽ､繝ｻ逎ｯﾂ荵励・陟趣ｽｦ繝ｻ蟲ｨ・・~255邵ｺ・ｮ鬮｢阮吶定ｲ贋ｻ｣・臥ｸｺ荵昶・陞溽甥蝟ｧ邵ｺ霈披雷郢ｧ繝ｻ
    int alpha = 128 + 127 * sinf(GetNowCount() * 0.01f);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    int pw = GetDrawStringWidthToHandle("PRESS ENTER OR SPACE", strlen("PRESS ENTER OR SPACE"), titleFontSmall);
    DrawStringToHandle((Config::INTERNAL_WIDTH / 2) - pw / 2, (int)(Config::INTERNAL_HEIGHT * 0.83f), "PRESS ENTER OR SPACE", GetColor(255, 255, 255), titleFontSmall);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
