#include "Notes.h"
#include <filesystem>

void NotesData::LoadFromJson(const nlohmann::json& json)
{
    notes.clear();

    title = json.value("name", "Unknown");
    bpm = json.value("BPM", 0);
    offset = json.value("offset", 0);

    if (!json.contains("notes")) return;

    float secPerBeat = 60.0f / bpm;

    auto calcTime = [&](int num, int LPB)
        {
            float beat = (float)num / LPB;
            return beat * secPerBeat + offset * 0.01f;
        };

    for (auto& n : json["notes"])
    {
        int type = n.value("type", 1);
        int num = n.value("num", 0);
        int block = n.value("block", 1);
        int LPB = n.value("LPB", 4);

        float startTime = calcTime(num, LPB);

        // 通常 or ロング開始
        Note start;
        start.type = type;
        start.lane = block;
        start.endLane = start.lane;
        start.time = startTime;
        start.endTime = startTime;
        notes.push_back(start);

        // ロング終点
        if (n.contains("notes") && !n["notes"].empty())
        {
            auto& child = n["notes"][0];

            int endNum = child.value("num", num);
            int endBlock = child.value("block", block);
            int endLPB = child.value("LPB", LPB);

            float endTime = calcTime(endNum, endLPB);

            Note end;
            end.type = 2;
            end.lane = block;
            end.endLane = endBlock;
            end.time = startTime;
            end.endTime = endTime;
            notes.push_back(end);
        }
    }

    std::sort(notes.begin(), notes.end(), [](auto& a, auto& b) {
        return a.time < b.time;
        });
}