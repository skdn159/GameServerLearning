#pragma once
#include <vector>
#include <unordered_map>
#include "Room.h"

namespace Common
{
	enum class ERROR_CODE :short;
}
using ERROR_CODE = Common::ERROR_CODE;

namespace ServerNetwork
{
	class TcpNetwork;
	class ServerLog;
}

namespace ServerLogic
{
	using TcpNetwork = ServerNetwork::TcpNetwork;
	using ServerLog = ServerNetwork::ServerLog;

	class User;
	//class Room;

	struct LobbyUser
	{
		short Index = 0;
		User* pUser = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		virtual ~Lobby();

		void Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount);
		void SetNetwork(TcpNetwork* pNetwork, ServerLog* pLogger);
		short GetIndex() { return m_LobbyIndex; }

		ERROR_CODE EnterUser(User* pUser);
		ERROR_CODE LeaveUser(const int userIndex);

		void NotifyLobbyEnterUserInfo(User* pUser);
		ERROR_CODE SendRoomList(const int sessionId, const short startRoomId);
		ERROR_CODE SendUserList(const int sessionId, const short startUserIndex);

		void NotifyLobbyLeaveUserInfo(User* pUser);
		Room* GetRoom(const short roomIndex);
		Room* CreateRoom();
		void NotifyChangedRoomInfo(const short roomIndex);

		short GetUserCount() { return static_cast<short>(m_UserIndexDic.size()); }
		auto GetMaxUserCount() { return (short)m_MaxUserCount; }
		auto GetMaxRoomCount() { return (short)m_RoomList.size(); }

		void NotifyChat(const int sessionIndex, const char* pszUserID, const wchar_t* pszMsg);
		void NotifyWhisper(const int receiveUserSessionIndex, const char* pszUserID, const wchar_t* pszMsg);

	protected:
		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);
		void SendToOneUser(const int targetUserindex, const short packetId, const short dataSize, char* pData);
		User* FindUser(const int userIndex);
		ERROR_CODE AddUser(User* pUser);
		void RemoveUser(const int userIndex);



	protected:
		ServerLog* m_pRefLogger;
		TcpNetwork* m_pRefNetwork;

		short m_LobbyIndex = 0;
		short m_MaxUserCount = 0;
		std::vector<LobbyUser> m_UserList;
		std::unordered_map<int, User*> m_UserIndexDic;
		std::unordered_map<const char*, User*> m_UserIDDic;

		std::vector<Room> m_RoomList;
	};
}