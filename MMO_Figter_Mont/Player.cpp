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
	unsigned int hostID;
	Player* player = nullptr;
	if (memcmp(packetBundle.ipHdr.srcIP, serverIP, sizeof(serverIP)) == 0) {
		// S->C
		if (playerPort.find(packetBundle.tcpHdr.dest) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.dest]) != players.end()) {
				player = players[playerPort[packetBundle.tcpHdr.dest]];
				hostID = player->hostID;
			}
		}
	}
	else if (memcmp(packetBundle.ipHdr.destIP, serverIP, sizeof(serverIP)) == 0) {
		// C->S
		if (playerPort.find(packetBundle.tcpHdr.source) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.source]) != players.end()) {
				player = players[playerPort[packetBundle.tcpHdr.source]];
				hostID = player->hostID;
			}
		}
	}
	else {
		// Create �޽����̱⿡ ��ϵ� port�� ���� �� �ִ�.
	}
	//if (player == nullptr) {
	//	return;
	//}

	// CreateMsg ���̱⿡ ��ϵ� ��Ʈ�� ���� �� �ִ�. �̷��� ��� �׳� �÷��̾ ���� ���� �� ���� CreateMsg�� ó���ϵ��� �����Ѵ�.
	// ���� �÷��̾� ������ �ִٸ� ���� �Ǵ�.

	if (player != nullptr) {
		WOA_Player_Lock(player);
	}

	bool closeFlag = false;
	if (packetBundle.tcpHdr.fin) {
		if (playerPort.find(packetBundle.tcpHdr.source) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.source]) != players.end()) {
				// Ŭ���̾�Ʈ -> ���� FIN ��Ŷ
				std::cout << "[C->S] FIN" << std::endl;
				//players[playerPort[packetBundle.tcpHdr.source]].finFlag = true;
				//players[playerPort[packetBundle.tcpHdr.source]].crtFlag = false;
				//players[playerPort[packetBundle.tcpHdr.source]].bMoveFlag = false;
				//players[playerPort[packetBundle.tcpHdr.source]].frameTimer = DEFAULT_TIMTER_SET;
				players[playerPort[packetBundle.tcpHdr.source]]->finFlag = true;
				players[playerPort[packetBundle.tcpHdr.source]]->crtFlag = false;
				players[playerPort[packetBundle.tcpHdr.source]]->bMoveFlag = false;
				players[playerPort[packetBundle.tcpHdr.source]]->frameTimer = DEFAULT_TIMTER_SET;
			}

		}
		else if (playerPort.find(packetBundle.tcpHdr.dest) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.dest]) != players.end()) {
				// Ŭ���̾�Ʈ -> ���� FIN ��Ŷ
				std::cout << "[S->C] FIN" << std::endl;
				//players[playerPort[packetBundle.tcpHdr.dest]].finFlag = true;
				//players[playerPort[packetBundle.tcpHdr.dest]].crtFlag = false;
				//players[playerPort[packetBundle.tcpHdr.dest]].bMoveFlag = false;
				//players[playerPort[packetBundle.tcpHdr.dest]].frameTimer = DEFAULT_TIMTER_SET;
				players[playerPort[packetBundle.tcpHdr.dest]]->finFlag = true;
				players[playerPort[packetBundle.tcpHdr.dest]]->crtFlag = false;
				players[playerPort[packetBundle.tcpHdr.dest]]->bMoveFlag = false;
				players[playerPort[packetBundle.tcpHdr.dest]]->frameTimer = DEFAULT_TIMTER_SET;
			}
		}
		closeFlag = true;
	}
	if (packetBundle.tcpHdr.rst) {
		if (playerPort.find(packetBundle.tcpHdr.source) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.source]) != players.end()) {
				// Ŭ���̾�Ʈ -> ���� RST ��Ŷ
				std::cout << "[C->S] RST" << std::endl;
				//players[playerPort[packetBundle.tcpHdr.source]].rstCSFlag = true;
				//players[playerPort[packetBundle.tcpHdr.source]].crtFlag = false;
				//players[playerPort[packetBundle.tcpHdr.source]].bMoveFlag = false;
				//players[playerPort[packetBundle.tcpHdr.source]].frameTimer = DEFAULT_TIMTER_SET;
				players[playerPort[packetBundle.tcpHdr.source]]->rstCSFlag = true;
				players[playerPort[packetBundle.tcpHdr.source]]->crtFlag = false;
				players[playerPort[packetBundle.tcpHdr.source]]->bMoveFlag = false;
				players[playerPort[packetBundle.tcpHdr.source]]->frameTimer = DEFAULT_TIMTER_SET;
			}
		}
		else if (playerPort.find(packetBundle.tcpHdr.dest) != playerPort.end()) {
			if (players.find(playerPort[packetBundle.tcpHdr.dest]) != players.end()) {
				// Ŭ���̾�Ʈ -> ���� RST ��Ŷ
				std::cout << "[S->C] RST" << std::endl;
				//players[playerPort[packetBundle.tcpHdr.dest]].rstSCFlag = true;
				//players[playerPort[packetBundle.tcpHdr.dest]].crtFlag = false;
				//players[playerPort[packetBundle.tcpHdr.dest]].bMoveFlag = false;
				//players[playerPort[packetBundle.tcpHdr.dest]].frameTimer = DEFAULT_TIMTER_SET;
				players[playerPort[packetBundle.tcpHdr.dest]]->rstSCFlag = true;
				players[playerPort[packetBundle.tcpHdr.dest]]->crtFlag = false;
				players[playerPort[packetBundle.tcpHdr.dest]]->bMoveFlag = false;
				players[playerPort[packetBundle.tcpHdr.dest]]->frameTimer = DEFAULT_TIMTER_SET;
			}
		}
		closeFlag = true;
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
				MovePlayerClnt(packetBundle.tcpHdr.source, { X, Y }, Direction);
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
				StopPlayerClnt(packetBundle.tcpHdr.source, { X, Y });
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

	if (player != nullptr) {
		WOA_Player_Unlock(player);
	}
}

void PlayerManager::ProcCapture() {
	while (procFlag) {
		if (!capture->IsEmptyPacketQueue()) {
			stCapturedPacket capPack = capture->GetCapturedPacket();
		
			//////////////////////////////////////////////
			// ProcPacket�� ������ Ǯ�� �۾�(work)�� �Ҵ�
			//////////////////////////////////////////////
			//thPool.Enqueue(&PlayerManager::ProcPacket, this, capPack);
			thPool.Enqueue(&PlayerManager::ProcPacket, this, capPack); // stCapturedPacket ��ü�� ����
		}
	}
}


void PlayerManager::FrameMove(BYTE loopMs) {
	BYTE loopDelta = loopMs / 40;

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
						if (object->clntPoint.X < DELTA_X * (loopDelta)) {			// ���� ������ ���� �߰�
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_LU:
						if (object->clntPoint.X < DELTA_X * (loopDelta)) {
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X * (loopDelta);
						}
						if (object->clntPoint.Y < DELTA_Y * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_UU:
						if (object->clntPoint.Y < DELTA_Y * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RU:
						if (object->clntPoint.X + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X * (loopDelta);
						}
						if (object->clntPoint.Y < DELTA_Y * (loopDelta)) {
							object->clntPoint.Y = dfRANGE_MOVE_TOP;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y -= DELTA_Y * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RR:
						if (object->clntPoint.X + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_RD:
						if (object->clntPoint.X + DELTA_X * (loopDelta) > dfRANGE_MOVE_RIGHT) {
							object->clntPoint.X = dfRANGE_MOVE_RIGHT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X += DELTA_X * (loopDelta);
						}
						if (object->clntPoint.Y + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_DD:
						if (object->clntPoint.Y + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y * (loopDelta);
						}
						break;

					case dfPACKET_MOVE_DIR_LD:
						if (object->clntPoint.X < DELTA_X * (loopDelta)) {
							object->clntPoint.X = dfRANGE_MOVE_LEFT;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.X -= DELTA_X * (loopDelta);
						}
						if (object->clntPoint.Y + DELTA_Y * (loopDelta) > dfRANGE_MOVE_BOTTOM) {
							object->clntPoint.Y = dfRANGE_MOVE_BOTTOM;
							object->bMoveFlag = false;
						}
						else {
							object->clntPoint.Y += DELTA_Y * (loopDelta);
						}
						break;
					default:
						break;
					}
				}
			}

			if (delFlag) {
				iter = players.erase(iter);
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
}