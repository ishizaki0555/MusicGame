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
    // Jsonオブジェクトを作成
    nlohmann::json json;
    json["musics"] = nlohmann::json::array();

    // フォルダ内のサブフォルダを検索
    std::wstring search = folderPath.wstring() + L"\\*";

	// FindFirstFileW と FindNextFileW を使用してサブフォルダを列挙
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search.c_str(), &fd);

	// フォルダが見つからない場合は終了
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

	// サブフォルダをループして、music.xxx を探す
    do
    {
        // ディレクトリかどうかを確認
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
			// "." と ".." をスキップ
            if (wcscmp(fd.cFileName, L".") == 0 ||
                wcscmp(fd.cFileName, L"..") == 0)
                continue;

			// サブフォルダのパスを作成
            std::wstring subFolder = folderPath.wstring() + L"\\" + fd.cFileName;

            // music.xxx を探す
            std::wstring searchMusic = subFolder + L"\\music.*";
            WIN32_FIND_DATAW fd2;
            HANDLE hFind2 = FindFirstFileW(searchMusic.c_str(), &fd2);

			// music.xxx が見つかった場合、Jsonに追加
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

	// ハンドルを閉じる
    FindClose(hFind);

	// Jsonファイルに書き込む
    std::ofstream file(folderPath / "musics.json");
    file << json.dump(4);
}


// @brief UTF16からUTF8に変換します。
// @param wstr 変換したい文字列
std::string JsonCreator::ws2utf8(const std::wstring& wstr)
{
	// 空文字列の場合は空のUTF-8文字列を返す
    if (wstr.empty()) return {};

	// UTF-16 → UTF-8 変換のためのバッファサイズを取得
    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        nullptr, 0,
        nullptr, nullptr
    );

	// 変換後のUTF-8文字列を格納するためのバッファを作成
    std::string result(size, 0);

	// UTF-16 → UTF-8 変換を実行
    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), (int)wstr.size(),
        &result[0], size,
        nullptr, nullptr
    );

    return result;
}
