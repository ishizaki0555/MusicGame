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

// シーンの種類（画面の切り替え用）
enum class SceneType
{
    TITLE_SCENE,     // タイトル画面
    SELECT_SCENE,    // 選曲画面
    GAME_SCENE,      // ゲームプレイ
    RESULT_SCENE     // リザルト画面
};

// 現在表示しているシーン
SceneType currentScene = SceneType::TITLE_SCENE;

// シーン遷移（暗転・明転）の状態を管理する列挙型
enum class TransitionState {
    None,       // 遷移なし（通常状態）
    FadeIn,     // 画面が暗くなる（新しいシーンへの準備）
    FadeOut     // 画面が明るくなる（新しいシーンの開始）
};
TransitionState transState = TransitionState::None;
float transProgress = 0.0f; // 遷移の進行度 (0.0f = 遷移なし, 1.0f = 完了)
SceneType nextSceneType = SceneType::TITLE_SCENE; // 次に切り替わる予定のシーン
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // ウィンドウの名前を設定
    SetMainWindowText("MusicGame");

    // DXLib 初期設定
    SetGraphMode(1280, 720, 32, 60);     // 画面サイズ設定
    ChangeWindowMode(TRUE);              // ウィンドウモード

    SetUseZBuffer3D(TRUE);               // Zバッファ使用
    SetWriteZBuffer3D(TRUE);             // Zバッファ書き込み

    // DXLibの初期化に失敗した場合はプログラムを終了する
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
    // Windowsのメッセージ処理を行い、問題なければループを継続する
    while (ProcessMessage() == 0)
    {
        ClearDrawScreen();

        // 現在のシーン状態に応じて処理を分岐する
        switch (currentScene)
        {
            case SceneType::TITLE_SCENE:            
                title->Update();
                title->Draw();
                // 遷移中でなく、タイトル画面で次へ進むフラグが立っている場合
                if (transState == TransitionState::None && title->goNext) {
                    nextSceneType = SceneType::SELECT_SCENE;
                    transState = TransitionState::FadeIn;
                }
                break;

            case SceneType::SELECT_SCENE:
                selectUI.Update();
                selectUI.Draw();
                // 遷移中でなく、選曲画面からゲームシーンへの遷移条件を満たした場合
                if (transState == TransitionState::None && selectUI.createdScene != nullptr) {
                    nextSceneType = SceneType::GAME_SCENE;
                    transState = TransitionState::FadeIn;
                }
                break;

            case SceneType::GAME_SCENE:
                // ゲームシーンのインスタンスが存在する場合のみ更新・描画を行う
                if (game) {
                    game->Update();
                    game->Draw();
                    // 遷移中でない場合のみ、ゲーム内の状態を確認して次のシーンへ進むか判定する
                    if (transState == TransitionState::None) {
                        if (game->IsFinished()) {
                            nextSceneType = SceneType::RESULT_SCENE;
                            transState = TransitionState::FadeIn;
                        } else if (game->IsRetire()) {
                            nextSceneType = SceneType::SELECT_SCENE;
                            transState = TransitionState::FadeIn;
                        } else if (game->IsRetry()) {
                            nextSceneType = SceneType::GAME_SCENE;
                            transState = TransitionState::FadeIn;
                        }
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

        // フェードイン（画面を暗くしていく処理）
        if (transState == TransitionState::FadeIn)
        {
            transProgress += 0.04f; // 毎フレーム進行度を増やす
            if (transProgress >= 1.0f)
            {
                transProgress = 1.0f; // フェードイン完了

                // --- ここでシーンの切り替えに伴うメモリの解放や再初期化を行う ---
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
                else if (currentScene == SceneType::GAME_SCENE && nextSceneType == SceneType::GAME_SCENE) 
                {
                    GameScene* newGame = new GameScene(game->GetNotesData(), game->GetBannerHandle(), game->GetIsAutoPlay());
                    delete game;
                    game = newGame;
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
                else if ((currentScene == SceneType::RESULT_SCENE || currentScene == SceneType::GAME_SCENE) && nextSceneType == SceneType::SELECT_SCENE) 
                {
                    if (result) {
                        delete result;
                        result = nullptr;
                    }
                    if (game) {
                        delete game;
                        game = nullptr;
                    }
                    selectUI.sceneStarted = false;
                    selectUI.ResetInputState();
                }
                
                currentScene = nextSceneType;
                transState = TransitionState::FadeOut;
            }
        }
        // フェードアウト（画面を明るくしていく処理）
        else if (transState == TransitionState::FadeOut)
        {
            transProgress -= 0.04f;
            // 進行度が0以下になったらフェードアウト完了
            if (transProgress <= 0.0f)
            {
                transProgress = 0.0f;
                transState = TransitionState::None;
            }
        }

        // 遷移中であれば、画面全体にフェード効果のための黒い矩形を描画する
        if (transState != TransitionState::None)
        {
            int w = 1280;
            int h = 720;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
            // フェードイン時は左から右へ黒く塗りつぶす
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

