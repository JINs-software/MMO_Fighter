#pragma once
#include <unordered_map>
#include <mutex>
#include <iostream>
#include "ArpSpoofer.h"
#include "JBuffer.h"
#include "Protocol.h"

#define DEFAULT_TIMTER_SET 100

struct Point {
	unsigned int X;
	unsigned int Y;
};

struct Player
{
	unsigned int hostID = 0;	// host id
	Point servPoint = { 0, 0 };
	Point clntPoint = { 0, 0 };
	unsigned short port = 0;
	bool bMoveFlag = false;
	BYTE byDir;
	BYTE byHP;

	bool focusOn = false;
	bool crtFlag = false;
	bool finFlag = false;
	bool rstSCFlag = false;
	bool rstCSFlag = false;

	unsigned short frameTimer = 0;

	Player() {}
	Player(unsigned int hostID, Point sp, Point cp, unsigned short pt, BYTE hp)
		: hostID(hostID), servPoint(sp), clntPoint(cp), port(pt), byHP(hp), crtFlag(true), frameTimer(DEFAULT_TIMTER_SET) {}
};

struct PlayerManager {
	ArpSpoofer* capture;
	bool procFlag = false;
	std::unordered_map<unsigned int, Player> players;
	std::mutex playersMtx;
	std::unordered_map<unsigned short, unsigned int> playerPort;

	void SetCapture(ArpSpoofer* capture_) {
		capture = capture_;
	}
	void ProcCapture();

	void RunProcCapture() {
		procFlag = true;
		std::thread thCapture(&PlayerManager::ProcCapture, this);
		thCapture.detach();
	}
	void StopCapture() {
		procFlag = false;
	}

	void FrameMove(BYTE loopDelta);

	// S->C
	void CreatePlayer(unsigned int hostID, Point sp, unsigned short port, BYTE hp) {
		playersMtx.lock();
		if (players.find(hostID) == players.end()) {
			if (playerPort.find(port) != playerPort.end()) {
				playerPort.erase(port);
			}

			players.insert(std::make_pair(hostID, Player(hostID, sp, sp, port, hp)));
			playerPort.insert(std::make_pair(port, hostID));
			//std::cout << "CreatePlayer, id: " << hostID << ", port: " << port << std::endl;
		}
		else {
			players[hostID].servPoint = sp;
		}
		playersMtx.unlock();
	}
	void DeletePlayer(unsigned int hostID) {
		playersMtx.lock();
		if (players.find(hostID) != players.end()) {
			unsigned short port = players[hostID].port;
			playerPort.erase(port);
			players.erase(hostID);
			//std::cout << "DeletePlayer, id: " << hostID << ", port: " << port << std::endl;
		}
		playersMtx.unlock();
	}

	void MovePlayerServ(unsigned int hostID, Point sp) {
		playersMtx.lock();
		if (players.find(hostID) != players.end()) {
			Player& player = players[hostID];
			player.servPoint = sp;
			//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		}
		playersMtx.unlock();
	}
	void StopPlayerServ(unsigned int hostID, Point sp) {
		playersMtx.lock();
		if (players.find(hostID) != players.end()) {
			Player& player = players[hostID];
			player.servPoint = sp;
			//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		}
		playersMtx.unlock();
	}
	void MovePlayerClnt(unsigned short port, Point cp, BYTE dir) {
		playersMtx.lock();
		if (playerPort.find(port) != playerPort.end()) {
			unsigned int hostID = playerPort[port];
			if (players.find(hostID) != players.end()) {
				Player& player = players[hostID];
				player.bMoveFlag = true;
				player.clntPoint = cp;
				player.servPoint = cp;
				player.byDir = dir;
				//std::cout << "MovePlayerClnt, id: " << hostID << ", x: " << player.clntPoint.X << ", y: " << player.clntPoint.Y << std::endl;
			}
		}
		playersMtx.unlock();
	}
	void StopPlayerClnt(unsigned short port, Point cp) {
		playersMtx.lock();
		if (playerPort.find(port) != playerPort.end()) {
			unsigned int hostID = playerPort[port];
			if (players.find(hostID) != players.end()) {
				Player& player = players[hostID];
				player.bMoveFlag = false;
				player.clntPoint = cp;
				player.servPoint = cp;
				//std::cout << "StopPlayerClnt, id: " << hostID << ", x: " << player.clntPoint.X << ", y: " << player.clntPoint.Y << std::endl;
			}
		}
		playersMtx.unlock();
	}
	void DamagePlayer(unsigned int hostID, BYTE hp) {
		playersMtx.lock();
		if (players.find(hostID) != players.end()) {
			Player& player = players[hostID];
			player.byHP = hp;
			//std::cout << "MovePlayerServ, id: " << hostID << ", x: " << player.servPoint.X << ", y: " << player.servPoint.Y << std::endl;
		}
		playersMtx.unlock();
	}
	void SyncPlayer(unsigned int hostID, Point p) {
		playersMtx.lock();
		if (players.find(hostID) != players.end()) {
			Player& player = players[hostID];
			player.servPoint = p;
			player.clntPoint = p;
			//std::cout << "SyncPlayer, id: " << hostID << ", x: " << p.X << ", y: " << p.Y << std::endl;
		}
		playersMtx.unlock();
	}
	
};
