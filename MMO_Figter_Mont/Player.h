#pragma once
#include <unordered_map>
#include <mutex>
#include <iostream>
#include "ArpSpoofer.h"
#include "JBuffer.h"
#include "Protocol.h"

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
				if (capPack.msgLen < 3) {
					continue;
				}
				JBuffer jbuff(capPack.msgLen);
				jbuff.Enqueue(capPack.msg, capPack.msgLen);

				BYTE msgCode;
				BYTE msgLen;
				BYTE msgID;
				jbuff.Peek(0, reinterpret_cast<BYTE*>(&msgCode), sizeof(msgCode));
				if (msgCode != dfPACKET_CODE) {
					// 코드 불일치
					continue;
				}
				jbuff.Peek(1, reinterpret_cast<BYTE*>(&msgLen), sizeof(msgLen));
				jbuff.Peek(2, reinterpret_cast<BYTE*>(&msgID), sizeof(msgID));
				
				switch (msgID)
				{
				case dfPACKET_SC_CREATE_MY_CHARACTER:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					BYTE HP;
					jbuff >> HP;
					//CRT_CHARACTER(remote, byCode, bySize, byType, ID, Direction, X, Y, HP);
					CreatePlayer(ID, { X, Y });
				}
				break;
				case dfPACKET_SC_CREATE_OTHER_CHARACTER:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					BYTE HP;
					jbuff >> HP;
					//CRT_OTHER_CHARACTER(remote, byCode, bySize, byType, ID, Direction, X, Y, HP);
				}
				break;
				case dfPACKET_SC_DELETE_CHARACTER:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					//DEL_CHARACTER(remote, byCode, bySize, byType, ID);
					DeletePlayer(ID);
				}
				break;
				case dfPACKET_CS_MOVE_START:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					//MovePlayerClnt(ID,)
				}
				break;
				case dfPACKET_SC_MOVE_START:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					//MOVE_START(remote, byCode, bySize, byType, ID, Direction, X, Y);
					MovePlayerServ(ID, { X, Y });
				}
				break;
				case dfPACKET_CS_MOVE_STOP:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					// ?
				}
				break;
				case dfPACKET_SC_MOVE_STOP:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					BYTE Direction;
					jbuff >> Direction;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					//MOVE_STOP(remote, byCode, bySize, byType, ID, Direction, X, Y);
					StopPlayer(ID, { X, Y });
				}
				break;
				case dfPACKET_SC_SYNC:
				{
					BYTE byCode;
					jbuff >> byCode;
					BYTE bySize;
					jbuff >> bySize;
					BYTE byType;
					jbuff >> byType;
					uint32_t ID;
					jbuff >> ID;
					uint16_t X;
					jbuff >> X;
					uint16_t Y;
					jbuff >> Y;
					//SYNC(remote, byCode, bySize, byType, ID, X, Y);
					SyncPlayer(ID, { X, Y });
				}
				break;
				default:
					break;
				}
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
			std::cout << "CreatePlayer, id: " << hostID << std::endl;
		}
		else {
			players[hostID].servPoint = sp;
		}
	}
	void DeletePlayer(unsigned int hostID) {
		if (players.find(hostID) != players.end()) {
			players.erase(hostID);
			std::cout << "DeletePlayer, id: " << hostID << std::endl;
		}
	}

	void MovePlayerServ(unsigned int hostID, Point sp) {
		if (players.find(hostID) != players.end()) {
			players[hostID].servPoint = sp;
			std::cout << "MovePlayerServ, id: " << hostID << std::endl;
		}
	}
	void StopPlayer(unsigned int hostID, Point sp) {
		if (players.find(hostID) != players.end()) {
			players[hostID].servPoint = sp;
			std::cout << "StopPlayer, id: " << hostID << std::endl;
		}
	}
	void MovePlayerClnt(unsigned int hostID, Point cp) {
		if (players.find(hostID) != players.end()) {
			players[hostID].clntPoint = cp;
		}
	}

	void SyncPlayer(unsigned int hostID, Point p) {
		if (players.find(hostID) != players.end()) {
			players[hostID].servPoint = p;
			players[hostID].clntPoint = p;
			std::cout << "SyncPlayer, id: " << hostID << std::endl;
		}
	}
	
};
