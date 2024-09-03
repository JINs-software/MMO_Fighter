#pragma once
#include "Common_FightGameAttack.h"

namespace FightGameAttack_S2C {

	class Stub : public JNetStub
	{
	public:
		virtual bool ATTACK1(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_S2C_ATTACK1 bool ATTACK1(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_S2C_ATTACK1(DerivedClass) bool DerivedClass::ATTACK1(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool ATTACK2(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_S2C_ATTACK2 bool ATTACK2(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_S2C_ATTACK2(DerivedClass) bool DerivedClass::ATTACK2(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool ATTACK3(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_S2C_ATTACK3 bool ATTACK3(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_S2C_ATTACK3(DerivedClass) bool DerivedClass::ATTACK3(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

namespace FightGameAttack_C2S {

	class Stub : public JNetStub
	{
	public:
		virtual bool ATTACK1(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_C2S_ATTACK1 bool ATTACK1(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_C2S_ATTACK1(DerivedClass) bool DerivedClass::ATTACK1(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool ATTACK2(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_C2S_ATTACK2 bool ATTACK2(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_C2S_ATTACK2(DerivedClass) bool DerivedClass::ATTACK2(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
		virtual bool ATTACK3(HostID remote, BYTE Dir, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameAttack_C2S_ATTACK3 bool ATTACK3(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)
#define JPDEF_FightGameAttack_C2S_ATTACK3(DerivedClass) bool DerivedClass::ATTACK3(HostID remote, BYTE Dir, uint16_t X, uint16_t Y)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

