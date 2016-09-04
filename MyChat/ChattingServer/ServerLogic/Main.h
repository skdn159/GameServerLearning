#pragma once
#include <memory>

#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"

using ERROR_CODE = Common::ERROR_CODE;

namespace ServerNetwork
{
	struct ServerConfig;
	class ServerLog;
	class TcpNetwork;
}

namespace ServerLogic
{
	class UserManager;
	class LobbyManager;
	class PacketProcess;
	class DBManager;


	class Main
	{
	public:
		Main();
		~Main();

		ERROR_CODE Init();
		void Run();
		void Stop();

	private:
		ERROR_CODE LoadConfig();
		void Release();


	private:
		bool m_IsRun = false;

		std::unique_ptr<ServerNetwork::ServerLog> m_pLogger;

		std::unique_ptr<ServerNetwork::ServerConfig> m_pServerConfig;
		std::unique_ptr<ServerNetwork::TcpNetwork> m_pNetwork;
		std::unique_ptr<PacketProcess> m_pPacketProc;
		std::unique_ptr<UserManager> m_pUserMgr;
		std::unique_ptr<LobbyManager> m_pLobbyMgr;
		std::unique_ptr<DBManager> m_pDBMgr;


	};
}
