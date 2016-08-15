#include <algorithm>

#include "../ServerNetwork/ServerLog.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
#include "Lobby.h"
#include "Room.h"

using PACKET_ID = Common::PACKET_ID;

namespace ServerLogic
{
	Lobby::Lobby()
	{
	}

	Lobby::~Lobby()
	{
	}

	Room* ServerLogic::Lobby::CreateRoom()
	{
// 		auto findIter = std::find_if(std::begin(m_RoomList), std::end(m_RoomList), [](auto& room){ return room.IsUsed() == false; });
// 
// 		if (findIter == std::end(m_RoomList)) {
// 			return nullptr;
// 		}
// 		
// 		return (Room*)&findIter;

		//# Todo ÀÌ°Å find_if STL·Î ¹Ù²Ü °Í
		for (int i = 0; i < m_RoomList.size(); ++i)
		{
			if (m_RoomList[i].IsUsed() == false) {
				
				//roomÀÇ index?
				return &m_RoomList[i];
			}

		}
		return nullptr;   
	}



	void ServerLogic::Lobby::Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		m_LobbyIndex = lobbyIndex;
		m_MaxUserCount = (short)maxLobbyUserCount;

		for (int i = 0; i < maxLobbyUserCount; ++i)
		{
			LobbyUser lobbyUser;
			lobbyUser.Index = (short)i;
			lobbyUser.pUser = nullptr;

			m_UserList.push_back(lobbyUser);
		}

		for (int i = 0; i < maxRoomCountByLobby; ++i)
		{
			m_RoomList.emplace_back(Room());
			m_RoomList[i].Init((short)i, maxRoomUserCount);
		}
	}

	void ServerLogic::Lobby::SetNetwork(TcpNetwork* pNetwork, ServerLog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;

		for (auto& room : m_RoomList)
		{
			room.SetNetwork(pNetwork, pLogger);
		}
	}

	ERROR_CODE ServerLogic::Lobby::EnterUser(User* pUser)
	{
		if (m_UserIndexDic.size() >= m_MaxUserCount) {
			return ERROR_CODE::LOBBY_ENTER_MAX_USER_COUNT;
		}

		if (FindUser(pUser->GetIndex()) != nullptr) {
			return ERROR_CODE::LOBBY_ENTER_USER_DUPLICATION;
		}

		auto addRet = AddUser(pUser);
		if (addRet != ERROR_CODE::NONE) {
			return addRet;
		}

		pUser->EnterLobby(m_LobbyIndex);

		m_UserIndexDic.insert({ pUser->GetIndex(), pUser });
		m_UserIDDic.insert({ pUser->GetID().c_str(), pUser });

		return ERROR_CODE::NONE;
	}

	ERROR_CODE ServerLogic::Lobby::LeaveUser(const int userIndex)
	{
		RemoveUser(userIndex);

		auto pUser = FindUser(userIndex);

		if (pUser == nullptr) {
			return ERROR_CODE::LOBBY_LEAVE_USER_NVALID_UNIQUEINDEX;
		}

		pUser->LeaveLobby();

		m_UserIndexDic.erase(pUser->GetIndex());
		m_UserIDDic.erase(pUser->GetID().c_str());

		return ERROR_CODE::NONE;
	}

	User* ServerLogic::Lobby::FindUser(const int userIndex)
	{
		auto findIter = m_UserIndexDic.find(userIndex);

		if (findIter == m_UserIndexDic.end()) {
			return nullptr;
		}

		return (User*)findIter->second;
	}

	ERROR_CODE ServerLogic::Lobby::AddUser(User* pUser)
	{
		auto findIter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [](auto& lobbyUser) { return lobbyUser.pUser == nullptr; });

		if (findIter == std::end(m_UserList)) {
			return ERROR_CODE::LOBBY_ENTER_EMPTY_USER_LIST;
		}

		findIter->pUser = pUser;
		return ERROR_CODE::NONE;
	}

	void ServerLogic::Lobby::RemoveUser(const int userIndex)
	{
		auto findIter = std::find_if(std::begin(m_UserList), std::end(m_UserList), [userIndex](auto& lobbyUser) { return lobbyUser.pUser != nullptr && lobbyUser.pUser->GetIndex() == userIndex; });

		if (findIter != std::end(m_UserList)) {
			return;
		}

		findIter->pUser = nullptr;
	}


	void ServerLogic::Lobby::NotifyLobbyEnterUserInfo(User* pUser)
	{
		Common::PktLobbyNewUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pUser->GetID().c_str(), Common::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::LOBBY_ENTER_USER_NTF, sizeof(pkt), (char*)&pkt, pUser->GetIndex());
	}

	void ServerLogic::Lobby::NotifyLobbyLeaveUserInfo(User* pUser)
	{
		Common::PktLobbyLeaveUserInfoNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pUser->GetID().c_str(), Common::MAX_USER_ID_SIZE);

		SendToAllUser((short)PACKET_ID::LOBBY_LEAVE_USER_NTF, sizeof(pkt), (char*)&pkt, pUser->GetIndex());
	}

	void ServerLogic::Lobby::NotifyChangedRoomInfo(const short roomIndex)
	{
		Common::PktChangedRoomInfoNtf pktNtf;

		auto& room = m_RoomList[roomIndex];

		pktNtf.RoomInfo.RoomIndex = room.GetIndex();
		pktNtf.RoomInfo.RoomUserCount = room.GetUserCount();

		if (m_RoomList[roomIndex].IsUsed()) {
			wcsncpy_s(pktNtf.RoomInfo.RoomTitle, Common::MAX_ROOM_TITLE_SIZE + 1, room.GetTitle(), Common::MAX_ROOM_TITLE_SIZE);
		}
		else {
			pktNtf.RoomInfo.RoomTitle[0] = L'\0';
		}

		SendToAllUser((short)PACKET_ID::ROOM_CHANGED_INFO_NTF, sizeof(pktNtf), (char*)&pktNtf);
	}

	void Lobby::NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		Common::PktRoomChatNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, Common::MAX_USER_ID_SIZE);
		wcsncpy_s(pkt.Msg, Common::MAX_LOBBY_CHAT_MSG_SIZE + 1, pszMsg, Common::MAX_LOBBY_CHAT_MSG_SIZE);

		SendToAllUser((short)PACKET_ID::LOBBY_CHAT_NTF, sizeof(pkt), (char*)&pkt, sessionIndex); // ³ª ÀÚ½ÅÇÑÅÙ ¾Èº¸³¿
	}

	void Lobby::NotifyWhisper(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg)
	{
		Common::PktRoomChatNtf pkt;
		strncpy_s(pkt.UserID, _countof(pkt.UserID), pszUserID, Common::MAX_USER_ID_SIZE);
		wcsncpy_s(pkt.Msg, Common::MAX_WHISPER_MSG_SIZE + 1, pszMsg, Common::MAX_WHISPER_MSG_SIZE);

		SendToAllUser((short)PACKET_ID::WHISPER_NTF, sizeof(pkt), (char*)&pkt, sessionIndex); // ³ª ÀÚ½ÅÇÑÅÙ ¾Èº¸³¿
	}


	ERROR_CODE ServerLogic::Lobby::SendRoomList(const int sessionId, const short startRoomId)
	{
		if (startRoomId < 0 || startRoomId >= (m_RoomList.size() - 1)) {
			return ERROR_CODE::LOBBY_ROOM_LIST_INVALID_START_ROOM_INDEX;
		}

		Common::PktLobbyRoomListRes pktRes;
		short roomCount = 0;
		int lastCheckedIndex = 0;

		for (int i = startRoomId; i < m_RoomList.size(); ++i)
		{
			auto& room = m_RoomList[i];
			lastCheckedIndex = i;

			if (room.IsUsed() == false) {
				continue;
			}

			pktRes.RoomInfo[roomCount].RoomIndex = room.GetIndex();
			pktRes.RoomInfo[roomCount].RoomUserCount = room.GetUserCount();
			wcsncpy_s(pktRes.RoomInfo[roomCount].RoomTitle, Common::MAX_ROOM_TITLE_SIZE + 1, room.GetTitle(), Common::MAX_ROOM_TITLE_SIZE);

			++roomCount;

			if (roomCount >= Common::MAX_NTF_LOBBY_ROOM_LIST_COUNT) {
				break;
			}
		}

		pktRes.Count = roomCount;

		if (roomCount <= 0 || (lastCheckedIndex + 1) == m_RoomList.size()) {
			pktRes.IsEnd = true;
		}

		m_pRefNetwork->SendData(sessionId, (short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES, sizeof(pktRes), (char*)&pktRes);

		return ERROR_CODE::NONE;
	}

	ERROR_CODE ServerLogic::Lobby::SendUserList(const int sessionId, const short startUserIndex)
	{
		if (startUserIndex < 0 || startUserIndex >= (m_UserList.size() - 1)) {
			return ERROR_CODE::LOBBY_USER_LIST_INVALID_START_USER_INDEX;
		}

		int lastCheckedIndex = 0;
		Common::PktLobbyUserListRes pktRes;
		short userCount = 0;

		for (int i = startUserIndex; i < m_UserList.size(); ++i)
		{
			auto& lobbyUser = m_UserList[i];
			lastCheckedIndex = i;

			if (lobbyUser.pUser == nullptr || lobbyUser.pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
				continue;
			}

			pktRes.UserInfo[userCount].LobbyUserIndex = (short)i;
			strncpy_s(pktRes.UserInfo[userCount].UserID, Common::MAX_USER_ID_SIZE + 1, lobbyUser.pUser->GetID().c_str(), Common::MAX_USER_ID_SIZE);

			++userCount;

			if (userCount >= Common::MAX_SEND_LOBBY_USER_LIST_COUNT) {
				break;
			}
		}

		pktRes.Count = userCount;

		if (userCount <= 0 || (lastCheckedIndex + 1) == m_UserList.size()) {
			pktRes.IsEnd = true;
		}

		m_pRefNetwork->SendData(sessionId, (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, sizeof(pktRes), (char*)&pktRes);

		return ERROR_CODE::NONE;
	}

	Room* ServerLogic::Lobby::GetRoom(const short roomIndex)
	{
		if (roomIndex < 0 || roomIndex >= m_RoomList.size()) {
			return nullptr;
		}

		return &m_RoomList[roomIndex];
	}

	void ServerLogic::Lobby::SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex /*= -1*/)
	{
		for (auto& pUser : m_UserIndexDic)
		{
// 			if (pUser.second->GetIndex() == passUserindex) {
// 				continue;
// 			}

			if (pUser.second->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
				continue;
			}

			m_pRefNetwork->SendData(pUser.second->GetSessioIndex(), packetId, dataSize, pData);
		}
	}



}