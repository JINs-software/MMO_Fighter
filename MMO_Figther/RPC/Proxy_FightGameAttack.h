#pragma once
#include "Common_FightGameAttack.h"

namespace FightGameAttack_S2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool ATTACK1(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool ATTACK2(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool ATTACK3(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

namespace FightGameAttack_C2S {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool ATTACK1(HostID remote, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool ATTACK2(HostID remote, BYTE Dir, uint16_t X, uint16_t Y);
		virtual bool ATTACK3(HostID remote, BYTE Dir, uint16_t X, uint16_t Y);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

