#include "Player.h"
#include "Configuration.h"

void PlayerManager::ProcCapture() {
	while (procFlag) {
		if (!capture->IsEmptyPacketQueue()) {
			const stCapturedPacket& capPack = capture->GetCapturedPacket();
			if (capPack.tcpHdr.fin) {
				if (playerPort.find(capPack.tcpHdr.source) != playerPort.end()) {
					if (players.find(playerPort[capPack.tcpHdr.source]) != players.end()) {
						// 클라이언트 -> 서버 FIN 패킷
						std::cout << "[C->S] FIN" << std::endl;
						players[playerPort[capPack.tcpHdr.source]].finFlag = true;
						players[playerPort[capPack.tcpHdr.source]].crtFlag = false;
						players[playerPort[capPack.tcpHdr.source]].bMoveFlag = false;
						players[playerPort[capPack.tcpHdr.source]].frameTimer = DEFAULT_TIMTER_SET;
					}
					
				}
				else if (playerPort.find(capPack.tcpHdr.dest) != playerPort.end()) {
					if (players.find(playerPort[capPack.tcpHdr.dest]) != players.end()) {
						// 클라이언트 -> 서버 FIN 패킷
						std::cout << "[S->C] FIN" << std::endl;
						players[playerPort[capPack.tcpHdr.dest]].finFlag = true;
						players[playerPort[capPack.tcpHdr.dest]].crtFlag = false;
						players[playerPort[capPack.tcpHdr.dest]].bMoveFlag = false;
						players[playerPort[capPack.tcpHdr.dest]].frameTimer = DEFAULT_TIMTER_SET;
					}
				}
			}
			if (capPack.tcpHdr.rst) {
				if (playerPort.find(capPack.tcpHdr.source) != playerPort.end()) {
					if (players.find(playerPort[capPack.tcpHdr.source]) != players.end()) {
						// 클라이언트 -> 서버 RST 패킷
						std::cout << "[C->S] RST" << std::endl;
						players[playerPort[capPack.tcpHdr.source]].rstCSFlag = true;
						players[playerPort[capPack.tcpHdr.source]].crtFlag = false;
						players[playerPort[capPack.tcpHdr.source]].bMoveFlag = false;
						players[playerPort[capPack.tcpHdr.source]].frameTimer = DEFAULT_TIMTER_SET;
					}
				}
				else if (playerPort.find(capPack.tcpHdr.dest) != playerPort.end()) {
					if (players.find(playerPort[capPack.tcpHdr.dest]) != players.end()) {
						// 클라이언트 -> 서버 RST 패킷
						std::cout << "[S->C] RST" << std::endl;
						players[playerPort[capPack.tcpHdr.dest]].rstSCFlag = true;
						players[playerPort[capPack.tcpHdr.dest]].crtFlag = false;
						players[playerPort[capPack.tcpHdr.dest]].bMoveFlag = false;
						players[playerPort[capPack.tcpHdr.dest]].frameTimer = DEFAULT_TIMTER_SET;
					}
				}
			}
			if (capPack.msgLen < 3) {
				continue;
			}

			capPack.tcpHdr.source;
			capPack.tcpHdr.dest;

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
				CreatePlayer(ID, { X, Y }, capPack.tcpHdr.dest, HP);
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
			//case dfPACKET_SC_DELETE_CHARACTER:
			//{
			//	BYTE byCode;
			//	jbuff >> byCode;
			//	BYTE bySize;
			//	jbuff >> bySize;
			//	BYTE byType;
			//	jbuff >> byType;
			//	uint32_t ID;
			//	jbuff >> ID;
			//	//DEL_CHARACTER(remote, byCode, bySize, byType, ID);
			//	DeletePlayer(ID);
			//}
			//break;
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
				MovePlayerClnt(capPack.tcpHdr.source, { X, Y }, Direction);
			}
			break;
			//case dfPACKET_SC_MOVE_START:
			//{
			//	BYTE byCode;
			//	jbuff >> byCode;
			//	BYTE bySize;
			//	jbuff >> bySize;
			//	BYTE byType;
			//	jbuff >> byType;
			//	uint32_t ID;
			//	jbuff >> ID;
			//	BYTE Direction;
			//	jbuff >> Direction;
			//	uint16_t X;
			//	jbuff >> X;
			//	uint16_t Y;
			//	jbuff >> Y;
			//	//MOVE_START(remote, byCode, bySize, byType, ID, Direction, X, Y);
			//	MovePlayerServ(ID, { X, Y });
			//}
			//break;
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
				StopPlayerClnt(capPack.tcpHdr.source, {X, Y});
			}
			break;
			//case dfPACKET_SC_MOVE_STOP:
			//{
			//	BYTE byCode;
			//	jbuff >> byCode;
			//	BYTE bySize;
			//	jbuff >> bySize;
			//	BYTE byType;
			//	jbuff >> byType;
			//	uint32_t ID;
			//	jbuff >> ID;
			//	BYTE Direction;
			//	jbuff >> Direction;
			//	uint16_t X;
			//	jbuff >> X;
			//	uint16_t Y;
			//	jbuff >> Y;
			//	//MOVE_STOP(remote, byCode, bySize, byType, ID, Direction, X, Y);
			//	StopPlayerServ(ID, { X, Y });
			//}
			//break;
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
				break;
			}
		}
	}
}


void PlayerManager::FrameMove(BYTE loopDelta) {
	for (auto iter = players.begin(); iter != players.end(); /*iter++*/) {
		bool delFlag = false;

		if (iter->second.crtFlag) {
			if (iter->second.frameTimer > 0) {
				iter->second.frameTimer--;
			}
			else {
				iter->second.crtFlag = false;
			}
		}
		else if (iter->second.rstCSFlag || iter->second.rstSCFlag || iter->second.finFlag) {
			if (iter->second.frameTimer > 0) {
				iter->second.frameTimer--;
			}
			else {
				delFlag = true;
			}
		}
		else if (iter->second.bMoveFlag) {
			Player* object = &iter->second;
			switch (object->byDir) {
			case dfPACKET_MOVE_DIR_LL:
				if (object->clntPoint.X < DELTA_X * (loopDelta)) {			// 오버 프래임 로직 추가
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

		if (delFlag) {
			iter = players.erase(iter);
		}
		else {
			iter++;
		}
	}
}