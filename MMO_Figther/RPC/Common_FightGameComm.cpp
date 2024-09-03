#include "Common_FightGameComm.h"

namespace FightGameComm_S2C {
	RpcID gRpcList[] = {
		RPC_SYNC,
		RPC_ECHO,
	};

	int gRpcListCount = 2;
}

namespace FightGameComm_C2S {
	RpcID gRpcList[] = {
		RPC_ECHO,
	};

	int gRpcListCount = 1;
}

