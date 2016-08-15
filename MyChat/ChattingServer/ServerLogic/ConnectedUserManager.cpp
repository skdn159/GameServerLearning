#pragma once
#include <time.h>

#include "ConnectedUserManager.h"

namespace ServerLogic
{

	ConnectedUserManager::ConnectedUserManager()
	{
		m_LatestLoginCheckTime = std::chrono::system_clock::now();
	}


	ConnectedUserManager::~ConnectedUserManager()
	{
	}

	void ConnectedUserManager::Init(const int maxSessionCount, TcpNetwork* pNetwork, ServerLog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;

		for (int i = 0; i < maxSessionCount; ++i)
		{
			ConnectedUserList.emplace_back(ConnectedUser());
		}
	}

	void ConnectedUserManager::SetConnectSession(const int sessionIndex)
	{
		time(&ConnectedUserList[sessionIndex].m_ConnectedTime);
	}

	void ConnectedUserManager::SetLogin(const int sessionIndex)
	{
		ConnectedUserList[sessionIndex].m_IsLoginSuccess = true;
	}

	void ConnectedUserManager::SetDisConnectSession(const int sessionIndex)
	{
		ConnectedUserList[sessionIndex].Clear();
	}

	void ConnectedUserManager::LoginCheck()
	{
		auto curTime = std::chrono::system_clock::now();
		auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_LatestLoginCheckTime);

		if (diffTime.count() < HEARTBEAT_RATE)
		{
			return;
		}
		else
		{
			m_LatestLoginCheckTime = curTime;
		}

		auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

		const auto maxSessionCount = (int)ConnectedUserList.size();

		if (m_LatestLogincheckIndex >= maxSessionCount) {
			m_LatestLogincheckIndex = -1;
		}

		++m_LatestLogincheckIndex;

		auto lastCheckIndex = m_LatestLogincheckIndex + 100;
		if (lastCheckIndex > maxSessionCount) {
			lastCheckIndex = maxSessionCount;
		}

		for (; m_LatestLogincheckIndex < lastCheckIndex; ++m_LatestLogincheckIndex)
		{
			auto i = m_LatestLogincheckIndex;

			if (ConnectedUserList[i].m_ConnectedTime == 0 || // connectTIme = 0 이면 접속 안한 사람
				ConnectedUserList[i].m_IsLoginSuccess)
			{
				continue;
			}

			auto diff = curSecTime - ConnectedUserList[i].m_ConnectedTime;
			if (diff >= 3)
			{
				m_pRefLogger->Write(ServerNetwork::LOG_TYPE::LOG_WARN, "%s | Login Wait Time Over. sessionIndex(%d).", __FUNCTION__, i);
				m_pRefNetwork->ForcingClose(i); // 악의적인 유저 짜름
			}
		}
	}

}