#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

struct Note
{
    int lane;
    float time;
    float endTime;
    int type;
};

class NotesData
{
public:
    std::vector<Note> notes;

    // DXLib 用に追加
    std::string musicPath;      // 音楽ファイルのフルパス
    std::string folderPath;     // 曲フォルダのパス
    std::string title;          // 曲名
    int bpm = 0;
    int offset = 0;

    void LoadFromJson(const nlohmann::json& json);
};