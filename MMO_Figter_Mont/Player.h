#pragma once
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <sstream>
#include "ArpSpoofer.h"
#include "JBuffer.h"
#include "JNetPool.h"
#include "Protocol.h"
#include <winnt.h>
#include "ThreadPool.h"
#include <cassert>
#include "ThreadSafeUMap.h"

#pragma comment(lib, "Synchronization.lib")
#pragma comment (lib, "winmm")

#define DEFAULT_TIMTER_SET 100
#define BUFF_SIZE 20000
#define MAXIM_PLAYER_NUM 10000

struct Point {
	unsigned int X;
	unsigned int Y;
};

struct Player
{
	BYTE internal_buff[BUFF_SIZE];
	JBuffer ringBuff;

	unsigned int hostID = 0;	// host id
	Point servPoint = { 0, 0 };
	Point clntPoint = { 0, 0 };
	unsigned short port = 0;
	BYTE byDir;
	BYTE byHP;

	bool binitFlag = false;
	bool bMoveFlag = false;
	bool focusOn = false;
	bool logOn = false;
	bool crtFlag = false;
	bool finFlag = false;
	bool rstSCFlag = false;
	bool rstCSFlag = false;

	unsigned short frameTimer = 0;

	// 락 변수
	uint32 p_lock;

	//JBuffer* buf : f;

	Player() : ringBuff(BUFF_SIZE, internal_buff) {}
	//Player(unsigned int hostID, Point sp, Point cp, unsigned short pt, BYTE hp)
	//	: hostID(hostID), servPoint(sp), clntPoint(cp), port(pt), byHP(hp),
	//	frameTimer(DEFAULT_TIMTER_SET),bMoveFlag(false), focusOn(false), logOn(false), crtFlag(true), finFlag(false), rstSCFlag(false), rstCSFlag(false), p_lock(0) {}
	inline void Set(unsigned int hostID, Point sp, Point cp, unsigned short pt, BYTE hp) {
		if (!binitFlag) {
			this->hostID = hostID;
			this->servPoint = sp;
			this->clntPoint = cp;
			this->port = pt;
			this->byHP = hp;
			this->frameTimer = DEFAULT_TIMTER_SET;

			binitFlag = true;
			bMoveFlag = false;
			focusOn = false;
			logOn = false;
			crtFlag = true;
			finFlag = false;
			rstSCFlag = false;
			rstCSFlag = false;
			p_lock = 0;
		}
	}
};

// Player 주소에 대한 WaitOnAddress Lock 함수
void WOA_Player_Lock(Player* pp);
void WOA_Player_Unlock(Player* pp);

struct PlayerManager {
	ArpSpoofer* capture;

	std::thread thCapture;
	std::thread thFrameMove;

	bool procFlag = true;
	bool loopBackMode = false;
	JiniPool playerPool;	// player 객체를 위한 풀
	ThreadSafeUnorderedMap<unsigned int, Player*> players;
	ThreadSafeUnorderedMap<unsigned short, unsigned> playerPort;

	uint8_t serverIP[4];
	uint16_t serverPort;
	//ThreadPool thPool;

	uint16_t playerCnt = 0;

	PlayerManager() : playerPool(sizeof(Player), MAXIM_PLAYER_NUM)/*, thPool(5)*/ {}
	~PlayerManager() {
		if (thCapture.joinable()) {
			thCapture.join();
		}
		if (thFrameMove.joinable()) {
			thFrameMove.join();
		}
	};

	void SetCapture(ArpSpoofer* capture_) {
		capture = capture_;
	}
	void SetServerAddress(const std::string& serverIPString, unsigned short port) {
		if (serverIPString.compare("127.0.0.1") == 0) {
			loopBackMode = true;
		}
		std::istringstream ss(serverIPString);
		std::string part;
		int i = 0;
		while (std::getline(ss, part, '.') && i < 4) {
			serverIP[i] = static_cast<uint8_t>(std::stoi(part));
			i++;
		}
		
		serverPort = port;
	}

	void ProcPacket(stCapturedPacket packetBundle);
	void ProcCapture();

	void RunProcCapture() {
		thCapture = std::thread(&PlayerManager::ProcCapture, this);
		//thCapture.detach();
	}
	void RunProcFrameMove(BYTE loopMs) {
		thFrameMove = std::thread(&PlayerManager::FrameMove, this, loopMs);
		//thFrameMove.detach();
	}
	void stopProcess() {
		procFlag = false;
	}

	void FrameMove(BYTE loopDelta);

	// S->C
	Player* AllocPlayer() {
		Player* player;
		player = (Player*)playerPool.AllocMem();
		new (player) Player();
		return player;
	}
	void CreatePlayer(unsigned int hostID, Point sp, unsigned short port, BYTE hp) {
		Player* player = AllocPlayer();
		assert(player != nullptr);

		player->Set(hostID, sp, sp, port, hp);
		playerPort.insert(port, hostID);
		players.insert(hostID, player);

		////playersMtx.lock();
		////if (players.find(hostID) == players.end()) {
		//Player* player;
		//if (!players.find(hostID, player)) {
		//	//portMtx.lock();
		//	if (!playerPort.find(port, hostID)) {
		//		playerPort.erase(port);
		//	}
		//	//playerPort[port] = hostID;	// 중복된 키의 경우 값 갱신
		//	playerPort.insert(port, hostID);
		//	//portMtx.unlock();
		//
		//	//players.insert(std::make_pair(hostID, Player(hostID, sp, sp, port, hp)));
		//	//playerPort.insert(std::make_pair(port, hostID));
		//	//std::cout << "CreatePlayer, id: " << hostID << ", port: " << port << std::endl;
		//
		//	Player* player = (Player*)playerPool.AllocMem();
		//	//player->Set(hostID, sp, sp, port, hp);
		//	new (player) Player(hostID, sp, sp, port, hp);
		//	players.insert(hostID, player);
		//
		//	playerCnt++;
		//}
		////else {
		////	// CreatePlayer 요청이 들어왔지만 해당 id의 플레이어가 이미 존재하는 경우
		////	// -> 객체를 삭제한다기보다 객체의 내용을 새로 갱신해주는 방식으로 ..
		////
		////	//players[hostID].servPoint = sp;
		////	//WOA_Player_Lock(players[hostID]);
		////	//players[hostID]->Set(hostID, sp, sp, port, hp);
		////	////WOA_Player_Unlock(players[hostID]);
		////	//playerPort[port] = hostID;	// 중복된 키의 경우 값 갱신
		////
		////	std::cout << "중복 ID 존재!!" << std::endl;
		////	//assert(false);
		////}
		//// ===> 서버에서는 특정 플레이어 주변의 플레이어만 관심을 같도록 범위 내 다른 플레이어들에 대한 Create/Delete를 빈번하게 발생시킨다.
		////playersMtx.unlock();
	}
	//void DeletePlayer(unsigned int hostID) {
	//	//playersMtx.lock();
	//	//if (players.find(hostID) != players.end()) {
	//	//	players.erase(hostID);
	//	//
	//	//	//portMtx.lock();
	//	//	unsigned short port = players[hostID]->port;
	//	//	playerPort.erase(port);
	//	//	//portMtx.unlock();
	//	//	//std::cout << "DeletePlayer, id: " << hostID << ", port: " << port << std::endl;
	//	//}
	//	//playersMtx.unlock();
	//
	//	Player* player;
	//	if (players.find(hostID, player)) {
	//		playerPort.erase(player->port);
	//		players.erase(hostID);
	//		playerPool.ReturnMem((BYTE*)player);
	//	}
	//}
	// ===> 서버에서는 특정 플레이어 주변의 플레이어만 관심을 같도록 범위 내 다른 플레이어들에 대한 Create/Delete를 빈번하게 발생시킨다.

	void MovePlayerServ(unsigned int hostID, Point sp) {
		//playersMtx.lock();
		//if (players.find(hostID) != players.end()) {
		//	//Player& player = players[hostID];
		//	//player.servPoint = sp;
		//	Player* player = players[hostID];
		//	player->servPoint = sp;
		//	//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		//}
		//playersMtx.unlock();

		Player* player;
	}
	void StopPlayerServ(unsigned int hostID, Point sp) {
		//playersMtx.lock();
		//if (players.find(hostID) != players.end()) {
		//	//Player& player = players[hostID];
		//	//player.servPoint = sp;
		//	Player* player = players[hostID];
		//	player->servPoint = sp;
		//	//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		//}
		//playersMtx.unlock();

		Player* player;
		if (players.find(hostID, player)) {
			player->servPoint = sp;
		}
	}
	void MovePlayerClnt(unsigned int hostID, Point cp, BYTE dir) {
		//playersMtx.lock();
		//if (playerPort.find(port) != playerPort.end()) {
		//	unsigned int hostID = playerPort[port];
			//if (players.find(hostID) != players.end()) {
			//	//Player& player = players[hostID];
			//	//player.bMoveFlag = true;
			//	//player.clntPoint = cp;
			//	//player.servPoint = cp;
			//	//player.byDir = dir;
			//	Player* player = players[hostID];
			//	player->bMoveFlag = true;
			//	player->clntPoint = cp;
			//	player->servPoint = cp;
			//	player->byDir = dir;
			//	//std::cout << "MovePlayerClnt, id: " << hostID << ", x: " << player.clntPoint.X << ", y: " << player.clntPoint.Y << std::endl;
			//}
		//}
		//playersMtx.unlock();

		Player* player;
		if (players.find(hostID, player)) {
			player->bMoveFlag = true;
			player->clntPoint = cp;
			player->servPoint = cp;
			player->byDir = dir;
		}
	}
	void StopPlayerClnt(unsigned int hostID, Point cp) {
		//playersMtx.lock();
		//if (playerPort.find(port) != playerPort.end()) {
		//	unsigned int hostID = playerPort[port];
			//if (players.find(hostID) != players.end()) {
			//	//Player& player = players[hostID];
			//	//player.bMoveFlag = false;
			//	//player.clntPoint = cp;
			//	//player.servPoint = cp;
			//	Player* player = players[hostID];
			//	player->bMoveFlag = false;
			//	player->clntPoint = cp;
			//	player->servPoint = cp;
			//	//std::cout << "StopPlayerClnt, id: " << hostID << ", x: " << player.clntPoint.X << ", y: " << player.clntPoint.Y << std::endl;
			//}
		//}
		//playersMtx.unlock();

		Player* player;
		if (players.find(hostID, player)) {
			player->bMoveFlag = false;
			player->clntPoint = cp;
			player->servPoint = cp;
		}
	}
	void DamagePlayer(unsigned int hostID, BYTE hp) {
		//playersMtx.lock();
		//if (players.find(hostID) != players.end()) {
		//	//Player& player = players[hostID];
		//	//player.byHP = hp;
		//	Player* player = players[hostID];
		//	player->byHP = hp;
		//	//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		//}
		//playersMtx.unlock();

		Player* player;
		if (players.find(hostID, player)) {
			player->byHP = hp;
		}
	}
	void SyncPlayer(unsigned int hostID, Point p) {
		//playersMtx.lock();
		//if (players.find(hostID) != players.end()) {
		//	//Player& player = players[hostID];
		//	//player.servPoint = p;
		//	//player.clntPoint = p;
		//	Player* player = players[hostID];
		//	player->servPoint = p;
		//	player->clntPoint = p;
		//	//std::cout << "SyncPlayer, id: " << hostID << ", x: " << p.X << ", y: " << p.Y << std::endl;
		//}
		//playersMtx.unlock();

		Player* player;
		if (players.find(hostID, player)) {
			player->servPoint = p;
			player->clntPoint = p;
		}
	}
	
};
