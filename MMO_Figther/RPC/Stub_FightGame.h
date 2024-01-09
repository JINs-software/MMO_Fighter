#pragma once

#include "Common_FightGame.h"

namespace FightGameS2C {

	class Stub : public JNetStub
	{
	public: 
		virtual bool CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) { return false; }
#define JPDEC_FightGameS2C_CRT_CHARACTER bool CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP)
#define JPDEF_FightGameS2C_CRT_CHARACTER(DerivedClass) bool DerivedClass::CRT_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP)
		virtual bool CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP) { return false; }
#define JPDEC_FightGameS2C_CRT_OTHER_CHARACTER bool CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP)
#define JPDEF_FightGameS2C_CRT_OTHER_CHARACTER(DerivedClass) bool DerivedClass::CRT_OTHER_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y, BYTE HP)
		virtual bool DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID) { return false; }
#define JPDEC_FightGameS2C_DEL_CHARACTER bool DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID)
#define JPDEF_FightGameS2C_DEL_CHARACTER(DerivedClass) bool DerivedClass::DEL_CHARACTER(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID)
		virtual bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_MOVE_START bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_MOVE_START(DerivedClass) bool DerivedClass::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_MOVE_STOP bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_MOVE_STOP(DerivedClass) bool DerivedClass::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_ATTACK1 bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_ATTACK1(DerivedClass) bool DerivedClass::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_ATTACK2 bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_ATTACK2(DerivedClass) bool DerivedClass::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_ATTACK3 bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_ATTACK3(DerivedClass) bool DerivedClass::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP) { return false; }
#define JPDEC_FightGameS2C_DAMAGE bool DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP)
#define JPDEF_FightGameS2C_DAMAGE(DerivedClass) bool DerivedClass::DAMAGE(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t attker, uint32_t target, BYTE targetHP)
		virtual bool SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameS2C_SYNC bool SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y)
#define JPDEF_FightGameS2C_SYNC(DerivedClass) bool DerivedClass::SYNC(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t ID, uint16_t X, uint16_t Y)
		virtual bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) { return false; }
#define JPDEC_FightGameS2C_ECHO bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time)
#define JPDEF_FightGameS2C_ECHO(DerivedClass) bool DerivedClass::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }

		bool ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};

}
namespace FightGameC2S {

	class Stub : public JNetStub
	{
	public: 
		virtual bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameC2S_MOVE_START bool MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameC2S_MOVE_START(DerivedClass) bool DerivedClass::MOVE_START(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameC2S_MOVE_STOP bool MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameC2S_MOVE_STOP(DerivedClass) bool DerivedClass::MOVE_STOP(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameC2S_ATTACK1 bool ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameC2S_ATTACK1(DerivedClass) bool DerivedClass::ATTACK1(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameC2S_ATTACK2 bool ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameC2S_ATTACK2(DerivedClass) bool DerivedClass::ATTACK2(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y) { return false; }
#define JPDEC_FightGameC2S_ATTACK3 bool ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
#define JPDEF_FightGameC2S_ATTACK3(DerivedClass) bool DerivedClass::ATTACK3(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, BYTE Direction, uint16_t X, uint16_t Y)
		virtual bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time) { return false; }
#define JPDEC_FightGameC2S_ECHO bool ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time)
#define JPDEF_FightGameC2S_ECHO(DerivedClass) bool DerivedClass::ECHO(HostID remote, BYTE byCode, BYTE bySize, BYTE byType, uint32_t Time)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }

		bool ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};

}
