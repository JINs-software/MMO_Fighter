#pragma once

#include "resource.h"

#include <thread>
#include <iostream>
#include "Grid.h"
#include "Player.h"
#include "ArpSpoofer.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")


//////////////////////////
// My Global Variable
//////////////////////////

// 초기값 입력 창
HWND hMainWnd;
HWND hInputDialog;
bool initFlag = false;


// Draw
HDC g_hMemDC;
HBITMAP g_hMemDC_Bitmap;
HBITMAP g_hMemDC_BitmapOld;
RECT g_MemDC_Rect;

// Grid
Grid gGrid;

// Packet Capture
ArpSpoofer servCapture;

// MMO_SERVER
std::string serverIP;// = "172.30.1.20";
unsigned short listenPort;// = 20000;

// Player
PlayerManager playerManager;

// Timer
const unsigned int timerID = 3;
const unsigned int timerMs = 40;
const BYTE WaitMS = 40;
