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
    // 指定のファイルを開くためのハンドルを作成
    HANDLE hFile = CreateFileW(
        path.wstring().c_str(),     // ファイルパス
        GENERIC_READ,               // 読み取りアクセス
        FILE_SHARE_READ,            // 他のプロセスも読み取り可能
        nullptr,                    // セキュリティ属性
		OPEN_EXISTING,			    // 既存のファイルを開く  
        FILE_ATTRIBUTE_NORMAL,      // 通常のファイル属性
        nullptr                     // テンプレートファイルなし
    );

    //  ファイルが開けなかったらからのJsonを返す
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::wcout << L"JSON ファイルを開けません: " << path.wstring() << std::endl;
        return nlohmann::json();
    }

    // ファイルサイズを取得して、バッファを確保
    DWORD size = GetFileSize(hFile, NULL);
    if (size == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return nlohmann::json();
    }

    // ファイル内容を読み取るためのバッファを作成
    std::string buffer(size, '\0');
    DWORD readBytes = 0;

    // ファイルからデータを読み取る
    if (!ReadFile(hFile, buffer.data(), size, &readBytes, NULL))
    {
        CloseHandle(hFile);
        return nlohmann::json();
    }

    // ファイルハンドルを閉じる
    CloseHandle(hFile);

    // Jsonをバースして返す
    try
    {
        return nlohmann::json::parse(buffer);
    }
	// パースエラーが発生した場合はエラーメッセージを表示して空のJsonを返す
    catch (const std::exception& e)
    {
        std::cout << "JSON パースエラー: " << e.what() << std::endl;
        return nlohmann::json();
    }
}
