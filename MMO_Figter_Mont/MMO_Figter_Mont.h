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
// Draw
//HDC g_hMemDC;
//HBITMAP g_hMemDC_Bitmap;
//HBITMAP g_hMemDC_BitmapOld;
//RECT g_MemDC_Rect;

HDC grid_hMemDC;
HBITMAP grid_hMemDC_Bitmap;
HBITMAP grid_hMemDC_BitmapOld;
RECT grid_MemDC_Rect;


//HPEN pen;
//HBRUSH hBrushServ;
//HBRUSH hBrushClnt;

// Grid
Grid gGrid;

// Packet Capture
ArpSpoofer servCapture;

// MMO_SERVER
std::string serverIP = "172.30.1.20";
unsigned short listenPort = 20000;

// Player
PlayerManager pMgr;

// Timer
const unsigned int timer40ms = 1;
const unsigned int timer120ms = 2;
const BYTE WaitMS = 40;
