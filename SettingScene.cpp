#include "SettingScene.h"
#include "Config.h"
#include <windows.h>
#include <iomanip>
#include <sstream>

std::string SettingScene::Utf8ToSjis(const std::string& utf8)
{
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	std::wstring wstr(wlen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);

	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string sjis(len, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &sjis[0], len, nullptr, nullptr);

	return sjis;
}

SettingScene::SettingScene()
	: currentItem(SettingItem::NoteSpeed)
	, currentResIndex(-1)
	, upTimer(0)
	, downTimer(0)
	, leftTimer(0)
	, rightTimer(0)
	, prevEnter(false)
	, prevSpace(false)
	, prevEsc(CheckHitKey(KEY_INPUT_ESCAPE) != 0)
{
	fontHandleSmall = CreateFontToHandle("Meiryo", 24, 3);
	fontHandleLarge = CreateFontToHandle("Meiryo", 48, 3);

	selectSE = LoadSoundMem("Sounds/select.mp3");
	if (selectSE != -1) {
		int seVol = (int)(255 * Config::seVolume * Config::masterVolume);
		if (seVol < 0) seVol = 0; if (seVol > 255) seVol = 255;
		ChangeVolumeSoundMem(seVol, selectSE);
	}

	if (Config::screenWidth == 640 && Config::screenHeight == 360) currentResIndex = 0;
	else if (Config::screenWidth == 1280 && Config::screenHeight == 720) currentResIndex = 1;
	else if (Config::screenWidth == 1920 && !Config::isFullScreen) currentResIndex = 2;
	else if (Config::screenWidth == 1920 && Config::isFullScreen) currentResIndex = 3;
	else currentResIndex = 0;
}

SettingScene::~SettingScene()
{
	DeleteFontToHandle(fontHandleSmall);
	DeleteFontToHandle(fontHandleLarge);
	DeleteSoundMem(selectSE);
}

void SettingScene::Update()
{
	bool nowEsc = (CheckHitKey(KEY_INPUT_ESCAPE) != 0);
	if (nowEsc && !prevEsc)
	{
		Config::Save();
		goBack = true;
		return;
	}
	prevEsc = nowEsc;

	int up = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
	int down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);
	int left = CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A);
	int right = CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D);
	
	bool nowEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0);
	bool nowSpace = (CheckHitKey(KEY_INPUT_SPACE) != 0);
	bool enterDown = (nowEnter && !prevEnter) || (nowSpace && !prevSpace);
	prevEnter = nowEnter;
	prevSpace = nowSpace;

	const int longPressValue = 30;
	bool moved = false;

	if (up) {
		if (upTimer == 0 || (upTimer > longPressValue && upTimer % 5 == 0)) {
			currentItem = static_cast<SettingItem>((static_cast<int>(currentItem) - 1 + static_cast<int>(SettingItem::Count)) % static_cast<int>(SettingItem::Count));
			moved = true;
		}
		upTimer++;
	} else upTimer = 0;

	if (down) {
		if (downTimer == 0 || (downTimer > longPressValue && downTimer % 5 == 0)) {
			currentItem = static_cast<SettingItem>((static_cast<int>(currentItem) + 1) % static_cast<int>(SettingItem::Count));
			moved = true;
		}
		downTimer++;
	} else downTimer = 0;

	bool valueChanged = false;

	if (left) {
		if (leftTimer == 0 || (leftTimer > longPressValue && leftTimer % 3 == 0)) {
			switch (currentItem) {
			case SettingItem::NoteSpeed: Config::noteSpeed -= 0.1f; if(Config::noteSpeed < 0.5f) Config::noteSpeed = 0.5f; break;
			case SettingItem::Resolution: currentResIndex = (currentResIndex - 1 + 4) % 4; break;
			case SettingItem::MasterVolume: Config::masterVolume -= 0.05f; if(Config::masterVolume < 0.0f) Config::masterVolume = 0.0f; valueChanged = true; break;
			case SettingItem::BgmVolume: Config::bgmVolume -= 0.05f; if(Config::bgmVolume < 0.0f) Config::bgmVolume = 0.0f; valueChanged = true; break;
			case SettingItem::SeVolume: Config::seVolume -= 0.05f; if(Config::seVolume < 0.0f) Config::seVolume = 0.0f; valueChanged = true; break;
			default: break;
			}
			moved = true;
		}
		leftTimer++;
	} else leftTimer = 0;

	if (right) {
		if (rightTimer == 0 || (rightTimer > longPressValue && rightTimer % 3 == 0)) {
			switch (currentItem) {
			case SettingItem::NoteSpeed: Config::noteSpeed += 0.1f; if(Config::noteSpeed > 5.0f) Config::noteSpeed = 5.0f; break;
			case SettingItem::Resolution: currentResIndex = (currentResIndex + 1) % 4; break;
			case SettingItem::MasterVolume: Config::masterVolume += 0.05f; if(Config::masterVolume > 1.0f) Config::masterVolume = 1.0f; valueChanged = true; break;
			case SettingItem::BgmVolume: Config::bgmVolume += 0.05f; if(Config::bgmVolume > 1.0f) Config::bgmVolume = 1.0f; valueChanged = true; break;
			case SettingItem::SeVolume: Config::seVolume += 0.05f; if(Config::seVolume > 1.0f) Config::seVolume = 1.0f; valueChanged = true; break;
			default: break;
			}
			moved = true;
		}
		rightTimer++;
	} else rightTimer = 0;

	if (enterDown && currentItem == SettingItem::ApplyResolution) {
		switch (currentResIndex) {
		case 0: Config::screenWidth = 640; Config::screenHeight = 360; Config::isFullScreen = false; break;
		case 1: Config::screenWidth = 1280; Config::screenHeight = 720; Config::isFullScreen = false; break;
		case 2: Config::screenWidth = 1920; Config::screenHeight = 1080; Config::isFullScreen = false; break;
		case 3: Config::screenWidth = 1920; Config::screenHeight = 1080; Config::isFullScreen = true; break;
		}
		ChangeWindowMode(Config::isFullScreen ? FALSE : TRUE);
		SetWindowSize(Config::screenWidth, Config::screenHeight);
		Config::Save();
		moved = true;
	}

	if (valueChanged) {
		Config::ApplyVolume();
		if (selectSE != -1) {
			int seVol = (int)(255 * Config::seVolume * Config::masterVolume);
			if (seVol < 0) seVol = 0; if (seVol > 255) seVol = 255;
			ChangeVolumeSoundMem(seVol, selectSE);
		}
	}

	if (moved && selectSE != -1) {
		PlaySoundMem(selectSE, DX_PLAYTYPE_BACK);
	}
}

void SettingScene::Draw()
{
	int w, h;
	GetDrawScreenSize(&w, &h);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
	DrawBox(0, 0, w, h, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	std::string title = Utf8ToSjis("Game Settings");
	DrawStringToHandle(100, 50, title.c_str(), GetColor(255, 255, 255), fontHandleLarge);

	int startY = 150;
	int spacing = 60;
	int x = 150;

	auto drawItem = [&](SettingItem item, const char* label, const std::string& valStr) {
		int y = startY + static_cast<int>(item) * spacing;
		bool selected = (currentItem == item);

		if (selected) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
			DrawBox(x - 20, y - 5, x + 600, y + 40, GetColor(100, 150, 255), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		std::string sjisLabel = Utf8ToSjis(label);
		DrawStringToHandle(x, y, sjisLabel.c_str(), GetColor(255, 255, 255), fontHandleSmall);

		if (!valStr.empty()) {
			std::string sjisVal = Utf8ToSjis(valStr);
			DrawStringToHandle(x + 300, y, sjisVal.c_str(), GetColor(200, 255, 200), fontHandleSmall);
		}
	};

	std::stringstream ss;

	ss << std::fixed << std::setprecision(1) << Config::noteSpeed;
	drawItem(SettingItem::NoteSpeed, "Note Speed", ss.str());
	ss.str("");

	const char* resStr[] = { "640x360", "1280x720", "1920x1080", "1920x1080(FullScreen)" };
	drawItem(SettingItem::Resolution, "Screen Resolution", resStr[currentResIndex]);

	drawItem(SettingItem::ApplyResolution, "Apply Resolution", (currentItem == SettingItem::ApplyResolution) ? "[Press Enter]" : "");

	ss << static_cast<int>(Config::masterVolume * 100) << "%";
	drawItem(SettingItem::MasterVolume, "Master Volume", ss.str());
	ss.str("");

	ss << static_cast<int>(Config::bgmVolume * 100) << "%";
	drawItem(SettingItem::BgmVolume, "BGM Volume", ss.str());
	ss.str("");

	ss << static_cast<int>(Config::seVolume * 100) << "%";
	drawItem(SettingItem::SeVolume, "SE Volume", ss.str());
	ss.str("");
	
	std::string guide = Utf8ToSjis("↑↓: Select | ←→: Change Value | Enter: Apply | ESC: Back");
	DrawStringToHandle(100, h - 60, guide.c_str(), GetColor(150, 150, 150), fontHandleSmall);
}
