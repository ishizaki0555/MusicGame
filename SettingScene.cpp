#include "SettingScene.h"
#include "Config.h"

SettingScene::SettingScene() {}

SettingScene::~SettingScene() {}

void SettingScene::Update()
{
	// ESCキー押されたらタイトル画面に戻る
	if (CheckHitKey(KEY_INPUT_ESCAPE))
	{
		Config::Save();
		goBack = true;
	}
}

void SettingScene::Draw()
{
	// ImGui設定・描画
	ImGui::Begin("Settings");

	ImGui::Text("GameSettings");
	ImGui::Separator();

	// ノーツの速度設定
	ImGui::SliderFloat("Note Speed", &Config::noteSpeed, 0.5f, 5.0f);

	// ====================
	// 画面サイズ
	// ====================

	// 画面サイズの選択
	static const char* resolutions[] = { "1280 x 720", "1600 x 900", "1920 x 1080" };
	static int currentRes = -1;

	// 初期設定の解像度に合わせる
	if (currentRes == -1)
	{
		if (Config::screenWidth == 1600) currentRes = 1;
		else if (Config::screenWidth == 1920) currentRes = 2;
		else currentRes = 0;
	}

	// 画面サイズのドロップダウンメニュー
	ImGui::Text("Screen Resolution");
	if (ImGui::Combo("Resolution", &currentRes, resolutions, IM_ARRAYSIZE(resolutions)))
	{
		// 選択された解像度に応じて画面サイズ変更
		switch (currentRes)
		{
		case 0: Config::screenWidth = 1280; Config::screenHeight = 720; break;
		case 1: Config::screenWidth = 1600; Config::screenHeight = 900; break;
		case 2: Config::screenWidth = 1920; Config::screenHeight = 1080; break;
		}
	}

	// 画面サイズを反映
	if (ImGui::Button("Apply Resolution"))
	{
		SetGraphMode(Config::screenWidth, Config::screenHeight, 32);
		Config::Save();
	}
	ImGui::Separator();

	// 音量設定
	if (ImGui::SliderFloat("Master Volume", &Config::masterVolume, 0.0f, 1.0f) ||
		ImGui::SliderFloat("BGM Volume", &Config::bgmVolume, 0.0f, 1.0f) ||
		ImGui::SliderFloat("SE Volume", &Config::seVolume, 0.0f, 1.0f))
	{
		Config::ApplyVolume(); // 音量変更時にリアルタイムで適用
	}

	ImGui::Separator();

	ImGui::End();
}
