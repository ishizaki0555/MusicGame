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

// BPM変更の構造体
struct BpmChange {
    int tick;               // JSONでのtick
    float bpm;              // 変更後のBPM
    float time;             // この変更が起こる絶対時間（秒）
    float beat;             // この変更が起こる絶対拍数（ビート）
};

// ノーツ判定用の構造体
struct JudgeNote {
	int lane;               // レーン番号
    int type;               // ノーツの種類
	float time;			    // ノーツの開始時間（秒）
    float endTime;          // ロングノーツの終点時間・通常ノーツは開始時間と同じ
    float beat;             // ノーツの開始拍数（ビート）
    float endBeat;          // ロングノーツの終了拍数（ビート）
    bool judged = false;    // 判定済みかどうか
};

// ノーツデータ全体を管理するクラス
class NotesData
{
public:
    std::vector<JudgeNote> judgeNotes;              // ノーツのリスト
    std::vector<BpmChange> bpmChanges;              // BPM変更のリスト

    std::string musicPath;                          // 音源のファイルパス
    std::string folderPath;                         // 音源のフォルダパス
    std::string title;                              // 曲名
    float bpm = 0;                                  // BPM
    int offset = 0;                                 // オフセット(ms)
    int maxLPB = 24;                                // ソフラン計算用の基準LPB

    // @brief JSONからノーツデータを読み込む
    // @param json JSONオブジェクト
    void LoadFromJson(const nlohmann::json& json);

    // @brief 拍数（ビート）から絶対時間（秒）の計算
    float GetTimeFromBeat(float beat) const;

    // @brief 絶対時間（秒）から拍数（ビート）の計算
    float GetBeatFromTime(float time) const;
};
