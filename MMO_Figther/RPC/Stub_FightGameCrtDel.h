#pragma once
#include "Common_FightGameCrtDel.h"

namespace FightGameCrtDel_S2C {

	class Stub : public JNetStub
	{
	public:
		virtual bool CRT_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP) { return false; }
#define JPDEC_FightGameCrtDel_S2C_CRT_CHARACTER bool CRT_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP)
#define JPDEF_FightGameCrtDel_S2C_CRT_CHARACTER(DerivedClass) bool DerivedClass::CRT_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP)
		virtual bool CRT_OTHER_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP) { return false; }
#define JPDEC_FightGameCrtDel_S2C_CRT_OTHER_CHARACTER bool CRT_OTHER_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP)
#define JPDEF_FightGameCrtDel_S2C_CRT_OTHER_CHARACTER(DerivedClass) bool DerivedClass::CRT_OTHER_CHARACTER(HostID remote, uint32_t ID, BYTE Dir, uint16_t X, uint16_t Y, BYTE HP)
		virtual bool DEL_CHARACTER(HostID remote, uint32_t ID) { return false; }
#define JPDEC_FightGameCrtDel_S2C_DEL_CHARACTER bool DEL_CHARACTER(HostID remote, uint32_t ID)
#define JPDEF_FightGameCrtDel_S2C_DEL_CHARACTER(DerivedClass) bool DerivedClass::DEL_CHARACTER(HostID remote, uint32_t ID)

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

namespace FightGameCrtDel_C2S {

	class Stub : public JNetStub
	{
	public:

		RpcID* GetRpcList() override { return gRpcList; }
		int GetRpcListCount() override { return gRpcListCount; }
		void ProcessReceivedMessage(HostID remote, JBuffer& jbuff) override;
	};
}

