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

// @brief デフォルトコンストラクタ
JsonLoader::JsonLoader(){}

// @brief デフォルトデストラクタ
JsonLoader::~JsonLoader(){}

// @brief 指定されたパスから JSON ファイルを読み込む
// @param path JSON ファイルのパス
nlohmann::json JsonLoader::Load(const std::filesystem::path& path)
{
	// ファイルを開く
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
		// ファイルが開けなかった場合のエラーハンドリング
        std::cout << "JSON ファイルを開けません: " << path << std::endl;
        return nlohmann::json(); // 空の JSON を返す
    }

	// JSON データをパース
    nlohmann::json data;
    try
    {
        file >> data;
    }
	// JSON パースエラーのキャッチ
    catch (const std::exception& e)
    {
        std::cout << "JSON パースエラー: " << e.what() << std::endl;
        return nlohmann::json(); // パース失敗時も空を返す
    }

    return data;
}

