#include "DxLib.h"
#include "math.h"
#include "time.h"

// 定数
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define STANDARD_BPM	60	// 基準となるBPM = 60
#define MAX_BPM		400	// 最大BPM = 400

#define FONT_SIZE	64	// フォントのサイズ
#define FONT_THICK	3	// フォントの太さ

// ------------------------------------
// グローバル変数定義ゾーン
// ------------------------------------

// マウス情報
int MouseX, MouseY;		// マウスのXY座標

// ------------------------------------
// 関数定義ゾーン
// ------------------------------------

// 乱数を取得する関数
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// マウスクリックされたかを判定する関数
BOOL ClickMouse(int button)
{
	switch (button)
	{
		// 左クリックのとき
		case 0:
			if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		case 1:
			if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		case 2:
			if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}

		default:
			return FALSE;
	}
}

// マウスが範囲内に入っているかを判定する関数
BOOL MouseInRange(int x1, int y1, int x2, int y2)
{
	if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ボタンを表示する関数
BOOL DrawButton(int beginX, int beginY, int endX, int endY, unsigned int color, int mouseButton, const char* str = "", unsigned int strColor = 0, int fontHandle = NULL)
{
	int buttonX = endX - beginX;
	int buttonY = endY - beginY;

	// マウスがボタンの範囲内にあるとき
	if (MouseInRange(beginX, beginY, endX, endY))
	{
		// 側を表示
		DrawBox(beginX, beginY, endX, endY, color, FALSE);

		// 文字の表示
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, color, fontHandle);

		// 指定のマウスボタンが押されたらTRUE
		if (ClickMouse(mouseButton))
		{
			return TRUE;
		}
	}
	else
	{
		// 側を表示（透過）
		DrawBox(beginX, beginY, endX, endY, color, TRUE);

		// 文字の表示
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, strColor, fontHandle);
	}

	return FALSE;
}

// スクロールバーを表示する関数
float DrawScrollBarWidth(int begin, int end, int place, int cursorSize, unsigned int color, float external)
{
	int length = end - begin;
	float cursorLength;

	static int cursorX = end - (length >> 1);
	static int cursorY = place;

	static BOOL isMove = FALSE;

	DrawBox(begin - 4, place - 4, end + 4, place + 4, color, TRUE);

	// マウスが押されている状態か
	if (ClickMouse(0))
	{
		// ボタンの範囲内にあるか
		if (MouseInRange(cursorX - cursorSize, cursorY - cursorSize, cursorX + cursorSize, cursorY + cursorSize))
		{
			// 左クリックした
			if (ClickMouse(0))
			{
				isMove = TRUE;
			}
		}
	}
	else
	{
		isMove = FALSE;
	}

	// カーソルをマウスのX座標に合わせる
	if (isMove)
	{
		cursorX = MouseX;

		// 範囲外に収める
		if (cursorX <= begin)
		{
			cursorX = begin;
		}

		if (cursorX >= end)
		{
			cursorX = end;
		}
	}
	else
	{
		cursorX = (int)((float)length * external) + begin;
	}

	// カーソルを表示
	DrawCircle(cursorX, cursorY, cursorSize, color, TRUE);

	// バーの左はじからカーソルまでの長さ
	cursorLength = cursorX - begin;

	return cursorLength / (float)length;
}

// メイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // ウィンドウモードに設定
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16); // 解像度の変更
	SetMainWindowText("メトロノーム"); // ウィンドウのタイトルを変更

	if (DxLib_Init() == -1) // DxLib初期化
	{
		return -1;			// DxLib初期化失敗ならソフト終了
	}

	// ------------------------------------
	// 変数定義ゾーン
	// ------------------------------------

	// 色
	unsigned int colorBlack = GetColor(0, 0, 0);
	unsigned int colorWhite = GetColor(255, 255, 255);
	
	// 時間
	int startTime;					// ゲーム開始時間を記録する用の変数
	int nowTime;					// ゲーム開始からの経過時間を記録する用の変数

	int oldTime = 0;				// 1コンマ前の時間を記録

	int second;						// 秒
	int minute;						// 分

	int bpm = STANDARD_BPM;			// BPM

	float bpmRatio = 1.0f;			// 基準のBPMとの比率
	float bpmScroll = 1.0f; 		// BPMのスクロールバーの比率

	// キーボード情報
	char keyState[256];				// キーボード情報を格納する配列
	char oldKeyState[256];			// ひとつ前のキーボード情報を格納する配

	// マウス情報
	BOOL isMouseLeft   = FALSE;		// 左クリック
	BOOL isMouseMiddle = FALSE;		// 中クリック
	BOOL isMouseRight  = FALSE;		// 右クリック

	// ひとつ前のマウス情報
	BOOL isOldMouseLeft   = FALSE;	// 左クリック
	BOOL isOldMouseMiddle = FALSE;	// 中クリック
	BOOL isOldMouseRight  = FALSE;	// 右クリック

	// フラグ
	BOOL isClick = FALSE;		// クリックされた

	// フォントハンドル
	int buttonFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);

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

		// 現在のマウスの情報を更新
		isMouseLeft = ClickMouse(0);
		isMouseMiddle = ClickMouse(1);
		isMouseRight = ClickMouse(2);

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
			/*
			DrawBox(
				SCREEN_WIDTH * 1 >> 2, SCREEN_HEIGHT * 1 >> 2,
				SCREEN_WIDTH * 3 >> 2, SCREEN_HEIGHT * 3 >> 2,
				colorWhite, TRUE
			);
			*/
		}

		// 左のボタン
		if (DrawButton(SCREEN_WIDTH * 4 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 6 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "-", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm > 0)
				{
					bpm--;
					//printfDx("クリック！！！！！");
				}
			}
		}
		
		// 右のボタン
		if (DrawButton(SCREEN_WIDTH * 10 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 12 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "+", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				bpm++;
				//printfDx("クリック！！！！！");
			}
		}

		// BPMのスクロールバー
		bpmScroll =  DrawScrollBarWidth(SCREEN_WIDTH >> 2, (SCREEN_WIDTH >> 2) * 3, SCREEN_HEIGHT >> 1, 16, colorWhite, (float)bpm / (float)MAX_BPM);

		// スクロールバーからBPMに代入
		bpm = MAX_BPM * bpmScroll;

		// 簡易画面表示
		printfDx("%d秒\n", second);
		printfDx("BPM %d\n", bpm);
		printfDx("BPMの比率 %f\n", bpmRatio);
		printfDx("%f\n", 1000 / bpmRatio);
		printfDx("マウスカーソル X %d Y %d\n", MouseX, MouseY);
		printfDx("スクロール %f\n", bpmScroll);
		
		// ------------------------------------
		// 後処理
		// ------------------------------------

		// キーボードの情報を保存
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		// マウスの情報を保存
		isOldMouseLeft = isMouseLeft;
		isOldMouseMiddle = isMouseMiddle;
		isOldMouseRight = isMouseRight;

		oldTime = nowTime;

		ScreenFlip(); // 過去を忘却
		
		// 画面の文字達を消す
		clsDx();
	}

	DxLib_End(); // DXライブラリ使用の終了処理

	return 0; // ソフト終了
}
