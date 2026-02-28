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

    for (auto& n : json["notes"])
    {
        int type = n.value("type", 1);
        int num = n.value("num", 0);
        int block = n.value("block", 1);
        int LPB = n.value("LPB", 4);

        float startTime = calcTime(num, LPB);

        JudgeNote jn;
        jn.lane = block;
        jn.type = (type == 1 ? 1 : 2);
        jn.time = startTime;
        jn.endTime = startTime;
        judgeNotes.push_back(jn);

		// ロングノートの終点を探す
        if (n.contains("notes") && !n["notes"].empty())
        {
            auto& child = n["notes"][0];

			int endNum = child.value("num", 0);
            int endBlock = child.value("block", block);
            int endLPB = child.value("LPB", LPB);

            float endTime = calcTime(endNum, endLPB);

            JudgeNote end;
            end.lane = endBlock;
            end.type = 3;
            end.time = endTime;
            end.endTime = endTime;
            judgeNotes.push_back(end);
        }
    }

    // 時間順にソート
    std::sort(judgeNotes.begin(), judgeNotes.end(),
		[](auto& a, auto& b){ return a.time < b.time;});
}