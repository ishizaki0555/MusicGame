// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// Notes.h
// ノーツデータの構造体と、JSONからノーツデータを読み込む機能を提供します。
// 
//========================================

#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

// ノーツ判定用の構造体
struct JudgeNote {
	int lane;               // レーン番号
    int type;               // ノーツの種類
	float time;			    // ノーツの開始時間（秒）
    float endTime;          // ロングノーツの終点時間・通常ノーツは開始時間と同じ
    bool judged = false;    // 判定済みかどうか
};

// ノーツデータ全体を管理するクラス
class NotesData
{
public:
    std::vector<JudgeNote> judgeNotes;              // ノーツのリスト

    std::string musicPath;                          // 音源のファイルパス
    std::string folderPath;                         // 音源のフォルダパス
    std::string title;                              // 曲名
    int bpm = 0;                                    // BPM
    int offset = 0;                                 // オフセット(ms)

    // @brief JSONからノーツデータを読み込む
    // @param json JSONオブジェクト
    void LoadFromJson(const nlohmann::json& json);
};