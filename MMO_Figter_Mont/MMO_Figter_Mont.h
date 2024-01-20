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

// Grid
Grid gGrid;

// Packet Capture
ArpSpoofer servCapture;

// MMO_SERVER
std::string serverIP = "172.30.1.100";
unsigned short listenPort = 20000;

// Player
PlayerManager pMgr;