#include "../../Common/Packet.h"
#include "../ServerNetwork/TcpNetwork.h"
#include "../../Common/ErrorCode.h"
#include "User.h"
#include "UserManager.h"
#include "LobbyManager.h"
#include "Lobby.h"
#include "Room.h"
#include "PacketProcess.h"

using PACKET_ID = Common::PACKET_ID;

namespace ServerLogic
{
	ERROR_CODE PacketProcess::RoomEnter(PacketInfo packetInfo)
	{
		CHECK_START

		auto reqPkt = (Common::PktRoomEnterReq*)packetInfo.pRefData;
		Common::PktRoomEnterRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::LOBBY) == false) {
			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_LOBBY_INDEX);
		}

// 		auto pRoom = pLobby->GetRoom(reqPkt->RoomIndex);  // ����! ���� ����� ����� �̰��� �Ҽ� ����. �游��µ� ���� index
// 		if (pRoom == nullptr) {
// 			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
// 		}
// 
// 		// ���� ����� ����� ���� �����
// 		if (reqPkt->IsCreate)							// �� �κе� ������ ����. ���� �����¿��� create�Ҽ� ������ �濡 �������鼭 ���������� ����� ����. check�ϴ� �ڵ尡 �ʿ���
// 		{
// 			auto ret = pRoom->CreateRoom(reqPkt->RoomTitle);
// 			if (ret != ERROR_CODE::NONE) {
// 				CHECK_ERROR(ret);
// 			}
// 		}
// 
		Room* pRoom = nullptr;

		// ���� ����� ����� ���� �����
		if (reqPkt->WantCreate) // ���� ����ڴٴ� ���
		{
			pRoom = pLobby->CreateRoom();
			if (pRoom == nullptr) {
				CHECK_ERROR(ERROR_CODE::ROOM_ENTER_EMPTY_ROOM);
			}

			auto ret = pRoom->CreateRoom(reqPkt->RoomTitle);
			if (ret != ERROR_CODE::NONE) {
				CHECK_ERROR(ret);
			}
		}
		else // ���� ������ �ʰڴٴ� ���
		{
			pRoom = pLobby->GetRoom(reqPkt->RoomIndex);
			if (pRoom == nullptr) {
				CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			}
		}

		auto roomidx = pRoom->GetIndex();

		auto enterRet = pRoom->EnterUser(pUser);
		if (enterRet != ERROR_CODE::NONE) {
			CHECK_ERROR(enterRet);
		}


		// ���� ������ �뿡 ���Դٰ� �����Ѵ�.
		pUser->EnterRoom(lobbyIndex, pRoom->GetIndex());

		// �κ� ������ �������� �˸���
		pLobby->NotifyLobbyLeaveUserInfo(pUser);

		// �κ� �� ������ �뺸�Ѵ�.
		pLobby->NotifyChangedRoomInfo(pRoom->GetIndex());

		// �뿡 �� ���� ���Դٰ� �˸���
		pRoom->NotifyEnterUserInfo(pUser->GetIndex(), pUser->GetID().c_str());

		resPkt.RoomIndex = pRoom->GetIndex();
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	//goto
	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::RoomUserList(PacketInfo packetInfo)
	{
		CHECK_START;


		auto result = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorcode = std::get<0>(result);
		
		if (errorcode != ERROR_CODE::NONE) 
		{
			CHECK_ERROR(errorcode);
		}

		auto pUser = std::get<1>(result);
		
		if (pUser->IsCurDomainState(User::DOMAIN_STATE::ROOM)==false)
		{
			CHECK_ERROR(ERROR_CODE::ROOM_USER_LIST_INVALID_DOMAIN);
		}
		
		auto pLobby = m_pRefLobbyMgr->GetLobby(pUser->GetLobbyIndex());
		if (pLobby == nullptr) 
		{
			CHECK_ERROR(ERROR_CODE::ROOM_USER_LIST_INVALID_LOBBY_INDEX);
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr)
		{
			CHECK_ERROR(ERROR_CODE::ROOM_USER_LIST_INVALID_ROOM_INDEX);
		}

		auto reqPkt = (Common::PktRoomUserInfoReq*)packetInfo.pRefData;
		pRoom->SendUserList(pUser->GetSessioIndex());

		return ERROR_CODE::NONE;

	CHECK_ERR:
		Common::PktRoomUserInfoRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_USER_LIST_RES, sizeof(Common::PktBase), (char*)&resPkt);
		return (ERROR_CODE)__result;

	}


	ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		CHECK_START

		Common::PktRoomLeaveRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);
		auto userIndex = pUser->GetIndex();

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::ROOM) == false) {
			CHECK_ERROR(ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_LOBBY_INDEX);
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) {
			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
		}

		auto leaveRet = pRoom->LeaveUser(userIndex);
		if (leaveRet != ERROR_CODE::NONE) {
			CHECK_ERROR(leaveRet);
		}

		// ���� ������ �κ�� ����
		pUser->EnterLobby(lobbyIndex);

		// �뿡 ������ �������� �뺸
		pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());

		// �κ� ���ο� ������ �������� �뺸
		pLobby->NotifyLobbyEnterUserInfo(pUser);

		// �κ� �ٲ� �� ������ �뺸
		pLobby->NotifyChangedRoomInfo(pRoom->GetIndex());

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	//goto
	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}

	ERROR_CODE PacketProcess::RoomChat(PacketInfo packetInfo)
	{
		CHECK_START

		auto reqPkt = (Common::PktRoomChatReq*)packetInfo.pRefData;
		Common::PktRoomChatRes resPkt;

		auto pUserRet = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pUserRet);

		if (errorCode != ERROR_CODE::NONE) {
			CHECK_ERROR(errorCode);
		}

		auto pUser = std::get<1>(pUserRet);

		if (pUser->IsCurDomainState(User::DOMAIN_STATE::ROOM) == false) {
			CHECK_ERROR(ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
		}

		auto lobbyIndex = pUser->GetLobbyIndex();
		auto pLobby = m_pRefLobbyMgr->GetLobby(lobbyIndex);
		if (pLobby == nullptr) {
			CHECK_ERROR(ERROR_CODE::ROOM_CHAT_INVALID_LOBBY_INDEX);
		}

		auto pRoom = pLobby->GetRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) {
			CHECK_ERROR(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
		}

		pRoom->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;

	//goto
	CHECK_ERR:
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return (ERROR_CODE)__result;
	}


}