#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include "ClientSceneLobby.h"
#include <string.h>
#include <atlstr.h>
#include <stdlib.h>

//CLIENT_SCENE_TYPE ClientSceneBase::m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;

ClientSceneLobby::ClientSceneLobby()
{
}

ClientSceneLobby::~ClientSceneLobby()
{
}


void ClientSceneLobby::Update()
{

}

bool ClientSceneLobby::ProcessPacket(const short packetId, char* pData)
{
	switch (packetId)
	{
	case (short)PACKET_ID::LOBBY_ENTER_RES:
	{
		auto pktRes = (Common::PktLobbyEnterRes*)pData;

		if (pktRes->ErrorCode == (short)Common::ERROR_CODE::NONE)
		{
			Init(pktRes->MaxUserCount);
			
			m_pRefClientStateMgr->ChangeTxtBoxCurState("Lobby : "+ std::to_string(m_pRefClientStateMgr->GetLobbyNum()));

			RequestRoomList(0);

		}
		else
		{
			m_pRefClientStateMgr->SetLobbyNum(-1);

			nana::msgbox mbox((form&)*m_pForm, "Fail LOBBY_ENTER_RES", nana::msgbox::ok);
			mbox.icon(mbox.icon_warning);
			mbox << "ErrorCode: " << pktRes->ErrorCode << " EnterLobby Failed";
			mbox.show();

			std::cout << "[LOBBY_ENTER_RES] ErrorCode: " << pktRes->ErrorCode << std::endl;
		}
	}
	break;
	case (short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES:
	{
		printf("[Lobby] Room List Receive \n");
		auto pktRes = (Common::PktLobbyRoomListRes*)pData;
		
		for (int i = 0; i < pktRes->Count; ++i)
		{
			UpdateRoomInfo(&pktRes->RoomInfo[i]);
		}

		if (pktRes->IsEnd == false)
		{
			RequestRoomList(pktRes->RoomInfo[pktRes->Count - 1].RoomIndex + 1);
		}
		else
		{
			SetRoomListGui();
			RequestLobbyUserList(0);
		}
	}
	break;


	case (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES:
	{
		printf("[Lobby] User List Receive \n");


		auto pktRes = (Common::PktLobbyUserListRes*)pData;

		for (int i = 0; i < pktRes->Count; ++i)
		{
			UpdateUserInfo(false, pktRes->UserInfo[i].UserID);
		}

		if (pktRes->IsEnd == false)
		{
			RequestLobbyUserList(pktRes->UserInfo[pktRes->Count - 1].LobbyUserIndex + 1);
		}
		else
		{
			SetLobbyUserListGui();
		}
	}
	break;
	case (short)PACKET_ID::ROOM_CHANGED_INFO_NTF:
	{
		auto pktRes = (Common::PktChangedRoomInfoNtf*)pData;
		UpdateRoomInfo(&pktRes->RoomInfo);
	}
	break;
	case (short)PACKET_ID::LOBBY_ENTER_USER_NTF:
	{
		auto pktRes = (Common::PktLobbyNewUserInfoNtf*)pData;
		printf("LOBBY USER SETTING\n");
		UpdateUserInfo(false, pktRes->UserID);
	}
	break;
	case (short)PACKET_ID::LOBBY_LEAVE_USER_NTF:
	{
		auto pktRes = (Common::PktLobbyLeaveUserInfoNtf*)pData;
		UpdateUserInfo(true, pktRes->UserID);
	}
	break;

	case (short)PACKET_ID::WHISPER_NTF:
	{
		Common::PktWhisperNtf* pktData = (Common::PktWhisperNtf*)pData;
		char id[Common::MAX_USER_ID_SIZE + 1] = { '\0', };
		memcpy(id, pktData->UserID, Common::MAX_USER_ID_SIZE);

		char msg[Common::MAX_WHISPER_MSG_SIZE + 1] = { '\0', };
		memcpy(msg, pktData->Msg, sizeof(msg));

		m_WhisperWindow->append("(Whisper From) ", true);
		m_WhisperWindow->append(id, true);
		m_WhisperWindow->append(": ", true);
		m_WhisperWindow->append(msg, true);
		m_WhisperWindow->append("\n", true);
	}
	break;

	default:
		return false;
	}

	return true;
}

void ClientSceneLobby::CreateUI(form* pform)
{
	m_pForm = pform;

	m_LobbyRoomList = std::make_shared<listbox>((form&)*m_pForm, nana::rectangle(204, 106, 300, 280));
	m_LobbyRoomList->append_header(L"RoomId", 50);
	m_LobbyRoomList->append_header(L"Title", 165);
	m_LobbyRoomList->append_header(L"Cur", 30);
	m_LobbyRoomList->append_header(L"Max", 30);

	m_LobbyIdTitleLb = std::make_shared<label>((form&)*m_pForm, nana::rectangle(22, 450, 110, 50));
	m_LobbyIdTitleLb->caption("<Lobby User List>");

	m_LobbyUserList = std::make_shared<listbox>((form&)*m_pForm, nana::rectangle(22, 470, 135, 200));
	m_LobbyUserList->append_header("UserID", 130);

	m_RoomNametxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(204, 390, 160, 23));
	m_RoomNametxt->caption("RoomName");
	
	m_CreateRoombtn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(374, 390, 102, 23));
	m_CreateRoombtn->caption("Create Room");
	m_CreateRoombtn->events().click([&]() { this->RequestCreateRoom(); });


	m_JoinRoombtn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(374, 417, 102, 23));
	m_JoinRoombtn->caption("Enter Room");
	m_JoinRoombtn->events().click([&]() {this->RequestEnterRoom(); });

	m_WhisperSendbtn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(525, 650, 50, 23));
	m_WhisperSendbtn->caption("Whisper");
	m_WhisperSendbtn->events().click([&]()
	{
		std::string msg;
		std::string tmp;
	
		int i = 0;
		while (m_WhisperMsgtxt->getline(i, tmp))
		{
			msg += tmp;
			i++;
		}
		this->RequestWhisperMsg(msg);
		m_WhisperMsgtxt->reset();
	});

	
	m_WhisperWindow = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(170, 470, 340, 170));
	
	m_WhisperMsgtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(170, 650, 340, 23));
	//m_WhisperMsgtxt 처리 #TODO

}

void ClientSceneLobby::Init(const int maxUserCount)
{
	m_MaxUserCount = maxUserCount;

	m_IsRoomListWorking = true;
	m_IsUserListWorking = true;

	m_RoomList.clear();
	m_UserList.clear();
}

void ClientSceneLobby::RequestRoomList(const short startIndex)
{
	Common::PktLobbyRoomListReq reqPkt;
	reqPkt.StartRoomIndex = startIndex;
	m_pRefNetwork->SendPacket((short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_REQ, sizeof(reqPkt), (char*)&reqPkt);
	printf("[lobby] RoomList req! \n");
}

void ClientSceneLobby::RequestLobbyUserList(const short startIndex)
{
	Common::PktLobbyUserListReq reqPkt;
	reqPkt.StartUserIndex = startIndex;
	m_pRefNetwork->SendPacket((short)PACKET_ID::LOBBY_ENTER_USER_LIST_REQ, sizeof(reqPkt), (char*)&reqPkt);
	printf("[lobby] UserList req! \n");
}

void ClientSceneLobby::SetRoomListGui()
{
	m_IsRoomListWorking = false;


	for (auto & room : m_RoomList)
	{
		m_LobbyRoomList->at(0).append({ std::to_wstring(room.RoomIndex),
			room.RoomTitle,
			std::to_wstring(room.RoomUserCount),
			std::to_wstring(m_MaxUserCount) });
	}

	m_RoomList.clear();
}

void ClientSceneLobby::SetLobbyUserListGui()
{
	m_IsUserListWorking = false;

	for (auto & userId : m_UserList)
	{
		m_LobbyUserList->at(0).append({ userId });
	}

	m_UserList.clear();
}

void ClientSceneLobby::UpdateRoomInfo(Common::RoomSmallInfo* pRoomInfo)
{
	Common::RoomSmallInfo newRoom;
	memcpy(&newRoom, pRoomInfo, sizeof(Common::RoomSmallInfo));

	bool IsRemove = newRoom.RoomUserCount == 0 ? true : false;

	if (m_IsRoomListWorking)
	{
		if (IsRemove == false)
		{
			auto findIter = std::find_if(std::begin(m_RoomList), std::end(m_RoomList), [&newRoom](auto& room) { return room.RoomIndex == newRoom.RoomIndex; });

			if (findIter != std::end(m_RoomList))
			{
				wcsncpy_s(findIter->RoomTitle, Common::MAX_ROOM_TITLE_SIZE + 1, newRoom.RoomTitle, Common::MAX_ROOM_TITLE_SIZE);
				findIter->RoomUserCount = newRoom.RoomUserCount;
			}
			else
			{
				m_RoomList.push_back(newRoom);
			}
		}
		else
		{
			m_RoomList.remove_if([&newRoom](auto& room) { return room.RoomIndex == newRoom.RoomIndex; });
		}
	}
	else // 방정보 초기화 하는경우
	{
		std::string roomIndex(std::to_string(newRoom.RoomIndex));

		if (IsRemove == false)
		{
			for (auto& room : m_LobbyRoomList->at(0))
			{
				if (room.text(0) == roomIndex)
				{
					room.text(1, newRoom.RoomTitle);
					room.text(2, std::to_wstring(newRoom.RoomUserCount));
					return;
				}
			}

			m_LobbyRoomList->at(0).append({ std::to_wstring(newRoom.RoomIndex),
				newRoom.RoomTitle,
				std::to_wstring(newRoom.RoomUserCount),
				std::to_wstring(m_MaxUserCount) });
		}
		else
		{
			for (auto& room : m_LobbyRoomList->at(0))
			{
				if (room.text(0) == roomIndex)
				{
					m_LobbyRoomList->erase(room);
					return;
				}
			}
		}
	}
}

void ClientSceneLobby::UpdateUserInfo(bool IsRemove, std::string userID)
{
	if (m_IsUserListWorking)
	{
		if (IsRemove == false)
		{
			auto findIter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [&userID](auto& ID) { return ID == userID; });

			if (findIter == std::end(m_UserList))
			{
				m_UserList.push_back(userID);
			}
		}
		else
		{
			m_UserList.remove_if([&userID](auto& ID) { return ID == userID; });
		}
	}
	else
	{
		if (IsRemove == false)
		{
			for (auto& user : m_LobbyUserList->at(0))
			{
				if (user.text(0) == userID) {
					return;
				}
			}

			m_LobbyUserList->at(0).append(userID);
		}
		else
		{
			auto i = 0;
			for (auto& user : m_LobbyUserList->at(0))
			{
				if (user.text(0) == userID)
				{
					m_LobbyUserList->erase(user);
					return;
				}
			}
		}
	}
}

void ClientSceneLobby::RequestCreateRoom()
{
	char szRoomName[Common::MAX_ROOM_TITLE_SIZE] = { 0, };
	UnicodeToAnsi( m_RoomNametxt->caption_wstring().c_str(), Common::MAX_ROOM_TITLE_SIZE, szRoomName);

	if (strlen(szRoomName) <= 0)
	{
		nana::msgbox m((form&)*m_pForm, "Enter Room Name!", nana::msgbox::ok);
		m.icon(m.icon_warning).show();
		return;
	}

	Common::PktRoomEnterReq reqPkt;
	reqPkt.WantCreate = true;							 
	mbstowcs(reqPkt.RoomTitle, szRoomName, Common::MAX_ROOM_TITLE_SIZE);
	m_pRefNetwork->SendPacket((short)PACKET_ID::ROOM_ENTER_REQ, sizeof(reqPkt), (char*)&reqPkt);

}

void ClientSceneLobby::RequestEnterRoom()
{
	auto selectedRoom = m_LobbyRoomList->selected();
	if (selectedRoom.empty())
	{
		nana::msgbox m((form&)*m_pForm, "Select Room!", nana::msgbox::ok);
		m.icon(m.icon_warning).show();
		return;
	}

	auto index = selectedRoom[0].item;
	auto roomIndex = std::atoi(m_LobbyRoomList->at(0).at(index).text(0).c_str());

	//m_pRefClientStateMgr->SetRoomNum(roomIndex);

	Common::PktRoomEnterReq reqPkt;
	reqPkt.WantCreate = false;
	reqPkt.RoomIndex = roomIndex;
	m_pRefNetwork->SendPacket((short)PACKET_ID::ROOM_ENTER_REQ, sizeof(reqPkt), (char*)&reqPkt);
}

void ClientSceneLobby::RequestWhisperMsg(std::string& msg)
{
	Common::PktWhisperReq reqPkt;

	auto selectedID = m_LobbyUserList->selected();
	if (selectedID.empty())
	{
		nana::msgbox m((form&)*m_pForm, "Select ID!", nana::msgbox::ok);
		m.icon(m.icon_warning).show();
		return;
	}
	auto IDindex = selectedID[0].item;
	std::string IDstr = m_LobbyUserList->at(0).at(IDindex).text(0).c_str();
	
	char id[Common::MAX_USER_ID_SIZE + 1] = { '\0', };
	memcpy(id, IDstr.c_str(), Common::MAX_USER_ID_SIZE);
	memcpy(reqPkt.ReceiveID, id, Common::MAX_USER_ID_SIZE);

	int msgSize = min(msg.size(), sizeof(m_WhisperMsgBuffer));
	for (int i = 0; i < msgSize; i++) 
	{
		m_WhisperMsgBuffer[i] = msg.at(i);
	}
	m_WhisperMsgBuffer[msgSize] = '\0';

	memcpy(reqPkt.Msg, m_WhisperMsgBuffer, msgSize);
	m_pRefNetwork->SendPacket((short)PACKET_ID::WHISPER_REQ, sizeof(reqPkt), (char*)&reqPkt);
	printf("WHISPER to %s " , id);
}

