#include <algorithm>

#include "../ServerNetwork/ServerLog.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
#include "User.h"
#include "Room.h"

using PACKET_ID = Common::PACKET_ID;


namespace ServerLogic
{

	Room::Room()
	{
	}


	Room::~Room()
	{
	}

	ERROR_CODE Room::SendUserList(const int sessionID)
	{
		Common::PktRoomUserInfoRes pktRes;

		pktRes.RoomIndex = m_Index;
		pktRes.UserCount = m_UserList.size();

		for (int i = 0; i < pktRes.UserCount; ++i)
		{
			auto& user = m_UserList[i];
			strncpy_s(pktRes.UserInfo[i].UserID, Common::MAX_USER_ID_SIZE + 1, user->GetID().c_str(), Common::MAX_USER_ID_SIZE);
		}

		m_pRefNetwork->SendData(sessionID, (short)PACKET_ID::ROOM_ENTER_USER_LIST_RES, sizeof(pktRes), (char*)&pktRes);

		return ERROR_CODE::NONE;
	}


	void ServerLogic::Room::Init(const short index, short int maxUserCount)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;
	}

	void ServerLogic::Room::SetNetwork(TcpNetwork* pNetwork, ServerLog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;
	}

	void ServerLogic::Room::Clear()
	{
		m_IsUsed = false;
		m_Title = L"";
		m_UserList.clear();
	}

	ERROR_CODE ServerLogic::Room::CreateRoom(const wchar_t* pRoomTitle)
	{
		if (m_IsUsed) {
			return ERROR_CODE::ROOM_ENTER_CREATE_FAIL;
		}

		m_IsUsed = true;
		m_Title = pRoomTitle;

		return ERROR_CODE::NONE;
	}

	ERROR_CODE ServerLogic::Room::EnterUser(User* pUser)
	{
		if (m_IsUsed == false) {
			return ERROR_CODE::ROOM_ENTER_NOT_CREATED;
		}

		if (m_UserList.size() == m_MaxUserCount) {
			return ERROR_CODE::ROOM_ENTER_MEMBER_FULL;
		}

		m_UserList.push_back(pUser);
		return ERROR_CODE::NONE;
	}

	ERROR_CODE ServerLogic::Room::LeaveUser(const short userIndex)
	{
		if (m_IsUsed == false) {  //�̹��� ����ϴ��� Ȯ��
			return ERROR_CODE::ROOM_ENTER_NOT_CREATED;
		}
		//��¥ �̹��� �´��� �ѹ� �� Ȯ��
		auto iter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [userIndex](auto pUser) { return pUser->GetIndex() == userIndex; });
		if (iter == std::end(m_UserList)) {
			return ERROR_CODE::ROOM_LEAVE_NOT_MEMBER;
		}

		m_UserList.erase(iter);

		if (m_UserList.empty()) // �׹濡 �ƹ��� ������ �ʱ�ȭ
		{
			Clear();
		}

		return ERROR_CODE::NONE;
	}

	void ServerLogic::Room::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex /*= -1*/)
	{
		for (auto pUser : m_UserList)
		{
			// 			if (pUser->GetIndex() == passUserindex) {
			// 				continue;
			// 			}
			m_pRefNetwork->SendData(pUser->GetSessioIndex(), packetId, dataSize, pData);
		}
	}

	void ServerLogic::Room::NotifyEnterUserInfo(const int userIndex, const char* pszUserID)
	{
		Common::PktRoomEnterUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, Common::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::ROOM_ENTER_USER_NTF, sizeof(pkt), (char*)&pkt, userIndex);
	}

	void ServerLogic::Room::NotifyLeaveUserInfo(const char* pszUserID)
	{
		if (m_IsUsed == false) {
			return;
		}

		Common::PktRoomLeaveUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, Common::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt);
	}

	void ServerLogic::Room::NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		Common::PktRoomChatNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, Common::MAX_USER_ID_SIZE);
		wcsncpy_s(pkt.Msg, Common::MAX_ROOM_CHAT_MSG_SIZE + 1, pszMsg, Common::MAX_ROOM_CHAT_MSG_SIZE); // ä���� �ִ� ũ�⸦ ���Ƴ���

		SendToAllUser((short)PACKET_ID::ROOM_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex); // broadCast
	}

}