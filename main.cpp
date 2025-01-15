#include "DxLib.h"
#include "math.h"
#include "time.h"

// �萔
#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define STANDARD_BPM	60	// ��ƂȂ�BPM = 60

// ------------------------------------
// �O���[�o���ϐ���`�]�[��
// ------------------------------------

// ------------------------------------
// �֐���`�]�[��
// ------------------------------------

// �������擾����֐�
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// ���C���֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɐݒ�
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16); // �𑜓x�̕ύX

	if (DxLib_Init() == -1) // DxLib������
	{
		return -1;			// DxLib���������s�Ȃ�\�t�g�I��
	}

	// ------------------------------------
	// �ϐ���`�]�[��
	// ------------------------------------

	// �F
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
	char oldKeyState[256];	// �ЂƂO�̃L�[�{�[�h�����i�[����z��

	// �}�E�X���
	int MouseX, MouseY;		// �}�E�X��XY���W

	// �t���O
	BOOL isClick = FALSE;	// �����Ȃ�����

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
			DrawBox(
				SCREEN_WIDTH * 1 >> 2, SCREEN_HEIGHT * 1 >> 2,
				SCREEN_WIDTH * 3 >> 2, SCREEN_HEIGHT * 3 >> 2,
				colorWhite, TRUE
			);

			isClick = TRUE;
		}

		// �ȈՉ�ʕ\��
		printfDx("%d�b\n", second);
		printfDx("BPM %d\n", bpm);
		printfDx("BPM�̔䗦 %f\n", bpmRatio);
		printfDx("%f\n", 1000 / bpmRatio);
		
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