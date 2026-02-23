// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// ResultScene.h
// ゲームプレイ後のリザルト画面の機能を提供します
// 
//========================================

#pragma once
#include <DxLib.h>
#include <string>

class ResultScene
{
public:
	
	// @brief コンストラクタ
	// @param score スコア
	// @param combo 最大コンボ
	// @param perfect perfect判定
	// @param great great判定
	// @param good good判定
	// @param miss miss判定
	// @param songName 曲名
	// @param banner バナー情報
	ResultScene(int score, int combo, int perfect, int great, int good, int miss, const std::string& songName, int banner);
	~ResultScene();

	// @brief 更新します
	void Update();

	// @brief 描画します
	void Draw();

	// @brief UTF8からShift-JISに変換します
	// @param 変換させたい文字
	std::string Utf8ToSjis(const std::string& utf8);

	bool goNext = false;

private:

	int score;				// スコア
	int maxCombo;			// 最大コンボ
	int bannerHandle;		// バナー情報
	std::string songName;	// 曲名

	int perfectCount = 0;	// Perfect判定
	int greatCount = 0;		// Great判定
	int goodCount = 0;		// Good判定
	int missCount = 0;		// Miss判定
};