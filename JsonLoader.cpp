// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// JsonLoader.cpp
// Jsonファイルを読み込む機能を提供します。
// 
//========================================

#include "JsonLoader.h"
#include <windows.h>
#include <string>

// @brief デフォルトコンストラクタ
JsonLoader::JsonLoader(){}

// @brief デフォルトデストラクタ
JsonLoader::~JsonLoader(){}

// @brief 指定されたパスから JSON ファイルを読み込む
// @param path JSON ファイルのパス
nlohmann::json JsonLoader::Load(const std::filesystem::path& path)
{
    HANDLE hFile = CreateFileW(
        path.wstring().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::wcout << L"JSON ファイルを開けません: " << path.wstring() << std::endl;
        return nlohmann::json();
    }

    DWORD size = GetFileSize(hFile, NULL);
    if (size == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return nlohmann::json();
    }

    std::string buffer(size, '\0');
    DWORD readBytes = 0;

    if (!ReadFile(hFile, buffer.data(), size, &readBytes, NULL))
    {
        CloseHandle(hFile);
        return nlohmann::json();
    }

    CloseHandle(hFile);

    try
    {
        return nlohmann::json::parse(buffer);
    }
    catch (const std::exception& e)
    {
        std::cout << "JSON パースエラー: " << e.what() << std::endl;
        return nlohmann::json();
    }
}
