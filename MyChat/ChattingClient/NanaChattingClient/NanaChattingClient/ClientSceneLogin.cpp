
#include <iostream>
#include "ClientSceneLogin.h"

//CLIENT_SCENE_TYPE ClientSceneBase::m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;

ClientSceneLogin::ClientSceneLogin()
{

}

ClientSceneLogin::~ClientSceneLogin()
{
}


void ClientSceneLogin::Update()
{
	if (GetCurSceneType() != CLIENT_SCENE_TYPE::LOGIN) {
		return;
	}

	if (CheckIsLobbySelected()==false) 
	{
		auto curTime = std::chrono::system_clock::now();
		auto diffTimeSec = std::chrono::duration_cast<std::chrono::seconds>(curTime - m_TimeLastedReqLobbyList);

		if (diffTimeSec.count() > 5) // 5초마다 Lobby요청 갱신 해야하나??
		{
			m_TimeLastedReqLobbyList = curTime;

			RequestLobbyList();
		}
	}
}

bool ClientSceneLogin::ProcessPacket(const short packetId, char* pData)
{
	switch (packetId)
	{
	case (short)PACKET_ID::LOBBY_LIST_RES:
	{
		auto pktRes = (Common::PktLobbyListRes*)pData;

		if (pktRes->ErrorCode == (short)Common::ERROR_CODE::NONE)
		{
			m_LobbyList->clear();

			for (int i = 0; i < pktRes->LobbyCount; ++i)
			{
				auto& pLobby = pktRes->LobbyList[i];

				m_LobbyList->at(0).append({ std::to_string(pLobby.LobbyId),
					std::to_string(pLobby.LobbyUserCount),
					std::to_string(100) }); // #TODO : friend Class를 쓰던 mgr를 쓰던 받아와야함
			}
		}
		else
		{
			std::cout << "[LOBBY_LIST_RES] ErrorCode: " << pktRes->ErrorCode << std::endl;
		}
	}
	break;
	default:
		return false;
	}

	return true;
}

void ClientSceneLogin::CreateUI(form* pform)
{
	m_pForm = pform;

	m_LobbyList = std::make_unique<listbox>((form&)*m_pForm, nana::rectangle(22, 106, 180, 280));
	m_LobbyList->append_header("LobbyID", 50);
	m_LobbyList->append_header("CurPlayer", 60);
	m_LobbyList->append_header("MaxPlayer", 65);

	m_btnEnterLobby = std::make_unique<button>((form&)*m_pForm, nana::rectangle(22, 390, 88, 23));
	m_btnEnterLobby->caption("Enter Lobby");
	m_btnEnterLobby->events().click([&]() {
		this->RequestEnterLobby();
	});
	
	m_btnLeaveLobby = std::make_unique<button>((form&)*m_pForm, nana::rectangle(110, 390, 88, 23));
	m_btnLeaveLobby->caption("Leave Lobby");
	m_btnLeaveLobby->events().click([&]() {
		this->RequestLeaveLobby();
	});
	m_btnLeaveLobby->enabled(false);


}

void ClientSceneLogin::RequestLobbyList()
{
	m_pRefNetwork->SendPacket((short)PACKET_ID::LOBBY_LIST_REQ, 0, nullptr);

}

bool ClientSceneLogin::CheckIsLobbySelected()
{
	auto selItem = m_LobbyList->selected();
	if (selItem.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}



void ClientSceneLogin::RequestEnterLobby()
{
	if (GetCurSceneType() != CLIENT_SCENE_TYPE::LOGIN) //예외처리
	{
		nana::msgbox m((form&)*m_pForm, "Require LogIn", nana::msgbox::ok);
		m << "Login Needed";
		m.icon(m.icon_warning).show();
		return;
	}

	auto selItem = m_LobbyList->selected();
	if (selItem.empty())
	{
		nana::msgbox m((form&)*m_pForm, "Fail Don't Select Lobby", nana::msgbox::ok);
		m << "No Lobby Selected";
		m.icon(m.icon_warning).show();
		return;
	}

	auto index = selItem[0].item;
	auto lobbyId = std::atoi(m_LobbyList->at(0).at(index).text(0).c_str());
	Common::PktLobbyEnterReq reqPkt;
	reqPkt.LobbyId = 0;
	m_pRefNetwork->SendPacket((short)PACKET_ID::LOBBY_ENTER_REQ, sizeof(reqPkt), (char*)&reqPkt);
	m_pRefClientStateMgr->SetLobbyNum(lobbyId);
}

void ClientSceneLogin::RequestLeaveLobby()
{
	Common::PktLobbyLeaveReq reqPkt;
	m_pRefNetwork->SendPacket((short)PACKET_ID::LOBBY_LEAVE_REQ, sizeof(reqPkt), (char*)&reqPkt);
	
}

