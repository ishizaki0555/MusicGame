#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

struct Note
{
    int lane;       // 開始レーン
    int endLane;    // 終端レーン（ロングノーツ用）
    float time;     // 開始時間
    float endTime;  // 終端時間（通常ノーツは time と同じ）
    int type;       // 1=通常, 2=ロング
    bool judged = false;
};

struct JudgeNote {
    int lane;
    int type;      // 1=通常, 2=ロング開始, 3=ロング終了
    float time;
    float endTime;
    bool judged = false;
};

class NotesData
{
public:
    std::vector<Note> notes;
    std::vector<JudgeNote> judgeNotes;

    std::string musicPath;
    std::string folderPath;
    std::string title;
    int bpm = 0;
    int offset = 0;

    void LoadFromJson(const nlohmann::json& json);
};