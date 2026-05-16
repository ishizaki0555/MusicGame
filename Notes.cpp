// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// Notes.cpp
// ノーツデータの構造体と、JSONからノーツデータを読み込む機能を提供します。
// 
//========================================

#include "Notes.h"
#include <filesystem>

// @brief JSONからノーツデータを読み込む
// @param json JSONオブジェクト
void NotesData::LoadFromJson(const nlohmann::json& json)
{
    // JSONから基本情報を読み込む
    judgeNotes.clear();
    bpmChanges.clear();
    title = json.value("name", "Unknown");
    bpm = json.value("BPM", 0);
    offset = json.value("offset", 0);
    maxLPB = json.value("maxLPB", 24);

    // BPM変更リストの読み込みと時間・拍数の計算
    if (json.contains("bpmChanges")) {
        float currentBeat = 0.0f;
        float currentTime = offset * 0.01f;
        float currentBpm = bpm;

        // まず全て読み込む
        for (auto& bc : json["bpmChanges"]) {
            BpmChange b;
            b.tick = bc.value("tick", 0);
            b.bpm = bc.value("bpm", bpm);
            b.beat = (float)b.tick / maxLPB;
            bpmChanges.push_back(b);
        }

        // 時間順にソートする（通常はすでにソートされているが念のため）
        std::sort(bpmChanges.begin(), bpmChanges.end(), [](auto& a, auto& b) { return a.tick < b.tick; });

        // 各変更位置の絶対時間を計算
        for (auto& bc : bpmChanges) {
            float dBeat = bc.beat - currentBeat;
            float secPerBeat = 60.0f / currentBpm;
            currentTime += dBeat * secPerBeat;
            
            bc.time = currentTime;
            
            currentBeat = bc.beat;
            currentBpm = bc.bpm;
        }
    }

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

    // 4レーン用の譜面かどうかを判定
    bool is4LaneChart = false;
    if (json.contains("maxBlock")) {
        is4LaneChart = (json.value("maxBlock", 6) == 4);
    } else {
        if ((minBlock >= 1 && maxBlock <= 4) || (minBlock == 0 && maxBlock <= 3)) {
            is4LaneChart = true;
        }
    }

    // ノーツデータを読み込む
    for (auto& n : json["notes"])
    {
        int type = n.value("type", 1);
        int num = n.value("num", 0);
        int block = n.value("block", 1);
        int LPB = n.value("LPB", 4);

        // 6レーン基準(0〜5)でのレーン位置計算
        int lane = block;
        if (is4LaneChart) {
            // 4レーン譜面の場合、中央(1,2,3,4)に寄せる
            if (minBlock >= 1) {
                // 原譜面が1〜4の場合
                lane = block; 
            } else {
                // 原譜面が0〜3の場合
                lane = block + 1;
            }
        } else {
            // 6レーン譜面またはその他
            if (minBlock >= 1) {
                lane = block - 1; // 1-indexedなら0-indexedに
            }
        }

        float noteBeat = (float)num / LPB;
        float startTime = GetTimeFromBeat(noteBeat);

        JudgeNote jn;
        jn.lane = lane;
        jn.type = type;
        jn.time = startTime;
        jn.endTime = startTime;
        jn.beat = noteBeat;
        jn.endBeat = noteBeat;
        judgeNotes.push_back(jn);

        if (n.contains("notes") && !n["notes"].empty())
        {
            auto& child = n["notes"][0];

            int endNum = child.value("num", 0);
            int endBlock = child.value("block", block);
            int endLPB = child.value("LPB", LPB);
            
            int endLane = endBlock;
            if (is4LaneChart) {
                if (minBlock >= 1) {
                    endLane = endBlock;
                } else {
                    endLane = endBlock + 1;
                }
            } else {
                if (minBlock >= 1) {
                    endLane = endBlock - 1;
                }
            }

            float endBeat = (float)endNum / endLPB;
            float endTime = GetTimeFromBeat(endBeat);

            JudgeNote end;
            end.lane = endLane;
            end.type = 3;
            end.time = endTime;
            end.endTime = endTime;
            end.beat = endBeat;
            end.endBeat = endBeat;
            judgeNotes.push_back(end);
        }
    }

    std::sort(judgeNotes.begin(), judgeNotes.end(),
        [](auto& a, auto& b) { return a.time < b.time; });
}

// @brief 拍数（ビート）から絶対時間（秒）の計算
float NotesData::GetTimeFromBeat(float beat) const {
    if (bpmChanges.empty()) {
        float secPerBeat = 60.0f / bpm;
        return (secPerBeat * beat) + offset * 0.01f;
    }

    // 対応するBPM区間を探す
    float currentBeat = 0.0f;
    float currentTime = offset * 0.01f;
    float currentBpm = bpm;

    for (const auto& bc : bpmChanges) {
        if (beat <= bc.beat) {
            break;
        }
        currentBeat = bc.beat;
        currentTime = bc.time;
        currentBpm = bc.bpm;
    }

    float dBeat = beat - currentBeat;
    float secPerBeat = 60.0f / currentBpm;
    return currentTime + dBeat * secPerBeat;
}

// @brief 絶対時間（秒）から拍数（ビート）の計算
float NotesData::GetBeatFromTime(float time) const {
    if (bpmChanges.empty()) {
        float secPerBeat = 60.0f / bpm;
        if (secPerBeat == 0.0f) return 0.0f;
        return (time - offset * 0.01f) / secPerBeat;
    }

    // 対応するBPM区間を探す
    float currentBeat = 0.0f;
    float currentTime = offset * 0.01f;
    float currentBpm = bpm;

    for (const auto& bc : bpmChanges) {
        if (time <= bc.time) {
            break;
        }
        currentBeat = bc.beat;
        currentTime = bc.time;
        currentBpm = bc.bpm;
    }

    float dTime = time - currentTime;
    float beatsPerSec = currentBpm / 60.0f;
    return currentBeat + dTime * beatsPerSec;
}
