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
#include "Config.h"

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
	judgeFont = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 32, 3);
	scoreFont = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 48, 3);
	songNameFont = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 48, 3);
	comboFont = CreateFontToHandle("Fonts/BIZ-UDMinchoM.ttc", 32, 3);

	// リザルト画面のBGMを再生
	bgmHandle = LoadSoundMem("Sounds/result.wav");
	Config::currentBgmHandle = bgmHandle;
	Config::ApplyVolume();
	PlaySoundMem(bgmHandle, DX_PLAYTYPE_LOOP);
}

// @brief デストラクタ
ResultScene::~ResultScene()
{
}

// @brief 更新します
void ResultScene::Update()
{
	// 現在のキー状態
	bool nowEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0);
	bool nowSpace = (CheckHitKey(KEY_INPUT_SPACE) != 0);

	// 押した瞬間だけ true
	bool enterDown = (nowEnter && !prevEnter);
	bool spaceDown = (nowSpace && !prevSpace);

	// 次フレームのために保存
	prevEnter = nowEnter;
	prevSpace = nowSpace;

	// 決定
	if (enterDown || spaceDown)
	{
		StopSoundMem(bgmHandle);
		goNext = true;
	}
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

	// 半透明に描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// ==============================
	// 曲名
	// ==============================
	std::string sjis = Utf8ToSjis(songName);
	int titleW = GetDrawStringWidth(sjis.c_str(), sjis.size(), songNameFont);
	DrawStringToHandle(640 - titleW / 2, 40, sjis.c_str(), GetColor(255, 255, 255), songNameFont);

	// ==============================
	// バナー描画
	// ==============================
	if (bannerHandle != -1)
	{
		DrawExtendGraph(
			80, 120,
			580, 370,
			bannerHandle,
			TRUE
		);
	}

	// ==============================
	// 最大コンボ
	// ==============================
	char buf[64];
	sprintf_s(buf, "MaxCombo  %d", maxCombo);
	DrawStringToHandle(80, 400, buf, GetColor(255, 255, 255), comboFont);

	// ==============================
	// 判定数描画
	// ==============================
	int baseX = 750;
	int y = 150;
	int dy = 50;

	// パーフェクト
	sprintf_s(buf, "Perfect   %d", perfectCount);
	DrawStringToHandle(baseX, y, buf, judgeTextColor[(int)JudgeNum::PERFECT], judgeFont); y += dy;

	// グレート
	sprintf_s(buf, "Great     %d", greatCount);
	DrawStringToHandle(baseX, y, buf, judgeTextColor[(int)JudgeNum::GREAT], judgeFont); y += dy;

	// グッド
	sprintf_s(buf, "Good      %d", goodCount);
	DrawStringToHandle(baseX, y, buf, judgeTextColor[(int)JudgeNum::GOOD], judgeFont); y += dy;

	// ミス
	sprintf_s(buf, "Miss      %d", missCount);
	DrawStringToHandle(baseX, y, buf, judgeTextColor[(int)JudgeNum::MISS], judgeFont); y += dy;

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

