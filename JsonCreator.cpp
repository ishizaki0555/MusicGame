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

    std::wstring search = folderPath.wstring() + L"\\*";

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (wcscmp(fd.cFileName, L".") == 0 ||
                wcscmp(fd.cFileName, L"..") == 0)
                continue;

            std::wstring subFolder = folderPath.wstring() + L"\\" + fd.cFileName;

            // music.xxx を探す
            std::wstring searchMusic = subFolder + L"\\music.*";
            WIN32_FIND_DATAW fd2;
            HANDLE hFind2 = FindFirstFileW(searchMusic.c_str(), &fd2);

            if (hFind2 != INVALID_HANDLE_VALUE)
            {
                json["musics"].push_back({
                    {"folder", ws2utf8(fd.cFileName)},
                    {"file",   ws2utf8(fd2.cFileName)}
                    });

                FindClose(hFind2);
            }
        }

    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    std::ofstream file(folderPath / "musics.json");
    file << json.dump(4);
}


// @brief UTF16からUTF8に変換します。
// @param wstr 変換したい文字列
std::string JsonCreator::ws2utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        nullptr, 0,
        nullptr, nullptr
    );

    std::string result(size, 0);

    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        &result[0], size,
        nullptr, nullptr
    );

    return result;
}
