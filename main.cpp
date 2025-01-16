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

// �{�^����\������֐�
BOOL DrawButton(int beginX, int beginY, int endX, int endY, unsigned int color, int mouseButton, const char* str, unsigned int strColor, int fontHandle)
{
	int buttonX = endX - beginX;
	int buttonY = endY - beginY;

	// �}�E�X���{�^���͈͓̔��ɂ���Ƃ�
	if (MouseX >= beginX && MouseX <= endX && MouseY >= beginY && MouseY <= endY)
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
	int startTime;			// �Q�[���J�n���Ԃ��L�^����p�̕ϐ�
	int nowTime;			// �Q�[���J�n����̌o�ߎ��Ԃ��L�^����p�̕ϐ�

	int oldTime = 0;		// 1�R���}�O�̎��Ԃ��L�^

	int second;				// �b
	int minute;				// ��

	int bpm = STANDARD_BPM;	// BPM

	float bpmRatio = 1.0f;	// ���BPM�Ƃ̔䗦

	// �L�[�{�[�h���
	char keyState[256];		// �L�[�{�[�h�����i�[����z��
	char oldKeyState[256];	// �ЂƂO�̃L�[�{�[�h�����i�[����z

	// �t���O
	BOOL isClick = FALSE;	// �����Ȃ�������

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

			isClick = TRUE;
		}

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH * 4 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 6 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "-", colorBlack, buttonFontHandle))
		{
			bpm--;
			//printfDx("�N���b�N�I�I�I�I�I");
		}
		
		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH * 10 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 12 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "+", colorBlack, buttonFontHandle))
		{
			bpm++;
			//printfDx("�N���b�N�I�I�I�I�I");
		}

		// �ȈՉ�ʕ\��
		printfDx("%d�b\n", second);
		printfDx("BPM %d\n", bpm);
		printfDx("BPM�̔䗦 %f\n", bpmRatio);
		printfDx("%f\n", 1000 / bpmRatio);
		printfDx("�}�E�X�J�[�\�� X %d Y %d\n", MouseX, MouseY);
		
		// ------------------------------------
		// �㏈��
		// ------------------------------------

		// �L�[�{�[�h�̏���ۑ�
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		oldTime = nowTime;

		ScreenFlip(); // �ߋ���Y�p
		
		// ��ʂ̕����B������
		clsDx();
	}

	DxLib_End(); // DX���C�u�����g�p�̏I������

	return 0; // �\�t�g�I��
}