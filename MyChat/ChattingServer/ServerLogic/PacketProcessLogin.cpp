#include "../../Common/Packet.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "../ServerLogic/ConnectedUserManager.h"
#include "User.h"
#include "UserManager.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = Common::PACKET_ID;

namespace ServerLogic
{
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
		CHECK_START
			//TODO: ���� �����Ͱ� PktLogInReq ũ�⸸ŭ���� �����ؾ� �Ѵ�.
			// �н������ ������ pass ���ش�.
			// ID �ߺ��̶�� ���� ó���Ѵ�.

			Common::PktLogInRes resPkt;
		auto reqPkt = (Common::PktLogInReq*)packetInfo.pRefData;

		auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);

		if (addRet != ERROR_CODE::NONE) {
			CHECK_ERROR(addRet);
		}

		m_pConnectedUserManager->SetLogin(packetInfo.SessionIndex);


		resPkt.ErrorCode = (short)addRet;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(Common::PktLogInRes), (char*)&resPkt);

		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOGIN_IN_RES, sizeof(Common::PktLogInRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::LobbyList(PacketInfo packetInfo)
	{
		CHECK_START
			// ���� ���� �����ΰ�?
			// ���� �κ� ���� ���� �����ΰ�?

			auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);


		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOGIN) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LIST_INVALID_DOMAIN);
		}

		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);

		return ERROR_CODE::NONE;

	CHECK_ERR:
		Common::PktLobbyListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(Common::PktLogInRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}
}