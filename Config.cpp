#include "Config.h"
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <DxLib.h>

namespace Config
{
    int screenWidth = 1280;
    int screenHeight = 720;
    float noteSpeed = 1.0f;
    float masterVolume = 1.0f;
    float bgmVolume = 1.0f;
    float seVolume = 1.0f;

    int currentBgmHandle = -1;

    const std::string CONFIG_FILE = "config.json";

    void Load()
    {
        std::ifstream file(CONFIG_FILE);
        if (file.is_open())
        {
            try
            {
                nlohmann::json j;
                file >> j;

                if (j.contains("screenWidth")) screenWidth = j["screenWidth"];
                if (j.contains("screenHeight")) screenHeight = j["screenHeight"];
                if (j.contains("noteSpeed")) noteSpeed = j["noteSpeed"];
                if (j.contains("masterVolume")) masterVolume = j["masterVolume"];
                if (j.contains("bgmVolume")) bgmVolume = j["bgmVolume"];
                if (j.contains("seVolume")) seVolume = j["seVolume"];
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to parse config.json: " << e.what() << std::endl;
            }
        }
    }

    void Save()
    {
        nlohmann::json j;
        j["screenWidth"] = screenWidth;
        j["screenHeight"] = screenHeight;
        j["noteSpeed"] = noteSpeed;
        j["masterVolume"] = masterVolume;
        j["bgmVolume"] = bgmVolume;
        j["seVolume"] = seVolume;

        std::ofstream file(CONFIG_FILE);
        if (file.is_open())
        {
            file << j.dump(4);
        }
    }

    void ApplyVolume()
    {
        if (currentBgmHandle != -1)
        {
            int vol = (int)(255 * bgmVolume * masterVolume);
            if (vol < 0) vol = 0;
            if (vol > 255) vol = 255;
            ChangeVolumeSoundMem(vol, currentBgmHandle);
        }
    }
}
