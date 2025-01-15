#include "DxLib.h"
#include "math.h"
#include "time.h"

// 定数
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define STANDARD_BPM	60	// 基準となるBPM = 60

// ------------------------------------
// グローバル変数定義ゾーン
// ------------------------------------

// ------------------------------------
// 関数定義ゾーン
// ------------------------------------

// 乱数を取得する関数
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// メイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // ウィンドウモードに設定
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16); // 解像度の変更

	if (DxLib_Init() == -1) // DxLib初期化
	{
		return -1;			// DxLib初期化失敗ならソフト終了
	}

	// ------------------------------------
	// 変数定義ゾーン
	// ------------------------------------

	// 色
	unsigned int colorWhite = GetColor(255, 255, 255);
	
	// 時間
	int startTime;			// ゲーム開始時間を記録する用の変数
	int nowTime;			// ゲーム開始からの経過時間を記録する用の変数

	int oldTime = 0;		// 1コンマ前の時間を記録

	int second;				// 秒
	int minute;				// 分

	int bpm = STANDARD_BPM;	// BPM

	float bpmRatio = 1.0f;	// 基準のBPMとの比率

	// キーボード情報
	char keyState[256];		// キーボード情報を格納する配列
	char oldKeyState[256];	// ひとつ前のキーボード情報を格納する配列

	// マウス情報
	int MouseX, MouseY;		// マウスのXY座標

	// フラグ
	BOOL isClick = FALSE;	// 音がなったか

	// ------------------------------------
	// ファイルの読み込み
	// ------------------------------------

	// オーディオ
	//int sound_wood_block;

	int sound_wood_block = LoadSoundMem("Files/Audio/wood-block01.mp3"); // 木魚の音

	// 画像

	// ひとつ前のキーボード情報を格納する配列を初期化
	for (int key = 0; key < 256; key++)
	{
		oldKeyState[key] = 0;
	}

	// マウス情報
	SetMouseDispFlag(TRUE);

	// ゲーム開始時間を得る
	startTime = GetNowCount();

	// ------------------------------------
	// 乱数関係
	// ------------------------------------

	// 毎回違う乱数を生成
	srand((unsigned int)time(NULL));

	// ゲームループ
	while (ProcessMessage() == 0)
	{
		// ゲーム開始からの経過時間を更新
		nowTime = GetNowCount() - startTime;

		second = nowTime / 1000;
		minute = second / 60;

		// 現在のキーボードの状態を更新
		GetHitKeyStateAll(keyState);

		// マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);

		// ------------------------------------
		// 描画処理
		// ------------------------------------
		ClearDrawScreen(); // 画面を焼き払う

		bpmRatio = (float)bpm / (float)STANDARD_BPM;

		if (nowTime % (int)(1000 / bpmRatio) < oldTime % (int)(1000 / bpmRatio))
		{
			PlaySoundMem(sound_wood_block, DX_PLAYTYPE_BACK);
			DrawBox(
				SCREEN_WIDTH * 1 >> 2, SCREEN_HEIGHT * 1 >> 2,
				SCREEN_WIDTH * 3 >> 2, SCREEN_HEIGHT * 3 >> 2,
				colorWhite, TRUE
			);

			isClick = TRUE;
		}

		// 簡易画面表示
		printfDx("%d秒\n", second);
		printfDx("BPM %d\n", bpm);
		printfDx("BPMの比率 %f\n", bpmRatio);
		printfDx("%f\n", 1000 / bpmRatio);
		
		// ------------------------------------
		// 後処理
		// ------------------------------------

		// キーボードの情報を保存
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		oldTime = nowTime;

		ScreenFlip(); // 過去を忘却
		
		// 画面の文字達を消す
		clsDx();
	}

	DxLib_End(); // DXライブラリ使用の終了処理

	return 0; // ソフト終了
}