#pragma once
#include <chrono>
#include "ClientSceneBase.h"

class ClientSceneLogin : public ClientSceneBase
{
public:
	ClientSceneLogin();
	virtual~ClientSceneLogin();

	virtual void Update() override;
	bool ProcessPacket(const short packetId, char* pData) override;

	void CreateUI(form* pform);

		
private:
	void RequestLobbyList();
	void RequestEnterLobby();
	void RequestLeaveLobby();
	bool CheckIsLobbySelected();

private:
	form* m_pForm = nullptr;

	std::unique_ptr<button> m_btnEnterLobby;
	std::unique_ptr<button> m_btnLeaveLobby;
	std::unique_ptr<listbox> m_LobbyList;

	std::chrono::system_clock::time_point m_TimeLastedReqLobbyList = std::chrono::system_clock::now();
};
