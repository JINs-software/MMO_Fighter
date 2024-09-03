#pragma once
#include "Common_FightGameComm.h"

namespace FightGameComm_S2C {

	class Stub : public JNetStub
	{
	public:
		virtual bool SYNC(HostID remote, uint32_t ID, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameComm_S2C_SYNC bool SYNC(HostID remote, uint32_t ID, uint16_t X, uint16_t Y)
#define JPDEF_FightGameComm_S2C_SYNC(DerivedClass) bool DerivedClass::SYNC(HostID remote, uint32_t ID, uint16_t X, uint16_t Y)
		virtual bool ECHO(HostID remote, uint32_t Time) { return false; }
#define JPDEC_FightGameComm_S2C_ECHO bool ECHO(HostID remote, uint32_t Time)
#define JPDEF_FightGameComm_S2C_ECHO(DerivedClass) bool DerivedClass::ECHO(HostID remote, uint32_t Time)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

namespace FightGameComm_C2S {

	class Stub : public JNetStub
	{
	public:
		virtual bool ECHO(HostID remote, uint32_t Time) { return false; }
#define JPDEC_FightGameComm_C2S_ECHO bool ECHO(HostID remote, uint32_t Time)
#define JPDEF_FightGameComm_C2S_ECHO(DerivedClass) bool DerivedClass::ECHO(HostID remote, uint32_t Time)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

