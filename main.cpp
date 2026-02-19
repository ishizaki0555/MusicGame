#include <DxLib.h>
#include "MusicSelectUI.h"
#include "GameScene.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // DXLib 初期化
    SetGraphMode(1280, 720, 32, 30);
    ChangeWindowMode(TRUE); // ウィンドウモード

    SetUseZBuffer3D(TRUE);     // 3D描画を有効化
    SetWriteZBuffer3D(TRUE);   // Zバッファ書き込みを有効化

    if (DxLib_Init() == -1) return -1;

    SetDrawScreen(DX_SCREEN_BACK);

    // ★ 曲フォルダを指定（あなたの環境に合わせて変更）
    MusicSelectUI selectUI("Musics");
    selectUI.LoadFont("Fonts/BIZ-UDMinchoM.ttc", 32);
    selectUI.LoadMusicList();

    GameScene* game = nullptr;

    while (ProcessMessage() == 0)
    {
        ClearDrawScreen();

        // まだゲームに入っていない → 選曲画面
        if (game == nullptr)
        {
            selectUI.Update();
            selectUI.Draw();

            // 選曲画面で GameScene が生成されたら遷移
            if (selectUI.createdScene != nullptr)
            {
                game = selectUI.createdScene;
            }
        }
        else
        {
            // ゲーム画面
            game->Update();
            game->Draw();
        }

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}