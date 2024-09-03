#include "Common_FightGameMove.h"

namespace FightGameMove_S2C {
	RpcID gRpcList[] = {
		RPC_MOVE_START,
		RPC_MOVE_STOP,
	};

	int gRpcListCount = 2;
}

namespace FightGameMove_C2S {
	RpcID gRpcList[] = {
		RPC_MOVE_START,
		RPC_MOVE_STOP,
	};

	int gRpcListCount = 2;
}

