// ========================================
// 
// MusicGame Project
// 
// ========================================
// 
// main.cpp
// タイトル → 選曲 → ゲーム → リザルト の各シーンを管理し、
// アプリケーション全体のループとシーン遷移を制御します。
// 
//========================================

#include <DxLib.h>
#include <imgui.h>
#include "MusicSelectUI.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

// シーンの種類
enum class SceneType
{
    TITLE_SCENE,     // タイトル画面
    SELECT_SCENE,    // 選曲画面
    GAME_SCENE,      // ゲームプレイ
    RESULT_SCENE     // リザルト画面
};

SceneType currentScene = SceneType::TITLE_SCENE;

enum class TransitionState {
    None,
    FadeIn,
    FadeOut
};
TransitionState transState = TransitionState::None;
float transProgress = 0.0f;
SceneType nextSceneType = SceneType::TITLE_SCENE;
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // ウィンドウの名前を設定
    SetMainWindowText("MusicGame");

    // DXLib 初期設定
    SetGraphMode(1280, 720, 32, 60);     // 画面サイズ設定
    ChangeWindowMode(TRUE);              // ウィンドウモード

    SetUseZBuffer3D(TRUE);               // Zバッファ使用
    SetWriteZBuffer3D(TRUE);             // Zバッファ書き込み

    if (DxLib_Init() == -1) return -1;   // DXLib 初期化
    SetDrawScreen(DX_SCREEN_BACK);       // 裏画面に描画

    // ImGuiの初期化


    // タイトルシーン生成
    TitleScene* title = new TitleScene();
    // 選曲UI生成
    MusicSelectUI selectUI("Musics");
    // フォント読み込み
    selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
    // 楽曲リスト読み込み
    selectUI.LoadMusicList();

    // ゲームシーン
    GameScene* game = nullptr;
    // リザルトシーン
    ResultScene* result = nullptr;

    // メインループ
    while (ProcessMessage() == 0)
    {
        ClearDrawScreen();

        switch (currentScene)
        {
            case SceneType::TITLE_SCENE:            
                title->Update();
                title->Draw();
                if (transState == TransitionState::None && title->goNext) {
                    nextSceneType = SceneType::SELECT_SCENE;
                    transState = TransitionState::FadeIn;
                }
                break;

            case SceneType::SELECT_SCENE:
                selectUI.Update();
                selectUI.Draw();
                if (transState == TransitionState::None && selectUI.createdScene != nullptr) {
                    nextSceneType = SceneType::GAME_SCENE;
                    transState = TransitionState::FadeIn;
                }
                break;

            case SceneType::GAME_SCENE:
                if (game) {
                    game->Update();
                    game->Draw();
                    if (transState == TransitionState::None && game->IsFinished()) {
                        nextSceneType = SceneType::RESULT_SCENE;
                        transState = TransitionState::FadeIn;
                    }
                }
                break;

            case SceneType::RESULT_SCENE:
                if (result) {
                    result->Update();
                    result->Draw();
                    if (transState == TransitionState::None && result->goNext) {
                        nextSceneType = SceneType::SELECT_SCENE;
                        transState = TransitionState::FadeIn;
                    }
                }
                break;
        }

        if (transState == TransitionState::FadeIn)
        {
            transProgress += 0.04f;
            if (transProgress >= 1.0f)
            {
                transProgress = 1.0f;

                if (currentScene == SceneType::TITLE_SCENE && nextSceneType == SceneType::SELECT_SCENE) 
                {
                    StopSoundMem(title->bgm);
                    selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
                    selectUI.LoadMusicList();
                }
                else if (currentScene == SceneType::SELECT_SCENE && nextSceneType == SceneType::GAME_SCENE) 
                {
                    game = selectUI.createdScene;
                    selectUI.createdScene = nullptr;
                }
                else if (currentScene == SceneType::GAME_SCENE && nextSceneType == SceneType::RESULT_SCENE) 
                {
                    result = new ResultScene(
                        game->GetScore(), game->GetMaxCombo(), game->GetPerfect(),
                        game->GetGreat(), game->GetGood(), game->GetMiss(),
                        game->GetSongName(), game->GetBannerHandle()
                    );
                    delete game;
                    game = nullptr;
                }
                else if (currentScene == SceneType::RESULT_SCENE && nextSceneType == SceneType::SELECT_SCENE) 
                {
                    delete result;
                    result = nullptr;
                    selectUI.sceneStarted = false;
                    selectUI.ResetInputState();
                }
                
                currentScene = nextSceneType;
                transState = TransitionState::FadeOut;
            }
        }
        else if (transState == TransitionState::FadeOut)
        {
            transProgress -= 0.04f;
            if (transProgress <= 0.0f)
            {
                transProgress = 0.0f;
                transState = TransitionState::None;
            }
        }

        if (transState != TransitionState::None)
        {
            int w = 1280;
            int h = 720;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
            if (transState == TransitionState::FadeIn)
            {
                int fillX = (int)(w * (1.0f - transProgress));
                DrawBox(fillX, 0, w, h, GetColor(0, 0, 0), TRUE);
            }
            else if (transState == TransitionState::FadeOut)
            {
                int rightX = (int)(w * transProgress);
                DrawBox(0, 0, rightX, h, GetColor(0, 0, 0), TRUE);
            }
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        ScreenFlip();
    }

    // DXLib終了処理
    DxLib_End();
    return 0;
}

