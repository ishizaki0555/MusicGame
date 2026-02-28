#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

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
    std::vector<JudgeNote> judgeNotes;

    std::string musicPath;
    std::string folderPath;
    std::string title;
    int bpm = 0;
    int offset = 0;

    void LoadFromJson(const nlohmann::json& json);
};