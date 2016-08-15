#pragma once
#include <vector>
#include <chrono>

#include "../ServerNetwork/ServerLog.h"
#include "../ServerNetwork/TcpNetwork.h"

namespace ServerLogic
{
	const int HEARTBEAT_RATE = 60; // 60mil sec


	struct ConnectedUser
	{
		void Clear()
		{
			m_IsLoginSuccess = false;
			m_ConnectedTime = 0;
		}

		bool m_IsLoginSuccess = false;
		time_t m_ConnectedTime = 0;
	};

	using ServerLog = ServerNetwork::ServerLog;
	using TcpNetwork = ServerNetwork::TcpNetwork;

	class ConnectedUserManager
	{
	public:
		ConnectedUserManager();
		virtual ~ConnectedUserManager();


		void Init(const int maxSessionCount, TcpNetwork* pNetwork, ServerLog* pLogger);
		void SetConnectSession(const int sessionIndex);
		void SetLogin(const int sessionIndex);
		void SetDisConnectSession(const int sessionIndex);
		void LoginCheck(); // heartBeat

		

	private:
		ServerLog* m_pRefLogger;
		TcpNetwork* m_pRefNetwork;

		std::vector<ConnectedUser> ConnectedUserList;

		std::chrono::system_clock::time_point m_LatestLoginCheckTime;
		int m_LatestLogincheckIndex = -1;

	};

}
