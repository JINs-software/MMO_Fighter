#pragma once
#include "Common_FightGameDamage.h"

namespace FightGameDamage_S2C {

	class Proxy : public JNetProxy
	{
	public: 
		virtual bool DAMAGE(HostID remote, uint32_t attker, uint32_t target, BYTE targetHP);

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

namespace FightGameDamage_C2S {

	class Proxy : public JNetProxy
	{
	public: 

		virtual RpcID* GetRpcList() override { return gRpcList; }
		virtual int GetRpcListCount() override { return gRpcListCount; }
	};
}

