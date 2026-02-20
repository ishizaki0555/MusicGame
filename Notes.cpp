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

    std::function<void(const nlohmann::json&)> parseNote =
        [&](const nlohmann::json& n)
        {
            int LPB = n.value("LPB", 4);
            int num = n.value("num", 0);
            int block = n.value("block", 0);
            int type = n.value("type", 1);

            // 時間計算
            float beat = (float)num / LPB;
            float time = beat * secPerBeat + (offset * 0.01f);

            Note note;
            note.lane = block;
            note.time = time;
            note.type = type;
            note.endTime = 0;

            // ロングノーツ
            if (n.contains("notes") && !n["notes"].empty())
            {
                auto& child = n["notes"][0];
                int endNum = child.value("num", num);
                float endBeat = (float)endNum / LPB;
                note.endTime = endBeat * secPerBeat + (offset * 0.01f);
            }

            notes.push_back(note);

            // 子ノーツも再帰処理
            if (n.contains("notes"))
            {
                for (auto& c : n["notes"])
                    parseNote(c);
            }
        };

    for (auto& n : json["notes"])
        parseNote(n);
}
