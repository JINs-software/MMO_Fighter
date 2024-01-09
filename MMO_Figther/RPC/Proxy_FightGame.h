#pragma once

#include "Common_FightGame.h"

namespace FightGameS2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP);
		virtual bool CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP);
		virtual bool DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID);
		virtual bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP);
		virtual bool SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y);
		virtual bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};

}

namespace FightGameC2S {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y);
		virtual bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};

}

