#pragma once
#include "Common_FightGameMove.h"

namespace FightGameMove_S2C {

	class Stub : public JNetStub
	{
	public:
		virtual bool MOVE_START(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameMove_S2C_MOVE_START bool MOVE_START(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameMove_S2C_MOVE_START(DerivedClass) bool DerivedClass::MOVE_START(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool MOVE_STOP(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameMove_S2C_MOVE_STOP bool MOVE_STOP(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameMove_S2C_MOVE_STOP(DerivedClass) bool DerivedClass::MOVE_STOP(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

namespace FightGameMove_C2S {

	class Stub : public JNetStub
	{
	public:
		virtual bool MOVE_START(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameMove_C2S_MOVE_START bool MOVE_START(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameMove_C2S_MOVE_START(DerivedClass) bool DerivedClass::MOVE_START(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool MOVE_STOP(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameMove_C2S_MOVE_STOP bool MOVE_STOP(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameMove_C2S_MOVE_STOP(DerivedClass) bool DerivedClass::MOVE_STOP(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

