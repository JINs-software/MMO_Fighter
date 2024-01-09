#include "Common_FightGame.h"

namespace FightGameS2C {

	RpcID gRpcList[] = {
		RPC_CRT_CHARACTER,
		RPC_CRT_OTHER_CHARACTER,
		RPC_DEL_CHARACTER,
		RPC_MOVE_START,
		RPC_MOVE_STOP,
		RPC_ATTACK1,
		RPC_ATTACK2,
		RPC_ATTACK3,
		RPC_DAMAGE,
		RPC_SYNC,
		RPC_ECHO,
	};

	int gRpcListCount = 11;

}

namespace FightGameC2S {

	RpcID gRpcList[] = {
		RPC_MOVE_START,
		RPC_MOVE_STOP,
		RPC_ATTACK1,
		RPC_ATTACK2,
		RPC_ATTACK3,
		RPC_ECHO,
	};

	int gRpcListCount = 6;

}

