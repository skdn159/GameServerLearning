#pragma once
#include <memory>

#include "../../Common/Packet.h"
#include "../ServerNetwork/Define.h"
#include "../../Common/ErrorCode.h"

using ERROR_CODE = Common::ERROR_CODE;

namespace ServerNetwork
{
	class TcpNetwork;
	class ServerLog;
}

namespace ServerLogic
{
	class UserManager;
	class LobbyManager;
	class ConnectedUserManager;

#define CHECK_START  ERROR_CODE __result=ERROR_CODE::NONE;
#define CHECK_ERROR(f) __result=f; goto CHECK_ERR;

	class PacketProcess
	{
		using PacketInfo = ServerNetwork::RecvPacketInfo;
		typedef ERROR_CODE(PacketProcess::*PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)Common::PACKET_ID::MAX];

		using ServerLog = ServerNetwork::ServerLog;
		using TcpNetwork = ServerNetwork::TcpNetwork;

	public:
		PacketProcess();
		~PacketProcess();

		void Init(TcpNetwork* pNetwork, UserManager* pUserMgr, LobbyManager* pLobbyMgr, ServerLog* pLogger);

		void Process(PacketInfo packetInfo);

		void StateCheck();

	private:
		ServerLog* m_pRefLogger;
		TcpNetwork* m_pRefNetwork;
		UserManager* m_pRefUserMgr;
		LobbyManager* m_pRefLobbyMgr;
		std::unique_ptr<ConnectedUserManager> m_pConnectedUserManager;



	private:
		ERROR_CODE NtfSysConnctSession(PacketInfo packetInfo);
		ERROR_CODE NtfSysCloseSesson(PacketInfo packetInfo);

		ERROR_CODE Login(PacketInfo packetInfo);

		ERROR_CODE LobbyList(PacketInfo packetInfo);
		ERROR_CODE LobbyEnter(PacketInfo packetInfo);
		ERROR_CODE LobbyRoomList(PacketInfo packetInfo);
		ERROR_CODE LobbyUserList(PacketInfo packetInfo);
		ERROR_CODE LobbyLeave(PacketInfo packetInfo);
		ERROR_CODE LobbyChat(PacketInfo packetInfo);
		ERROR_CODE Whisper(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);
		ERROR_CODE RoomUserList(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);
		ERROR_CODE RoomChat(PacketInfo packetInfo);

	};
}