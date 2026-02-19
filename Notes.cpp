#include "Notes.h"
#include <filesystem>

void NotesData::LoadFromJson(const nlohmann::json& json)
{
    notes.clear();

    title = json.value("name", "Unknown");
    bpm = json.value("BPM", 0);
    offset = json.value("offset", 0);

    // ÉmÅ[Écì«Ç›çûÇ›
    if (json.contains("notes"))
    {
        for (auto& n : json["notes"])
        {
            Note note;
            note.lane = n.value("lane", 0);
            note.time = n.value("time", 0.0f);
            note.endTime = n.value("endTime", 0.0f);
            note.type = n.value("type", 1);
            notes.push_back(note);
        }
    }
}
