#include "../../Common/Packet.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "User.h"
#include "UserManager.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PacketProcess.h"

using PACKET_ID = Common::PACKET_ID;

namespace ServerLogic
{
	ERROR_CODE PacketProcess::LobbyEnter(PacketInfo packetInfo)
	{
		CHECK_START
			// 현재 위치 상태는 로그인이 맞나?
			// 로비에 들어간다.
			// 기존 로비에 있는 사람에게 새 사람이 들어왔다고 알려준다

		auto reqPkt = (Common::PktLobbyEnterReq*)packetInfo.pRefData;
		Common::PktLobbyEnterRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOGIN) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(reqPkt->LobbyId);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ENTER_INVALID_LOBBY_INDEX);
		}

		auto enterRet = pLobby->EnterUser(pUser);
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}

		pLobby->NotifyLobbyEnterUserInfo(pUser);

		resPkt.MaxUserCount = pLobby->GetMaxUserCount();
		resPkt.MaxRoomCount = pLobby->GetMaxRoomCount();
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(Common::PktLobbyEnterRes), (char*)&resPkt);
		
		printf("[Lobby] send Lobby Enter RES! \n ");
		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_RES, sizeof(Common::PktLobbyEnterRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::LobbyRoomList(PacketInfo packetInfo)
	{
		CHECK_START
			// 현재 로비에 있는지 조사한다.
			// 룸 리스트를 보내준다.

			auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_ROOM_LIST_INVALID_LOBBY_INDEX);
		}

		auto reqPkt = (Common::PktLobbyRoomListReq*)packetInfo.pRefData;

		pLobby->SendRoomList(pUser->GetSessioIndex(), reqPkt->StartRoomIndex);

		return ERROR_CODE::NONE;
	CHECK_ERR:
		Common::PktLobbyRoomListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES, sizeof(Common::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::LobbyUserList(PacketInfo packetInfo)
	{
		CHECK_START
			// 현재 로비에 있는지 조사한다.
			// 유저 리스트를 보내준다.

			auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_USER_LIST_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_USER_LIST_INVALID_LOBBY_INDEX);
		}

		auto reqPkt = (Common::PktLobbyUserListReq*)packetInfo.pRefData;

		pLobby->SendUserList(pUser->GetSessioIndex(), reqPkt->StartUserIndex);

		return ERROR_CODE::NONE;
	CHECK_ERR:
		Common::PktLobbyUserListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, sizeof(Common::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}


	ERROR_CODE PacketProcess::LobbyLeave(PacketInfo packetInfo)
	{
		CHECK_START
			// 현재 로비에 있는지 조사한다.
			// 로비에서 나간다
			// 기존 로비에 있는 사람에게 나가는 사람이 있다고 알려준다.
			Common::PktLobbyLeaveRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_DOMAIN);
		}

		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_LEAVE_INVALID_LOBBY_INDEX);
		}

		auto enterRet = pLobby->LeaveUser(pUser->GetIndex());
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}

		pLobby->NotifyLobbyLeaveUserInfo(pUser);

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(Common::PktLobbyLeaveRes), (char*)&resPkt);

		return ERROR_CODE::NONE;
	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LEAVE_RES, sizeof(Common::PktLobbyLeaveRes), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}


	ERROR_CODE PacketProcess::LobbyChat(PacketInfo packetInfo)
	{
		CHECK_START
			auto reqPkt = (Common::PktLobbyChatReq*)packetInfo.pRefData;
		Common::PktLobbyChatRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::LOBBY_CHAT_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::LOBBY_CHAT_INVALID_LOBBY_INDEX);
		}


		pLobby->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}


	ERROR_CODE PacketProcess::Whisper(PacketInfo packetInfo)
	{
		CHECK_START
			auto reqPkt = (Common::PktWhisperReq*)packetInfo.pRefData;
		Common::PktWhisperRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::WHISPER_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::WHISPER_INVALID_LOBBY_INDEX);
		}

		auto receiveUserRet = m_pRefUserMgr->GetUser(reqPkt->ReceiveID);
		auto receiveErrorCode = std::get<0>(receiveUserRet);
		if (receiveErrorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(receiveErrorCode);
		}
		auto receiveUser = std::get<1>(receiveUserRet);

//  		auto receiveUser = m_pRefUserMgr->FindUser(reqPkt->ReceiveID);
 		if (receiveUser == nullptr)
 		{
 			printf("null ptr whisper \n");
 			return ERROR_CODE::WHISPER_INVALID_DOMAIN;
 		}

		pLobby->NotifyWhisper(receiveUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);


//  		Common::PktWhisperNtf whisperPkt;
//  		strncpy_s(whisperPkt.UserID, _countof(whisperPkt.UserID), pUser->GetID().c_str(), Common::MAX_USER_ID_SIZE);
//  		wcsncpy_s(whisperPkt.Msg, Common::MAX_WHISPER_MSG_SIZE + 1, reqPkt->Msg, Common::MAX_WHISPER_MSG_SIZE);// 		m_pRefNetwork->SendData(receiveUser->GetSessioIndex(), (short)PACKET_ID::WHISPER_NTF, sizeof(whisperPkt), (char*)&whisperPkt);


		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::WHISPER_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::WHISPER_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;

	}

}