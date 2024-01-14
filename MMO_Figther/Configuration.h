#pragma once

//#define DUMB_SPACE_DIV
#define FIXED_GRID_SPACE_DIV

//#define DEFAULT_POOL
#define JINI_POOL

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7

//#define SERVER_PORT 5000

#define SLEEP_TIME_MS 40	// 20fps: 1�ʴ� 25������ -> 40ms�� 1������
							// 50fps: 1�ʴ� 50������ -> 20ms�� 1������

#define RECV_BUF_SIZE 10000
#define SEND_BUF_SIZE 10000

// ��ȿ�� �˻� ����
#define VALID_PACKET_NUM 0X89

/*
* [ȭ�� �̵� ����]
* �� ��ǥ�� ���� ���ϰ� �ؾ��ϸ�,
* �ش� ��ǥ�� ��� ��� �������� ���߾�� ��.
* (�۰ų� ������ ����)
*/
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400

/*
* [Ŭ���̾�Ʈ �׸��� �� ���� ����
*/
#define dfGridCell_Length 64
#define dfGridCell_Col 12 // ������ ��ǥ {0, 0} ���� ���� ���� 10���� cell
#define dfGridCell_Row 10 // ������ ��ǥ {0, 0} ���� ���� ���� 8���� cell

#define dfRANGE_GRID_TOP	0
#define dfRANGE_GRID_BOTTOM	dfRANGE_MOVE_BOTTOM / dfGridCell_Length		// 0 ~ 100
#define dfRANGE_GRID_LEFT	0
#define dfRANGE_GRID_RIGHT	dfRANGE_MOVE_RIGHT / dfGridCell_Length		// 0 ~ 100

/*
* [������ �� �̵� ����(50FPS ����)
*/
#define DELTA_X 6 // 25FPS
#define DELTA_Y 4 // 25FPS
#define DELTA_X_50FPS 3
#define DELTA_Y_50FPS 2
#

/*
* [�̵� ����üũ ����]
*/
#define dfERROR_RANGE		50

/*
* [���ݹ���]
*/
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20

/*
* [���� ������]
*/
#define dfATTACK1_DAMAGE		1
#define dfATTACK2_DAMAGE		2
#define dfATTACK3_DAMAGE		3

/*
* [�⺻ HP]
*/
#define dfDEAFAULT_INIT_HP		100

