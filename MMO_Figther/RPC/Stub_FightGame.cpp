#include "Stub_FightGame.h"

namespace FightGameS2C {

	bool Stub::ProcessReceivedMessage(HostID remote, JBuffer& jbuff) {
		while(true) {

		if (jbuff.GetUseSize() < 3) {
			return false;
		}
		BYTE msgLen;
		BYTE msgID;
		jbuff.Peek(1, reinterpret_cast<BYTE*>(&msgLen), sizeof(msgLen));
		jbuff.Peek(2, reinterpret_cast<BYTE*>(&msgID), sizeof(msgID));
		if(jbuff.GetUseSize() < 3 + msgLen) {
			return false;
		}

		switch(static_cast<RpcID>(msgID)) {
		case RPC_CRT_CHARACTER:
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
			CRT_CHARACTER(remote, byCode, bySize, byType, ID, Direction, X, Y, HP);
		}
		break;
		case RPC_CRT_OTHER_CHARACTER:
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
			CRT_OTHER_CHARACTER(remote, byCode, bySize, byType, ID, Direction, X, Y, HP);
		}
		break;
		case RPC_DEL_CHARACTER:
		{
			BYTE byCode;
			jbuff >> byCode;
			BYTE bySize;
			jbuff >> bySize;
			BYTE byType;
			jbuff >> byType;
			uint32_t ID;
			jbuff >> ID;
			DEL_CHARACTER(remote, byCode, bySize, byType, ID);
		}
		break;
		case RPC_MOVE_START:
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
			MOVE_START(remote, byCode, bySize, byType, ID, Direction, X, Y);
		}
		break;
		case RPC_MOVE_STOP:
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
			MOVE_STOP(remote, byCode, bySize, byType, ID, Direction, X, Y);
		}
		break;
		case RPC_ATTACK1:
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
			ATTACK1(remote, byCode, bySize, byType, ID, Direction, X, Y);
		}
		break;
		case RPC_ATTACK2:
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
			ATTACK2(remote, byCode, bySize, byType, ID, Direction, X, Y);
		}
		break;
		case RPC_ATTACK3:
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
			ATTACK3(remote, byCode, bySize, byType, ID, Direction, X, Y);
		}
		break;
		case RPC_DAMAGE:
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
			DAMAGE(remote, byCode, bySize, byType, attker, target, targetHP);
		}
		break;
		case RPC_SYNC:
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
			SYNC(remote, byCode, bySize, byType, ID, X, Y);
		}
		break;
		case RPC_ECHO:
		{
			BYTE byCode;
			jbuff >> byCode;
			BYTE bySize;
			jbuff >> bySize;
			BYTE byType;
			jbuff >> byType;
			uint32_t Time;
			jbuff >> Time;
			ECHO(remote, byCode, bySize, byType, Time);
		}
		break;
		}

		}
	}
}
namespace FightGameC2S {

	bool Stub::ProcessReceivedMessage(HostID remote, JBuffer& jbuff) {
		while(true) {

		if (jbuff.GetUseSize() < 3) {
			return false;
		}
		BYTE msgLen;
		BYTE msgID;
		jbuff.Peek(1, reinterpret_cast<BYTE*>(&msgLen), sizeof(msgLen));
		jbuff.Peek(2, reinterpret_cast<BYTE*>(&msgID), sizeof(msgID));
		if(jbuff.GetUseSize() < 3 + msgLen) {
			return false;
		}

		switch(static_cast<RpcID>(msgID)) {
		case RPC_MOVE_START:
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
			MOVE_START(remote, byCode, bySize, byType, Direction, X, Y);
		}
		break;
		case RPC_MOVE_STOP:
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
			MOVE_STOP(remote, byCode, bySize, byType, Direction, X, Y);
		}
		break;
		case RPC_ATTACK1:
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
			ATTACK1(remote, byCode, bySize, byType, Direction, X, Y);
		}
		break;
		case RPC_ATTACK2:
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
			ATTACK2(remote, byCode, bySize, byType, Direction, X, Y);
		}
		break;
		case RPC_ATTACK3:
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
			ATTACK3(remote, byCode, bySize, byType, Direction, X, Y);
		}
		break;
		case RPC_ECHO:
		{
			BYTE byCode;
			jbuff >> byCode;
			BYTE bySize;
			jbuff >> bySize;
			BYTE byType;
			jbuff >> byType;
			uint32_t Time;
			jbuff >> Time;
			ECHO(remote, byCode, bySize, byType, Time);
		}
		break;
		}

		}
	}
}
