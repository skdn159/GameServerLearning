#include <thread>
#include <chrono>


#include "../ServerNetwork/Define.h"
#include "../ServerNetwork/ServerNetworkErrorEnum.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "LobbyManager.h"
#include "consoleUtil.h"
#include "Main.h"
//#include <winbase.h>

using LOG_TYPE = ServerNetwork::LOG_TYPE;
using NETWORK_ERROR_CODE = ServerNetwork::NETWORK_ERROR_CODE;

namespace ServerLogic 
{
	Main::Main()
	{

	}

	Main::~Main()
	{
		Release();
	}

	ERROR_CODE Main::Init()
	{
		m_pLogger = std::make_unique<ConsoleLog>();

		LoadConfig();

		m_pNetwork = std::make_unique<ServerNetwork::TcpNetwork>();
		auto result = m_pNetwork->Init(m_pServerConfig.get(), m_pLogger.get());

		if (result != NETWORK_ERROR_CODE::NONE)
		{
			m_pLogger->Write(LOG_TYPE::LOG_ERROR, "%s | Init Fail. NetErrorCode(%s)", __FUNCTION__, (short)result);
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}

		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(m_pServerConfig->maxClientCount);

		m_pLobbyMgr = std::make_unique<LobbyManager>();
		m_pLobbyMgr->Init({ m_pServerConfig->maxLobbyCount,
			m_pServerConfig->maxLobbyUserCount,
			m_pServerConfig->maxRoomCountByLobby,
			m_pServerConfig->maxRoomUserCount },
			m_pNetwork.get(), m_pLogger.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(), m_pLobbyMgr.get(), m_pLogger.get());

		m_IsRun = true;

		m_pLogger->Write(LOG_TYPE::LOG_INFO, "%s | Init Success. Server Run", __FUNCTION__);
		return ERROR_CODE::NONE;
	}


	void Main::Run()
	{
		while (m_IsRun)
		{
			m_pNetwork->Run();

			while (true)
			{
				auto packetInfo = m_pNetwork->GetPacketInfo();

				if (packetInfo.PacketId == 0)
				{
					break;
				}

				m_pPacketProc->Process(packetInfo);
			}



			m_pPacketProc->StateCheck();

			//std::this_thread::sleep_for(std::chrono::milliseconds(0)); 
			// 0초 쉬는것은 내가 스레드를 다 쓰지 않고 다른 스레드에 양보한단 소리. (이 코드 없으면 cpu점유율 올라감)
			// Qstn : 이 코드 왜 안쓰지?
		}
	}

	void Main::Stop()
	{
		m_IsRun = false;
	}

	ERROR_CODE Main::LoadConfig()
	{
		m_pServerConfig = std::make_unique<ServerNetwork::ServerConfig>();

		wchar_t directoryPath[MAX_PATH] = { 0, };
		::GetCurrentDirectory(MAX_PATH, directoryPath);

		wchar_t iniPath[MAX_PATH] = { 0, };
		//_snwprintf_s(iniPath, _countof(iniPath), _TRUNCATE, L"%s\\ServerConfig.ini", directoryPath);
		_snwprintf_s(iniPath, _countof(iniPath), _TRUNCATE, L"%s\\..\\..\\..\\Bin\\ServerConfig.ini", directoryPath);

		m_pServerConfig->port = (unsigned short)GetPrivateProfileInt(L"Config", L"Port", 0, iniPath);
		m_pServerConfig->backLogCount = GetPrivateProfileInt(L"Config", L"BackLogCount", 0, iniPath);
		m_pServerConfig->maxClientCount = GetPrivateProfileInt(L"Config", L"MaxClientCount", 0, iniPath);

		m_pServerConfig->maxClientSockOptRecvBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSockOptRecvBufferSize", 0, iniPath);
		m_pServerConfig->maxClientSockOptSendBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSockOptSendBufferSize", 0, iniPath);
		m_pServerConfig->maxClientRecvBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientRecvBufferSize", 0, iniPath);
		m_pServerConfig->maxClientSendBufferSize = (short)GetPrivateProfileInt(L"Config", L"MaxClientSendBufferSize", 0, iniPath);

		m_pServerConfig->extraClientCount = GetPrivateProfileInt(L"Config", L"ExtraClientCount", 0, iniPath);
		m_pServerConfig->maxLobbyCount = GetPrivateProfileInt(L"Config", L"MaxLobbyCount", 0, iniPath);
		m_pServerConfig->maxLobbyUserCount = GetPrivateProfileInt(L"Config", L"MaxLobbyUserCount", 0, iniPath);
		m_pServerConfig->maxRoomCountByLobby = GetPrivateProfileInt(L"Config", L"MaxRoomCountByLobby", 0, iniPath);
		m_pServerConfig->maxRoomUserCount = GetPrivateProfileInt(L"Config", L"MaxRoomUserCount", 0, iniPath);

		//m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | Port(%d), Backlog(%d)", __FUNCTION__, m_pServerConfig->Port, m_pServerConfig->BackLogCount);
		return ERROR_CODE::NONE;



	}

	void Main::Release()
	{

	}

}
