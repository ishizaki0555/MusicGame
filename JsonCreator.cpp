// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// JsonCrator.cpp
// 音源をまとめたJsonファイルを作成する機能を提供します。
// 
//========================================

#include "JsonCreator.h"

// @brief 指定されたフォルダパスにmusic.jsonを作成する
// @param folderPath フォルダパス
void JsonCreator::CreateMusicJson(const fs::path& folderPath)
{
    nlohmann::json json;
    json["musics"] = nlohmann::json::array();

    // Musics フォルダ内のサブフォルダを列挙
    for (auto& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_directory())
        {
            fs::path musicFile;

            // サブフォルダ内の music.xxx を探す
            for (auto& file : fs::directory_iterator(entry.path()))
            {
                auto ext = file.path().extension().string();
                auto name = file.path().stem().string();

                if (name == "music" &&
                    (ext == ".mp3" || ext == ".wav" || ext == ".ogg"))
                {
                    musicFile = file.path();
                    break;
                }
            }

            // music.xxx が見つかった場合のみ JSON に追加
            if (!musicFile.empty())
            {
                json["musics"].push_back({
                    {"folder", entry.path().filename().string()},
                    {"file", musicFile.filename().string()}
                    });
            }
        }
    }

    // JSON 保存
    std::ofstream file(folderPath / "musics.json");
    file << json.dump(4);
}