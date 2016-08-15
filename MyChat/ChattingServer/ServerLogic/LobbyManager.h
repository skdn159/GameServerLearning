#pragma once
#include <vector>
#include <unordered_map>

namespace ServerNetwork 
{
	class TcpNetwork;
	class ServerLog;
}

namespace ServerLogic
{
	struct LobbyManagerConfig
	{
		int maxLobbyCount;
		int maxLobbyUserCount;
		int maxRoomCountByLobby;
		int maxRoomUserCount;
	};

	struct LobbySmallInfo
	{
		short num;
		short userCount;
	};

	class Lobby;

	class LobbyManager
	{
		using TcpNetwork = ServerNetwork::TcpNetwork;
		using ServerLog = ServerNetwork::ServerLog;

	public:
		LobbyManager();
		virtual ~LobbyManager();

		void Init(const LobbyManagerConfig config, TcpNetwork* pNetwork, ServerLog* pLogger);
		Lobby* GetLobby(short lobbyId);
		void SendLobbyListInfo(const int sessionIndex);

	private:
		ServerLog* m_pRefLogger;
		TcpNetwork* m_pRefNetwork;

		std::vector<Lobby> m_LobbyList;
	};

}