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

// 判定の種類
enum class JudgeNum
{
	PERFECT = 0,	// パーフェクト
	GREAT,			// グレート
	GOOD,			// グッド
	MISS			// ミス
};

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
	
	// @brief デストラクタです
	~ResultScene();

	// @brief 更新します
	void Update();

	// @brief 描画します
	void Draw();

	// @brief UTF8からShift-JISに変換します
	// @param 変換させたい文字
	std::string Utf8ToSjis(const std::string& utf8);

	bool goNext = false;	// 次のシーンに遷移するかのフラグ
	int bgmHandle = -1;     // リザルト画面BGM

	bool prevEnter = false;
	bool prevSpace = false;

private:

	int score;				// スコア
	int maxCombo;			// 最大コンボ
	int bannerHandle;		// バナー情報
	std::string songName;	// 曲名
	JudgeNum judgeNum;		// 判定の種類

	int perfectCount = 0;	// Perfect判定
	int greatCount = 0;		// Great判定
	int goodCount = 0;		// Good判定
	int missCount = 0;		// Miss判定

	// 判定の文字色
	unsigned int judgeTextColor[4] = { GetColor(255, 255, 0) , GetColor(255, 80, 80) , GetColor(80, 255, 120) , GetColor(180, 180, 180) };

	int judgeFont = -1;			// 判定テキスト表示用フォントハンドル
	int scoreFont = -1;			// スコア表示用フォントハンドル
	int songNameFont = -1;		// 曲名表示用フォントハンドル
	int comboFont = -1;			// コンボ表示用フォントハンドル
};