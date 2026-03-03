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
    title = json.value("name", "Unknown");
    bpm = json.value("BPM", 0);
    offset = json.value("offset", 0);

    // 譜面のnumとLPBから時間を計算する
    auto calcTime = [&](int num, int LPB)
        {
            // BPMとLPBから１泊あたりの秒数を計算
            float secPerBeat = 60.0f / bpm;
            return (secPerBeat * (float)num / LPB) + offset * 0.01f;
        };

    // まず原データの block 範囲を調べて正規化方針を決める
    int minBlock = INT_MAX, maxBlock = INT_MIN;
    for (auto& n : json["notes"])
    {
        // 譜面が1..4の範囲なら正規化する。5以上あればそのまま使う
        int block = n.value("block", 1);
        minBlock = std::min(minBlock, block);
        maxBlock = std::max(maxBlock, block);
        
        // 子ノーツのチェック
        if (n.contains("notes") && !n["notes"].empty())
        {
			// 子ノーツも同様に block をチェック
            auto& child = n["notes"][0];
            int cblock = child.value("block", block);
            minBlock = std::min(minBlock, cblock);
            maxBlock = std::max(maxBlock, cblock);
        }
    }
    bool needsSubtract = (minBlock >= 1 && maxBlock <= 4); // 1..4 の譜面なら -1 する

    // ノーツデータを読み込む。子ノーツ(ロングノーツの終点)があればそれも追加する
    for (auto& n : json["notes"])
    {
        // ノーツの基本情報を読み込む
        int type = n.value("type", 1);
        int num = n.value("num", 0);
        int block = n.value("block", 1);
        int LPB = n.value("LPB", 4);

        // 正規化：譜面が 1..4 の場合は 0..3 に変換
        int lane = needsSubtract ? (block - 1) : block;

        float startTime = calcTime(num, LPB);

        // 正規化された情報をJudgeNoteにセットして追加
        JudgeNote jn;
        jn.lane = lane;
        jn.type = type;
        jn.time = startTime;
        jn.endTime = startTime;
        judgeNotes.push_back(jn);

        // 子ノーツ（終点）があれば追加。終点は type=3 として扱う（安全のため）
        if (n.contains("notes") && !n["notes"].empty())
        {
            // 子ノーツの情報を読み込む
            auto& child = n["notes"][0];

            // 子ノーツも正規化する
            int endNum = child.value("num", 0);
            int endBlock = child.value("block", block);
            int endLPB = child.value("LPB", LPB);
            int endLane = needsSubtract ? (endBlock - 1) : endBlock;
            float endTime = calcTime(endNum, endLPB);

            // 正規化された情報をJudgeNoteにセットして追加
            JudgeNote end;
            end.lane = endLane;
            end.type = 3;
            end.time = endTime;
            end.endTime = endTime;
            judgeNotes.push_back(end);
        }
    }

    // 時間でソート
    std::sort(judgeNotes.begin(), judgeNotes.end(),
        [](auto& a, auto& b) { return a.time < b.time; });
}