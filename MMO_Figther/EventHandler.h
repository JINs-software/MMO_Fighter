#pragma once
#include "JNetCoreServer.h"
#include "Contents.h"

class EventHandler : public JNetServerEventHandler {
public:
	void OnReadyToAccept() override {
		return;
	}
	bool OnConnectRequest() override {
		return true;
	}
	void OnClientJoin(HostID remote) override {
		CreateFighter(remote);
	}
	bool OnClientDisconnect(HostID remote) override {
		DeleteFighter(remote, true);
		return true;
	}
};