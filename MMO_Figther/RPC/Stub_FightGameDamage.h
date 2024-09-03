#pragma once
#include "Common_FightGameDamage.h"

namespace FightGameDamage_S2C {

	class Stub : public JNetStub
	{
	public:
		virtual bool DAMAGE(HostID remote, uint32_t attker, uint32_t target, BYTE targetHP) { return false; }
#define JPDEC_FightGameDamage_S2C_DAMAGE bool DAMAGE(HostID remote, uint32_t attker, uint32_t target, BYTE targetHP)
#define JPDEF_FightGameDamage_S2C_DAMAGE(DerivedClass) bool DerivedClass::DAMAGE(HostID remote, uint32_t attker, uint32_t target, BYTE targetHP)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

namespace FightGameDamage_C2S {

	class Stub : public JNetStub
	{
	public:

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

