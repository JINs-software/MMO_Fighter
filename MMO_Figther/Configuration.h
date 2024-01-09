#pragma once

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7

#define SERVER_PORT 5000

#define SLEEP_TIME_MS 20	// 1초당 50프레임 -> 20ms당 1프레임

#define RECV_BUF_SIZE 10000
#define SEND_BUF_SIZE 10000

// 유효성 검사 숫자
#define VALID_PACKET_NUM 0X89

/*
* [클라이언트 그리드 셀 길이 단위
*/
#define dfGridCell_Length 64
#define dfGridCell_Col 12 // 윈도우 좌표 {0, 0} 기준 가로 방향 10개의 cell
#define dfGridCell_Row 10 // 윈도우 좌표 {0, 0} 기준 세로 방향 8개의 cell

/*
* [화면 이동 영역]
* 위 좌표에 가지 못하게 해야하며,
* 해당 좌표에 닿는 경우 움직임을 멈추어야 함.
* (작거나 같으면 멈춤)
*/
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400

/*
* [프레임 당 이동 단위(50FPS 기준)
*/
#define DELTA_X 3
#define DELTA_Y 2

/*
* [이동 오류체크 범위]
*/
#define dfERROR_RANGE		50

/*
* [공격범위]
*/
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20

/*
* [공격 데미지]
*/
#define dfATTACK1_DAMAGE		1
#define dfATTACK2_DAMAGE		2
#define dfATTACK3_DAMAGE		3

/*
* [기본 HP]
*/
#define dfDEAFAULT_INIT_HP		100

