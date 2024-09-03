#include "Common_FightGameCrtDel.h"

namespace FightGameCrtDel_S2C {
	RpcID gRpcList[] = {
		RPC_CRT_CHARACTER,
		RPC_CRT_OTHER_CHARACTER,
		RPC_DEL_CHARACTER,
	};

	int gRpcListCount = 3;
}

namespace FightGameCrtDel_C2S {
	int gRpcListCount = 0;
}

