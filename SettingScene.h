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

	float noteSpeed = 1.0f;		// ノーツの速度
	int screenWidth = 1280;		// 画面の幅
	int screenHeight = 720;		// 画面の高さ

	float bgmVolume = 1.0f;		// BGMの音量
	float seVolume = 1.0f;		// SEの音量
	float masterVolume = 1.0f;	// 全体の音量

	bool goBack = false;		// タイトル画面に戻るフラグ
};