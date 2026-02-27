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
#include "MusicSelectUI.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

enum class SceneType
{
    TITLE_SCENE,     // タイトル画面
    SELECT_SCENE,    // 選曲画面
    GAME_SCENE,      // ゲームプレイ
    RESULT_SCENE     // リザルト画面
};

SceneType currentScene = SceneType::TITLE_SCENE;   // 現在のシーン

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // DXLib 初期設定
    SetGraphMode(1280, 720, 32, 60);     // 画面サイズ設定
    ChangeWindowMode(TRUE);              // ウィンドウモード

    SetUseZBuffer3D(TRUE);               // Zバッファ使用
    SetWriteZBuffer3D(TRUE);             // Zバッファ書き込み

    if (DxLib_Init() == -1) return -1;   // DXLib 初期化
    SetDrawScreen(DX_SCREEN_BACK);       // 裏画面に描画

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
        // 画面クリア
        ClearDrawScreen();

        // 現在のシーンに応じて処理
        switch (currentScene)
        {
            // タイトルシーン処理
            case SceneType::TITLE_SCENE:            
                title->Update();
                title->Draw();

                // タイトルから進むか判定
                if (title->goNext)
                {
                    // タイトルBGM停止
                    StopSoundMem(title->bgm);

                    selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
                    selectUI.LoadMusicList();
                    // 選曲へ遷移
                    currentScene = SceneType::SELECT_SCENE;
                }
                break;

            // 選曲シーン処理
            case SceneType::SELECT_SCENE:
                selectUI.Update();
                selectUI.Draw();

                // ゲームシーンが生成されたか判定
                if (selectUI.createdScene != nullptr)
                {
                    // 生成されたゲームシーンを受け取る
                    game = selectUI.createdScene;
                    selectUI.createdScene = nullptr;
                    // ゲームへ遷移
                    currentScene = SceneType::GAME_SCENE;
                }
                break;

            // ゲームシーン処理
            case SceneType::GAME_SCENE:
                game->Update();
                game->Draw();

                // ゲーム終了判定
                if (game->IsFinished())
                {
                    result = new ResultScene(
                        game->GetScore(),
                        game->GetMaxCombo(),
                        game->GetPerfect(),
                        game->GetGreat(),
                        game->GetGood(),
                        game->GetMiss(),
                        game->GetSongName(),
                        game->GetBannerHandle()
                    );

                    // ゲームシーン破棄
                    delete game;
                    game = nullptr;
                        
                    // リザルトへ遷移
                    currentScene = SceneType::RESULT_SCENE;
                }
                break;

            // リザルトシーン処理
            case SceneType::RESULT_SCENE:
                result->Update();
                result->Draw();
    
                // リザルトから戻るか判定
                if (result->goNext)
                {
                    // リザルト破棄
                    delete result;
                    result = nullptr;
                    currentScene = SceneType::SELECT_SCENE; // 選曲へ戻る
                }
                break;
        }

        // 画面反映
        ScreenFlip();
    }

    // DXLib終了処理
    DxLib_End();
    return 0;
}
