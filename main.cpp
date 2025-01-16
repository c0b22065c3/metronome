#include "DxLib.h"
#include "math.h"
#include "time.h"

// �萔
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define STANDARD_BPM	60	// ��ƂȂ�BPM = 60

#define FONT_SIZE		64	// �t�H���g�̃T�C�Y
#define FONT_THICK		3	// �t�H���g�̑���

// ------------------------------------
// �O���[�o���ϐ���`�]�[��
// ------------------------------------

// �}�E�X���
int MouseX, MouseY;		// �}�E�X��XY���W

// ------------------------------------
// �֐���`�]�[��
// ------------------------------------

// �������擾����֐�
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// �}�E�X�N���b�N���ꂽ���𔻒肷��֐�
BOOL ClickMouse(int button)
{
	switch (button)
	{
		// ���N���b�N�̂Ƃ�
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

// �}�E�X���͈͓��ɓ����Ă��邩�𔻒肷��֐�
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

// �{�^����\������֐�
BOOL DrawButton(int beginX, int beginY, int endX, int endY, unsigned int color, int mouseButton, const char* str = "", unsigned int strColor = 0, int fontHandle = NULL)
{
	int buttonX = endX - beginX;
	int buttonY = endY - beginY;

	// �}�E�X���{�^���͈͓̔��ɂ���Ƃ�
	if (MouseInRange(beginX, beginY, endX, endY))
	{
		// ����\��
		DrawBox(beginX, beginY, endX, endY, color, FALSE);

		// �����̕\��
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, color, fontHandle);

		// �w��̃}�E�X�{�^���������ꂽ��TRUE
		if (ClickMouse(mouseButton))
		{
			return TRUE;
		}
	}
	else
	{
		// ����\���i���߁j
		DrawBox(beginX, beginY, endX, endY, color, TRUE);

		// �����̕\��
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, strColor, fontHandle);
	}

	return FALSE;
}

// �X�N���[���o�[��\������֐�
float DrawScrollBarWidth(int begin, int end, int place, int cursorSize, unsigned int color)
{
	int length = end - begin;
	float cursorLength;

	static int cursorX = end - (length >> 1);
	static int cursorY = place;

	static BOOL isMove = FALSE;

	DrawBox(begin - 4, place - 4, end + 4, place + 4, color, TRUE);

	// �}�E�X��������Ă����Ԃ�
	if (ClickMouse(0))
	{
		// �{�^���͈͓̔��ɂ��邩
		if (MouseInRange(cursorX - cursorSize, cursorY - cursorSize, cursorX + cursorSize, cursorY + cursorSize))
		{
			// ���N���b�N����
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

	// �J�[�\�����}�E�X��X���W�ɍ��킹��
	if (isMove)
	{
		cursorX = MouseX;

		// �͈͊O�Ɏ��߂�
		if (cursorX <= begin)
		{
			cursorX = begin;
		}

		if (cursorX >= end)
		{
			cursorX = end;
		}
	}

	// �J�[�\����\��
	DrawCircle(cursorX, cursorY, cursorSize, color, TRUE);

	// �o�[�̍��͂�����J�[�\���܂ł̒���
	cursorLength = cursorX - begin;

	return cursorLength / (float)length;
}

// ���C���֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɐݒ�
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16); // �𑜓x�̕ύX
	SetMainWindowText("���g���m�[��"); // �E�B���h�E�̃^�C�g����ύX

	if (DxLib_Init() == -1) // DxLib������
	{
		return -1;			// DxLib���������s�Ȃ�\�t�g�I��
	}

	// ------------------------------------
	// �ϐ���`�]�[��
	// ------------------------------------

	// �F
	unsigned int colorBlack = GetColor(0, 0, 0);
	unsigned int colorWhite = GetColor(255, 255, 255);
	
	// ����
	int startTime;					// �Q�[���J�n���Ԃ��L�^����p�̕ϐ�
	int nowTime;					// �Q�[���J�n����̌o�ߎ��Ԃ��L�^����p�̕ϐ�

	int oldTime = 0;				// 1�R���}�O�̎��Ԃ��L�^

	int second;						// �b
	int minute;						// ��

	int bpm = STANDARD_BPM;			// BPM

	float bpmRatio = 1.0f;			// ���BPM�Ƃ̔䗦
	float bpmScroll = 1.0f; 		// BPM�̃X�N���[���o�[�̔䗦

	// �L�[�{�[�h���
	char keyState[256];				// �L�[�{�[�h�����i�[����z��
	char oldKeyState[256];			// �ЂƂO�̃L�[�{�[�h�����i�[����z

	// �}�E�X���
	BOOL isMouseLeft   = FALSE;		// ���N���b�N
	BOOL isMouseMiddle = FALSE;		// ���N���b�N
	BOOL isMouseRight  = FALSE;		// �E�N���b�N

	// �ЂƂO�̃}�E�X���
	BOOL isOldMouseLeft   = FALSE;	// ���N���b�N
	BOOL isOldMouseMiddle = FALSE;	// ���N���b�N
	BOOL isOldMouseRight  = FALSE;	// �E�N���b�N

	// �t���O
	BOOL isClick = FALSE;		// �N���b�N���ꂽ

	// �t�H���g�n���h��
	int buttonFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);

	// ------------------------------------
	// �t�@�C���̓ǂݍ���
	// ------------------------------------

	// �I�[�f�B�I
	//int sound_wood_block;

	int sound_wood_block = LoadSoundMem("Files/Audio/wood-block01.mp3"); // �؋��̉�

	// �摜

	// �ЂƂO�̃L�[�{�[�h�����i�[����z���������
	for (int key = 0; key < 256; key++)
	{
		oldKeyState[key] = 0;
	}

	// �}�E�X���
	SetMouseDispFlag(TRUE);

	// �Q�[���J�n���Ԃ𓾂�
	startTime = GetNowCount();

	// ------------------------------------
	// �����֌W
	// ------------------------------------

	// ����Ⴄ�����𐶐�
	srand((unsigned int)time(NULL));

	// �Q�[�����[�v
	while (ProcessMessage() == 0)
	{
		// �Q�[���J�n����̌o�ߎ��Ԃ��X�V
		nowTime = GetNowCount() - startTime;

		second = nowTime / 1000;
		minute = second / 60;

		// ���݂̃L�[�{�[�h�̏�Ԃ��X�V
		GetHitKeyStateAll(keyState);

		// ���݂̃}�E�X�̏����X�V
		isMouseLeft = ClickMouse(0);
		isMouseMiddle = ClickMouse(1);
		isMouseRight = ClickMouse(2);

		// �}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);

		// ------------------------------------
		// �`�揈��
		// ------------------------------------
		ClearDrawScreen(); // ��ʂ��Ă�����

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

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH * 4 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 6 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "-", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm > 0)
				{
					bpm--;
					//printfDx("�N���b�N�I�I�I�I�I");
				}
			}
		}
		
		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH * 10 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 12 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "+", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				bpm++;
				//printfDx("�N���b�N�I�I�I�I�I");
			}
		}

		// �X�N���[���o�[
		bpmScroll =  DrawScrollBarWidth(SCREEN_WIDTH >> 2, (SCREEN_WIDTH >> 2) * 3, SCREEN_HEIGHT >> 1, 16, colorWhite);

		// �ȈՉ�ʕ\��
		printfDx("%d�b\n", second);
		printfDx("BPM %d\n", bpm);
		printfDx("BPM�̔䗦 %f\n", bpmRatio);
		printfDx("%f\n", 1000 / bpmRatio);
		printfDx("�}�E�X�J�[�\�� X %d Y %d\n", MouseX, MouseY);
		printfDx("�X�N���[�� %f\n", bpmScroll);
		
		// ------------------------------------
		// �㏈��
		// ------------------------------------

		// �L�[�{�[�h�̏���ۑ�
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		// �}�E�X�̏���ۑ�
		isOldMouseLeft = isMouseLeft;
		isOldMouseMiddle = isMouseMiddle;
		isOldMouseRight = isMouseRight;

		oldTime = nowTime;

		ScreenFlip(); // �ߋ���Y�p
		
		// ��ʂ̕����B������
		clsDx();
	}

	DxLib_End(); // DX���C�u�����g�p�̏I������

	return 0; // �\�t�g�I��
}