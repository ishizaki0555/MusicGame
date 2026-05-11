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
#include "imgui/imgui.h"
#include <vector>

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

private:

	float noteSpeed = 1.0f;
	int screenWidth = 1280;
	int screenHeight = 720;

	float bgmVolume = 1.0f;
	float seVolume = 1.0f;
	float masterVolume = 1.0f;
};