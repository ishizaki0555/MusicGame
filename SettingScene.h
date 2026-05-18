// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// SettingScene.h
// 設定画面の機能を提供します
// 
// ========================================

#pragma once
#include <DxLib.h>
#include <vector>
#include <string>

enum class SettingItem
{
	NoteSpeed,
	Resolution,
	ApplyResolution,
	MasterVolume,
	BgmVolume,
	SeVolume,
	Count // 項目数
};

// 設定画面のクラス
class SettingScene
{
public:

	// @brief コンストラクタ
	SettingScene();

	// @brief デストラクタ
	~SettingScene();

	// @brief 更新します
	void Update();

	// @brief 描画します
	void Draw();

	bool goBack = false;		// タイトル画面に戻るフラグ

private:
	int fontHandleSmall;
	int fontHandleLarge;
	
	SettingItem currentItem;
	int currentResIndex;

	// キー入力用のタイマー
	int upTimer;
	int downTimer;
	int leftTimer;
	int rightTimer;
	
	bool prevEnter;
	bool prevSpace;
	bool prevEsc = false;

	int selectSE;

	// UTF-8からShift-JISへの変換
	std::string Utf8ToSjis(const std::string& utf8);
};