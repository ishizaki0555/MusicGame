#include "Notes.h"
#include <filesystem>

void NotesData::LoadFromJson(const nlohmann::json& json)
{
    judgeNotes.clear();

    title = json.value("name", "Unknown");
    bpm = json.value("BPM", 0);
    offset = json.value("offset", 0);

    auto calcTime = [&](int num, int LPB)
        {
            float secPerBeat = 60.0f / bpm;
            return (secPerBeat * (float)num / LPB) + offset * 0.01f;
        };

    // まず原データの block 範囲を調べて正規化方針を決める
    int minBlock = INT_MAX, maxBlock = INT_MIN;
    for (auto& n : json["notes"])
    {
        int block = n.value("block", 1);
        minBlock = std::min(minBlock, block);
        maxBlock = std::max(maxBlock, block);
        if (n.contains("notes") && !n["notes"].empty())
        {
            auto& child = n["notes"][0];
            int cblock = child.value("block", block);
            minBlock = std::min(minBlock, cblock);
            maxBlock = std::max(maxBlock, cblock);
        }
    }
    bool needsSubtract = (minBlock >= 1 && maxBlock <= 4); // 1..4 の譜面なら -1 する

    for (auto& n : json["notes"])
    {
        int type = n.value("type", 1);
        int num = n.value("num", 0);
        int block = n.value("block", 1);
        int LPB = n.value("LPB", 4);

        // 正規化：譜面が 1..4 の場合は 0..3 に変換
        int lane = needsSubtract ? (block - 1) : block;

        float startTime = calcTime(num, LPB);

        JudgeNote jn;
        jn.lane = lane;               // 正規化済み lane
        jn.type = type;               // JSON の type をそのまま保持
        jn.time = startTime;
        jn.endTime = startTime;
        judgeNotes.push_back(jn);

        // 子ノーツ（終点）があれば追加。終点は type=3 として扱う（安全のため）
        if (n.contains("notes") && !n["notes"].empty())
        {
            auto& child = n["notes"][0];

            int endNum = child.value("num", 0);
            int endBlock = child.value("block", block);
            int endLPB = child.value("LPB", LPB);

            int endLane = needsSubtract ? (endBlock - 1) : endBlock;
            float endTime = calcTime(endNum, endLPB);

            JudgeNote end;
            end.lane = endLane;
            end.type = 3; // 終点は明示的に 3
            end.time = endTime;
            end.endTime = endTime;
            judgeNotes.push_back(end);
        }
    }

    // 時間でソート
    std::sort(judgeNotes.begin(), judgeNotes.end(),
        [](auto& a, auto& b) { return a.time < b.time; });
}