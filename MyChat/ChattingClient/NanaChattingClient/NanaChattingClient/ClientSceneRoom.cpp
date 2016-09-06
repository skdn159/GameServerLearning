#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include "ClientSceneRoom.h"
#include <stdlib.h>


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

			RequestRoomUserList(pktRes->RoomIndex);

// 			if (m_IsUserListWorking == false)
// 			{
// 				RequestRoomUserList(pktRes->RoomIndex);
// 			}

			m_RoomNum = pktRes->RoomIndex;
			m_pRefClientStateMgr->ChangeTxtBoxCurState("Room : " + std::to_string(m_RoomNum));

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

	case (short)PACKET_ID::ROOM_ENTER_USER_LIST_RES:
	{
		auto pktRes = (Common::PktRoomUserInfoRes*)pData;
		if (pktRes->UserCount == 0)
		{
			return false;
		}

		for (int i = 0; i < pktRes->UserCount; ++i)
		{
			UpdateUserInfo(false, pktRes->UserInfo[i].UserID);
		}

		SetUserListGui();
	}
	break;


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

	case (short)PACKET_ID::LOBBY_ENTER_RES:
	{
		auto pkRes = (Common::PktRoomLeaveRes*)pData;
		if (pkRes->ErrorCode == (short)Common::ERROR_CODE::NONE)
		{
			m_RoomUserList->clear();
			SetCurSceneType(CLIENT_SCENE_TYPE::LOBBY);
		}
	}
	break;

	case (short)PACKET_ID::ROOM_CHAT_RES:
	{
		Common::PktRoomChatRes* pktData = (Common::PktRoomChatRes*)pData;

		if ((short)Common::ERROR_CODE::NONE != pktData->ErrorCode)
		{
 			//alert


		}
	}
	break;

	case (short)PACKET_ID::ROOM_CHAT_NTF:
	{
		Common::PktRoomChatNtf* pktData = (Common::PktRoomChatNtf*)pData;
		char id[Common::MAX_USER_ID_SIZE + 1] = { '\0', };
		memcpy(id, pktData->UserID, Common::MAX_USER_ID_SIZE);

		char msg[Common::MAX_ROOM_CHAT_MSG_SIZE + 1] = { '\0', };
		memcpy(msg, pktData->Msg, sizeof(msg));

		m_ChattingWindow->append(id, true);
		m_ChattingWindow->append(": ", true);
		m_ChattingWindow->append(msg, true);
		m_ChattingWindow->append("\n", true);
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
	
	m_ChattingSendBttn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(850, 625, 50, 23));
	m_ChattingSendBttn->caption("Send");
	m_ChattingSendBttn->events().click([&]()
	{ 
		std::string msg;
		std::string tmp;

		int i = 0;
		while (m_Sendingtxt->getline(i,tmp))
		{
			msg += tmp;
			i++;
		}

		this->RequestRoomChattingMsg(msg);
		
		m_Sendingtxt->reset();
	});

	m_Sendingtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(655, 625, 190, 25));
	//sending txtÃ³¸®

	m_Sendingtxt->events().key_press([&](const nana::arg_keyboard& event)
	{
// 		auto key = event.key;
// 		if (key != '\r')
// 			return;
// 
// 		std::string msg;
// 		std::string tmp;
// 
// 		int i = 0;
// 		while (m_pChatInput->getline(i, tmp))
// 		{
// 			msg += tmp;
// 			i++;
// 		}
// 
// 		msg.erase(msg.find_last_not_of("\n\r\t") + 1);
// 
// 		this->ProcessMsg(msg);
// 
// 		m_pChatInput->reset();

	});

}

void ClientSceneRoom::SetUserListGui()
{
	m_IsUserListWorking = true;

	//m_RoomUserList->clear();
	for (auto & userId : m_UserIDList)
	{
		m_RoomUserList->at(0).append({ userId });
	}

	m_UserIDList.clear();
}

void ClientSceneRoom::UpdateUserInfo(bool IsRemove, std::string userID)
{
	if (m_IsUserListWorking == false)
	{
		if (IsRemove == false)
		{
			auto findIter = std::find_if(std::begin(m_UserIDList), std::end(m_UserIDList), [&userID](auto& ID) { return ID == userID; });

			if (findIter == std::end(m_UserIDList))
			{
				m_UserIDList.push_back(userID);
			}
		}
		else
		{
			m_UserIDList.remove_if([&userID](auto& ID) { return ID == userID; });
		}
	}
	else
	{
		if (IsRemove == false)
		{
			for (auto& user : m_RoomUserList->at(0))
			{
				if (user.text(0) == userID) {
					return;
				}
			}

			m_RoomUserList->at(0).append(userID);
		}
		else
		{
			auto i = 0;
			for (auto& user : m_RoomUserList->at(0))
			{
				if (user.text(0) == userID)
				{
					m_RoomUserList->erase(user);
					return;
				}
			}
		}
	}
}
	


void ClientSceneRoom::RequestRoomUserList(short roomIndex)
{
	Common::PktRoomUserInfoReq reqPkt;
	reqPkt.RoomIndex = roomIndex;
	m_pRefNetwork->SendPacket((short)PACKET_ID::ROOM_ENTER_USER_LIST_REQ, sizeof(reqPkt), (char*)&reqPkt);
}

void ClientSceneRoom::RequestRoomChattingMsg(std::string& msg)
{
	int msgSize = min(msg.size(), sizeof(m_MsgBuffer));

	for (int i = 0; i < msgSize; i++)
	{
		m_MsgBuffer[i] = msg.at(i);
	}
	m_MsgBuffer[msgSize] = '\0';

	Common::PktRoomChatReq reqPkt;
	memcpy(reqPkt.Msg, m_MsgBuffer, msgSize);
	m_pRefNetwork->SendPacket((short)PACKET_ID::ROOM_CHAT_REQ, sizeof(reqPkt), (char*)&reqPkt);
}
