// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// JsonLoader.h
// Jsonファイルを読み込む機能を提供します。
// 
//========================================

#include <iostream>
#include <string>
#include <fstream>
#include <json.hpp>

// JsonLoader クラス
class JsonLoader
{
public:

	// @brief コンストラクタ
    JsonLoader();

	// @brief デストラクタ
	~JsonLoader();

	// @brief 指定されたパスから JSON ファイルを読み込む
	// @param JSON ファイルのパス
    // @returns 読み込んだJsonデータ
    static nlohmann::json Load(const std::filesystem::path& path);
};