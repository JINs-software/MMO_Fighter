#include "Player.h"
#include "Configuration.h"

// Player �ּҿ� ���� WaitOnAddress Lock �Լ�
void WOA_Player_Lock(Player* pp) {
	uint32 cmp = 1;
	while (true) {
		if (InterlockedExchange(&(pp->p_lock), cmp) == 1) {
			WaitOnAddress(&(pp->p_lock), &cmp, sizeof(pp->p_lock), INFINITE);
		}
		else {
			break;
		}
	}
}
void WOA_Player_Unlock(Player* pp) {
	pp->p_lock = 0;	// ���Ͷ� ��� �ʿ� ����?
	// Unlock�ϴ� ��Ȳ�̶�� �Ӱ� ������ �ϳ��� �����常�� �۵��� �� �ִٴ� ��
	// ���� ���� ������ ������ �ʿ���� ������ ����
	WakeByAddressSingle(&(pp->p_lock));
}

void PlayerManager::ProcPacket(stCapturedPacket packetBundle)
{
	// S->C ���� C->S ���� ��
	bool StoC = false;
	unsigned int hostID;
	unsigned short port = 0;
	Player* player = nullptr;

	// ������ IP �ּҰ� ����-�� �ּҰ� �ƴ϶��, ���� IP ��ġ ���η� S->C���� C->S���� �ĺ�
	if (!loopBackMode) {
		// ��Ŷ source IP == ���� IP
		if (memcmp(packetBundle.ipHdr.srcIP, serverIP, sizeof(serverIP)) == 0) {
			// S->C
			StoC = true;
			if (playerPort.find(packetBundle.tcpHdr.dest, hostID)) {
				port = packetBundle.tcpHdr.dest;
			}
			if (port != 0) {
				if (players.find(hostID, player)) {
					if (player->hostID != hostID) {
						std::cout << "player->hostID != hostID !" << std::endl;
						assert(false);
					}
				}
			}
		}
		// ��Ŷ dest IP == ���� IP
		else if (memcmp(packetBundle.ipHdr.destIP, serverIP, sizeof(serverIP)) == 0) {
			// C->S
			StoC = false;
			if (playerPort.find(packetBundle.tcpHdr.source, hostID)) {
				port = packetBundle.tcpHdr.source;
			}
			if (port != 0) {
				if (players.find(hostID, player)) {
					if (player->hostID != hostID) {
						std::cout << "player->hostID != hostID !" << std::endl;
						assert(false);
					}
				}
			}
		}
	}
	else {
		if (htons(serverPort) == packetBundle.tcpHdr.source) {
			StoC = true;
			if (playerPort.find(packetBundle.tcpHdr.dest, hostID)) {
				port = packetBundle.tcpHdr.dest;
			}
			if (port != 0) {
				if (players.find(hostID, player)) {
					if (player->hostID != hostID) {
						std::cout << "player->hostID != hostID !" << std::endl;
						assert(false);
					}
				}
			}
		}
		else if (htons(serverPort) == packetBundle.tcpHdr.dest) {
			StoC = false;
			if (playerPort.find(packetBundle.tcpHdr.source, hostID)) {
				port = packetBundle.tcpHdr.source;
			}
			if (port != 0) {
				if (players.find(hostID, player)) {
					if (player->hostID != hostID) {
						std::cout << "player->hostID != hostID !" << std::endl;
						assert(false);
					}
				}
			}
		}
	}

	// CreateMsg ���̱⿡ ��ϵ� ��Ʈ�� ���� �� �ִ�. �̷��� ��� �׳� �÷��̾ ���� ���� �� ���� CreateMsg�� ó���ϵ��� �����Ѵ�.
	// ���� �÷��̾� ������ �ִٸ� ���� �Ǵ�.
	bool closeFlag = false;
	if (player != nullptr) {
		//WOA_Player_Lock(player);

		if (packetBundle.tcpHdr.fin) {
			player->finFlag = true;
			player->crtFlag = false;
			player->bMoveFlag = false;
			player->frameTimer = DEFAULT_TIMTER_SET;
			closeFlag = true;
		}
		if (packetBundle.tcpHdr.rst) {
			if (StoC) {
				player->rstSCFlag = true;
				player->crtFlag = false;
				player->bMoveFlag = false;
				player->frameTimer = DEFAULT_TIMTER_SET;
			}
			else {
				player->rstCSFlag = true;
				player->crtFlag = false;
				player->bMoveFlag = false;
				player->frameTimer = DEFAULT_TIMTER_SET;
			}
			closeFlag = true;
		}
	}

	if (!closeFlag) {
		JBuffer jbuff(packetBundle.msgLen);
		jbuff.Enqueue(packetBundle.msg, packetBundle.msgLen);
		while (true) {
			BYTE msgCode;
			BYTE msgLen;
			BYTE msgID;
			if (jbuff.GetUseSize() < sizeof(msgCode) + sizeof(msgLen) + sizeof(msgID)) {
				// ��� �޽��� ���� ����
				break;
			}
			jbuff.Peek(0, reinterpret_cast<BYTE*>(&msgCode), sizeof(msgCode));
			if (msgCode != dfPACKET_CODE) {
				// �ڵ� ����ġ
				break;
			}
			jbuff.Peek(1, reinterpret_cast<BYTE*>(&msgLen), sizeof(msgLen));
			jbuff.Peek(2, reinterpret_cast<BYTE*>(&msgID), sizeof(msgID));

			if (jbuff.GetUseSize() < sizeof(msgCode) + sizeof(msgLen) + sizeof(msgID) + msgLen) {
				// �ٵ� �޽��� ���� ����
				break;
			}

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
				CreatePlayer(ID, { X, Y }, packetBundle.tcpHdr.dest, HP);
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
				if (player != nullptr) {
					MovePlayerClnt(hostID, { X, Y }, Direction);
				}
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
				if (player != nullptr) {
					StopPlayerClnt(hostID, { X, Y });
				}
			}
			break;
			case dfPACKET_SC_DAMAGE:
			{
				BYTE byCode;
				jbuff >> byCode;
				BYTE bySize;
				jbuff >> bySize;
				BYTE byType;
				jbuff >> byType;
				uint32_t attker;
				jbuff >> attker;
				uint32_t target;
				jbuff >> target;
				BYTE targetHP;
				jbuff >> targetHP;
				DamagePlayer(target, targetHP);
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
			{
				jbuff.DirectMoveDequeueOffset(sizeof(msgCode) + sizeof(msgID) + sizeof(msgLen) + msgLen);
			}
			break;
			}
		}
	}

	//if (player != nullptr) {
	//	WOA_Player_Unlock(player);
	//}
}

void PlayerManager::ProcCapture() {
	while (procFlag) {
		//if (!capture->IsEmptyPacketQueue()) {
		//	stCapturedPacket capPack = capture->GetCapturedPacket();
		//
		//	//////////////////////////////////////////////
		//	// ProcPacket�� ������ Ǯ�� �۾�(work)�� �Ҵ�
		//	//////////////////////////////////////////////
		//	//thPool.Enqueue(&PlayerManager::ProcPacket, this, capPack);
		//
		//	ProcPacket(capPack);
		//
		//	//thPool.Enqueue(&PlayerManager::ProcPacket, this, capPack); // stCapturedPacket ��ü�� ����
		//
		//	//ProcPacketFront(capPack);
		//}

		stCapturedPacket capPack = capture->GetCapturedPacket();
		ProcPacket(capPack);
	}
}


void PlayerManager::FrameMove(BYTE loopMs) {
	//SetThreadPriority(GetCurrentThread(), +1);
	BYTE loopDelta = loopMs / 20;
	BYTE overDelta = 0;
	double over = 0;
	
	timeBeginPeriod(1);
	double start = clock();
	while (procFlag) {
		for (auto iter = players.begin(); iter != players.end(); /*iter++*/) {
			bool delFlag = false;

			if (iter->second->rstCSFlag || iter->second->rstSCFlag || iter->second->finFlag) {
				if (iter->second->frameTimer > 0) {
					iter->second->frameTimer--;
				}
				else {
					delFlag = true;
				}
			}
			else {
				if (iter->second->crtFlag) {
					if (iter->second->frameTimer > 0) {
						iter->second->frameTimer--;
					}
					else {
						iter->second->crtFlag = false;
					}
				}
				if (iter->second->bMoveFlag) {
					Player* object = iter->second;
					switch (object->byDir) {
					case dfPACKET_MOVE_DIR_LL:
						if (object->clntPoint.X < DELTA_X_50FPS * (loopDelta)) {			// ���� ������ ���� �߰�
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_LU:
						if (object->clntPoint.X < DELTA_X_50FPS * (loopDelta)) {
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X_50FPS * (loopDelta);
						}
						if (object->clntPoint.Y < DELTA_Y_50FPS * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_UU:
						if (object->clntPoint.Y < DELTA_Y_50FPS * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RU:
						if (object->clntPoint.X + DELTA_X_50FPS * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X_50FPS * (loopDelta);
						}
						if (object->clntPoint.Y < DELTA_Y_50FPS * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RR:
						if (object->clntPoint.X + DELTA_X_50FPS * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RD:
						if (object->clntPoint.X + DELTA_X_50FPS * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X_50FPS * (loopDelta);
						}
						if (object->clntPoint.Y + DELTA_Y_50FPS * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_DD:
						if (object->clntPoint.Y + DELTA_Y_50FPS * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y_50FPS * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_LD:
						if (object->clntPoint.X < DELTA_X_50FPS * (loopDelta)) {
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X_50FPS * (loopDelta);
						}
						if (object->clntPoint.Y + DELTA_Y_50FPS * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y_50FPS * (loopDelta);
						}
						break;
					default:
						break;
					}
				}
			}

			if (delFlag) {
				playerPort.erase(iter->second->port);
				iter = players.erase(iter->first);
				playerPool.ReturnMem((BYTE*)iter->second);
			}
			else {
				iter++;
			}
		}

		double end = clock();
		double duration = end - start;
		if (duration > loopMs) {
			std::cout << "�ð� �ʰ�" << std::endl;
		}
		else {
			Sleep(loopMs - duration);
		}
		start = clock();
	}
	timeEndPeriod(1);
}