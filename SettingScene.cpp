#include "SettingScene.h"

// @brief コンストラクタ
SettingScene::SettingScene(){}

// @brief デストラクタ
SettingScene::~SettingScene(){}

// @brief 更新します
void SettingScene::Update()
{
	// ESCで戻る
	if (CheckHitKey(KEY_INPUT_ESCAPE))
		goBack = true;
}

// @brief 描画します
void SettingScene::Draw()
{
	// 背景
	DrawBox(0, 0, 1280, 720, GetColor(20, 20, 20), TRUE);
}
