#include <iostream>

#include "ClientSceneRoom.h"

//CLIENT_SCENE_TYPE ClientSceneBase::m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;

ClientSceneRoom::ClientSceneRoom()
{
}


ClientSceneRoom::~ClientSceneRoom()
{
}

void ClientSceneRoom::Update()
{

}

bool ClientSceneRoom::ProcessPacket(const short packetId, char* pData)
{
	switch (packetId)
	{
	case (short)PACKET_ID::ROOM_ENTER_RES:
	{
		
		auto pktRes = (Common::PktRoomEnterRes*)pData; 

		if (pktRes->ErrorCode == (short)Common::ERROR_CODE::NONE)
		{
			m_RoomNum = pktRes->RoomIndex;
			m_pRefClientStateMgr->ChangeTxtBoxCurState("Room : " + m_RoomNum);

		}

		else
		{
			//m_pRefClientStateMgr->SetRoomNum(-1);

			nana::msgbox mbox((form&)*m_pForm, "Fail LOBBY_ENTER_RES", nana::msgbox::ok);
			mbox.icon(mbox.icon_warning);
			mbox << "ErrorCode: " << pktRes->ErrorCode << " EnterRoom Failed";
			mbox.show();

			std::cout << "[ROOM_ENTER_RES] ErrorCode: " << pktRes->ErrorCode << std::endl;
		}

		SetCurSceneType(CLIENT_SCENE_TYPE::ROOM);
	}
	break;

// 	case (short)PACKET_ID::ROOM_ENTER_USER_LIST_RES:
// 	{
// 		auto pktRes = (Common::PktEnterRoomUserInfoRes*)pData;
// 		if (pktRes->UserCount == 0)
// 		{
// 			return false;
// 		}
// 
// 		for (int i = 0; i < pktRes->UserCount; ++i)
// 		{
// 			UpdateUserInfo(false, pktRes->UserInfo[i].UserID);
// 		}
// 
// 		SetUserListGui();
// 	}
// 	break;


	case (short)PACKET_ID::ROOM_ENTER_USER_NTF:
	{
		auto pktRes = (Common::PktRoomEnterUserInfoNtf*)pData;
		UpdateUserInfo(false, pktRes->UserID);
	}
	break;

	case (short)PACKET_ID::ROOM_LEAVE_USER_NTF:
	{
		auto pktRes = (Common::PktRoomEnterUserInfoNtf*)pData;
		UpdateUserInfo(true, pktRes->UserID);
	}
	break;

	default:
		return false;
	}

	return true;
}

void ClientSceneRoom::CreateUI(form* pform)
{
	m_pForm = pform;

	m_RoomUserList = std::make_shared<listbox>((form&)*m_pForm, nana::rectangle(510, 106, 140, 250));
	m_RoomUserList->append_header("UserID", 130);

	m_ChattingWindow = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(655, 20, 240, 600));
	m_Sendingtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(655, 625, 190, 25));
	m_ChattingSendBttn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(850, 625, 50, 23));
	m_ChattingSendBttn->caption("Send");
	m_ChattingSendBttn->events().click([&]() { this->RequestRoomChattingMsg(); });



}

void ClientSceneRoom::UpdateUserInfo(bool IsRemove, std::string userID)
{
	printf("ADD me TO ROOMLIST!");
	// RoomList에 추가한다.!!
	
	







// 	if (m_IsUserListWorking)
// 	{
// 		if (IsRemove == false)
// 		{
// 			auto findIter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [&userID](auto& ID) { return ID == userID; });
// 
// 			if (findIter == std::end(m_UserList))
// 			{
// 				m_UserList.push_back(userID);
// 			}
// 		}
// 		else
// 		{
// 			m_UserList.remove_if([&userID](auto& ID) { return ID == userID; });
// 		}
// 	}
// 	else
// 	{
// 		if (IsRemove == false)
// 		{
// 			for (auto& user : m_LobbyUserList->at(0))
// 			{
// 				if (user.text(0) == userID) {
// 					return;
// 				}
// 			}
// 
// 			m_LobbyUserList->at(0).append(userID);
// 		}
// 		else
// 		{
// 			auto i = 0;
// 			for (auto& user : m_LobbyUserList->at(0))
// 			{
// 				if (user.text(0) == userID)
// 				{
// 					m_LobbyUserList->erase(user);
// 					return;
// 				}
// 			}
// 		}
// 	}


}

void ClientSceneRoom::RequestRoomUserList()
{

}

void ClientSceneRoom::RequestRoomChattingMsg()
{

}
