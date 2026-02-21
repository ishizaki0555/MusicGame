// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// JsonCrator.h
// 音源をまとめたJsonファイルを作成する機能を提供します。
// 
//========================================

#include <filesystem>
#include <fstream>
#include <windows.h>
#include <json.hpp>

namespace fs = std::filesystem;

class JsonCreator
{
public:

	// @brief 指定されたフォルダパスにmusic.jsonを作成する
	// @param folderPath フォルダパス
	static void CreateMusicJson(const fs::path& folderPath);
	
	// @brief UTF16からUTF8に変換します。
	// @param wstr 変換したい文字列
	static std::string ws2utf8(const std::wstring& wstr);
};