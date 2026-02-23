// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// ResultScene.cpp
// ゲームプレイ後のリザルト画面の機能を提供します
// 
//========================================

#include "ResultScene.h"

// @brief コンストラクタ
// @param score スコア
// @param combo 最大コンボ
// @param perfect perfect判定
// @param great great判定
// @param good good判定
// @param miss miss判定
// @param songName 曲名
// @param banner バナー情報
ResultScene::ResultScene(int score, int combo, int perfect, int great, int good, int miss, const std::string& songName, int banner)
	: score(score), maxCombo(combo), perfectCount(perfect), greatCount(great), goodCount(good), missCount(miss), songName(songName), bannerHandle(banner)
{
}

// @brief 更新します
ResultScene::~ResultScene()
{
}

// @brief 更新します
void ResultScene::Update()
{
	// エンターキーかスペースキーで選曲画面に遷移
	if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_SPACE))
		goNext = true;
}

// @brief 描画します
void ResultScene::Draw()
{
	// 描画を一度リセットする
	ClearDrawScreen();

	// ==============================
	// 背景にバナーを描画する
	// ==============================
	if (bannerHandle != -1)
	{
		DrawExtendGraph(0, 0, 1280, 720, bannerHandle, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ==============================
	// 曲名
	// ==============================
	std::string sjis = Utf8ToSjis(songName);
	DrawString(100, 50, sjis.c_str(), GetColor(255, 255, 255));

	// ==============================
	// バナー描画
	// ==============================
	if (bannerHandle != -1)
	{
		DrawExtendGraph(
			390, 120,
			890, 370,
			bannerHandle,
			TRUE
		);
	}

	// ==============================
	// 判定数描画
	// ==============================
	int y = 400;
	int dy = 40;

	char buf[64];

	// パーフェクト
	sprintf_s(buf, "Perfect   %d", perfectCount);
	DrawString(100, y, buf, GetColor(255, 255, 0)); y += dy;

	// グレート
	sprintf_s(buf, "Great     %d", greatCount);
	DrawString(100, y, buf, GetColor(255, 80, 80)); y += dy;

	// グッド
	sprintf_s(buf, "Good      %d", goodCount);
	DrawString(100, y, buf, GetColor(80, 255, 120)); y += dy;

	// ミス
	sprintf_s(buf, "Miss      %d", missCount);
	DrawString(100, y, buf, GetColor(180, 180, 180)); y += dy;

	// ==============================
	// 最大コンボ
	// ==============================
	sprintf_s(buf, "MaxCombo  %d", maxCombo);
	DrawString(100, y, buf, GetColor(255, 255, 255));

	// ==============================
	// 戻る案内
	// ==============================
	DrawString(500, 600, "PRESS ENTER OR SPACE TO RETURN", GetColor(255, 255, 255));
}

std::string ResultScene::Utf8ToSjis(const std::string& utf8)
{
	// UTF-8 → UTF-16
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	std::wstring wstr(wlen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

	// UTF-16 → Shift-JIS
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string sjis(len, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, nullptr, nullptr);

	return sjis;
}

