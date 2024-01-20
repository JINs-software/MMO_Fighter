#pragma once
#include <unordered_map>
#include <mutex>
#include "ArpSpoofer.h"

struct Point {
	unsigned int X;
	unsigned int Y;
};

struct Player
{
	unsigned int hostID;	// host id
	Point servPoint;
	Point clntPoint;

	Player() {}
	Player(unsigned int hostID, Point sp, Point cp)
		: hostID(hostID), servPoint(sp), clntPoint(cp) {}
};

struct PlayerManager {
	ArpSpoofer* capture;
	bool procFlag = false;
	std::unordered_map<unsigned int, Player> players;
	std::mutex pmapMtx;

	void SetCapture(ArpSpoofer* capture_) {
		capture = capture_;
	}
	void ProcCapture() {
		while (procFlag) {
			if (!capture->IsEmptyPacketQueue()) {
				const stCapturedPacket& capPack = capture->GetCapturedPacket();
				//if(capPack.)
			}
		}
	}
	void RunProcCapture() {
		procFlag = true;
		std::thread thCapture(&PlayerManager::ProcCapture, this);
		thCapture.detach();
	}
	void StopCapture() {
		procFlag = false;
	}

	void CreatePlayer(unsigned int hostID, Point sp) {
		if (players.find(hostID) == players.end()) {
			players.insert(std::make_pair(hostID, Player(hostID, sp, sp)));
		}
		else {
			players[hostID].servPoint = sp;
		}
	}
	void DeletePlayer(unsigned int hostID) {
		if (players.find(hostID) != players.end()) {
			players.erase(hostID);
		}
	}

	void MovePlayerServ(unsigned int hostID, Point sp) {
		if (players.find(hostID) != players.end()) {
			players[hostID].servPoint = sp;
		}
	}
	void MovePlayerClnt(unsigned int hostID, Point cp) {
		if (players.find(hostID) != players.end()) {
			players[hostID].clntPoint = cp;
		}
	}
};
