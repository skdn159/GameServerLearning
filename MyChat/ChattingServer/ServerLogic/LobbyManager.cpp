#include "LobbyManager.h"
#include "../ServerNetwork/ServerLog.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/Packet.h"
#include "../../Common/ErrorCode.h"
#include "Lobby.h"

using ERROR_CODE = Common::ERROR_CODE;
using PACKET_ID = Common::PACKET_ID;

namespace ServerLogic
{

	LobbyManager::LobbyManager()
	{
	}

	LobbyManager::~LobbyManager()
	{
	}

	void ServerLogic::LobbyManager::Init(const LobbyManagerConfig config, TcpNetwork* pNetwork, ServerLog* pLogger)
	{
		m_pRefLogger = pLogger;
		m_pRefNetwork = pNetwork;

		for (int i = 0; i < config.maxLobbyCount; ++i)
		{
			Lobby lobby;
			lobby.Init((short)i, (short)config.maxLobbyUserCount, (short)config.maxRoomCountByLobby, (short)config.maxRoomUserCount);
			lobby.SetNetwork(m_pRefNetwork, m_pRefLogger);

			m_LobbyList.push_back(lobby);
		}
	}

	Lobby* ServerLogic::LobbyManager::GetLobby(short lobbyId)
	{
		if (lobbyId < 0 || lobbyId >= (short)m_LobbyList.size()) {
			return nullptr;
		}

		return &m_LobbyList[lobbyId];
	}

	void ServerLogic::LobbyManager::SendLobbyListInfo(const int sessionIndex)
	{
		Common::PktLobbyListRes resPkt;
		resPkt.ErrorCode = (short)ERROR_CODE::NONE;
		resPkt.LobbyCount = static_cast<short>(m_LobbyList.size());

		int index = 0;
		for (auto& lobby : m_LobbyList)
		{
			resPkt.LobbyList[index].LobbyId = lobby.GetIndex();
			resPkt.LobbyList[index].LobbyUserCount = lobby.GetUserCount();

			++index;
		}

		// ���� �����͸� ���̱� ���� ������� ���� LobbyListInfo ũ��� ���� ������ �ȴ�.
		m_pRefNetwork->SendData(sessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
	}

}