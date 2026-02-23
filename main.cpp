#include <DxLib.h>
#include "MusicSelectUI.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

enum class SceneType
{
    TITLE_SCENE,
    SELECT_SCENE,
    GAME_SCENE,
    RESULT_SCENE
};

SceneType currentScene = SceneType::TITLE_SCENE;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // DXLib 初期化
    SetGraphMode(1280, 720, 32, 30);
    ChangeWindowMode(TRUE); // ウィンドウモード

    SetUseZBuffer3D(TRUE);     // 3D描画を有効化
    SetWriteZBuffer3D(TRUE);   // Zバッファ書き込みを有効化

    if (DxLib_Init() == -1) return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    TitleScene* title = new TitleScene();
    MusicSelectUI selectUI("Musics");
    selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
    selectUI.LoadMusicList();

    GameScene* game = nullptr;
    ResultScene* result = nullptr;

    while (ProcessMessage() == 0)
    {
        ClearDrawScreen();

        switch (currentScene)
        {
        case SceneType::TITLE_SCENE:
            title->Update();
            title->Draw();

            if (title->goNext)
            {
                StopSoundMem(title->bgm);

                selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
                selectUI.LoadMusicList();
                currentScene = SceneType::SELECT_SCENE;
            }
            break;

        case SceneType::SELECT_SCENE:
            selectUI.Update();
            selectUI.Draw();

            if (selectUI.createdScene != nullptr)
            {
                game = selectUI.createdScene;
                selectUI.createdScene = nullptr;
                currentScene = SceneType::GAME_SCENE;
            }
            break;

        case SceneType::GAME_SCENE:
            game->Update();
            game->Draw();

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

                delete game;
                game = nullptr;

                currentScene = SceneType::RESULT_SCENE;
            }
            break;

        case SceneType::RESULT_SCENE:
            result->Update();
            result->Draw();

            if (result->goNext)
            {
                delete result;
                result = nullptr;
                currentScene = SceneType::SELECT_SCENE;
            }
            break;
        }

        ScreenFlip();
    }


    DxLib_End();
    return 0;
}