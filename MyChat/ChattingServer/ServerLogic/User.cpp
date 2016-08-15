#include "User.h"

namespace ServerLogic
{

	User::User()
	{
	}


	User::~User()
	{
	}

	void ServerLogic::User::Clear()
	{
		m_SessionIndex = 0;
		m_ID = "";
		m_IsConfirm = false;
		m_CurDomainState = DOMAIN_STATE::NONE;
		m_LobbyIndex = -1;
		m_RoomIndex = -1;
	}

	void ServerLogic::User::Set(const int sessionIndex, const char* strID)
	{
		m_IsConfirm = true;
		m_CurDomainState = DOMAIN_STATE::LOGIN;
		m_SessionIndex = sessionIndex;
		m_ID = strID;
	}

	void User::EnterLobby(const short lobbyIndex)
	{
		m_LobbyIndex = lobbyIndex;
		m_CurDomainState = DOMAIN_STATE::LOBBY;
	}

	void User::EnterRoom(const short lobbyIndex, const short roomIndex)
	{
		m_LobbyIndex = lobbyIndex;
		m_RoomIndex = roomIndex;
		m_CurDomainState = DOMAIN_STATE::ROOM;
	}

	void User::LeaveLobby()
	{
		m_CurDomainState = DOMAIN_STATE::LOGIN;
	}

	bool User::IsCurDomainState(DOMAIN_STATE domainState)
	{
		bool isCurstate = false;
		switch (domainState)
		{
		case ServerLogic::User::DOMAIN_STATE::NONE:
			if (m_CurDomainState == DOMAIN_STATE::NONE) { isCurstate = true;  }
			//return m_CurDomainState == DOMAIN_STATE::NONE ? true : false;
			break;			
		case ServerLogic::User::DOMAIN_STATE::LOGIN:
			if (m_CurDomainState == DOMAIN_STATE::LOGIN) { isCurstate = true; }
			//return m_CurDomainState == DOMAIN_STATE::LOGIN ? true : false;
			break;
		case ServerLogic::User::DOMAIN_STATE::LOBBY:
			if (m_CurDomainState == DOMAIN_STATE::LOBBY) { isCurstate = true; }
			//return m_CurDomainState == DOMAIN_STATE::LOBBY ? true : false;
			break;
		case ServerLogic::User::DOMAIN_STATE::ROOM:
			if (m_CurDomainState == DOMAIN_STATE::ROOM) { isCurstate = true; }
			//return m_CurDomainState == DOMAIN_STATE::ROOM ? true : false;
			break;
			//Debug. Err?
		}
		return isCurstate;

		// 		bool IsCurDomainInLogIn() {
		// 			return m_CurDomainState == DOMAIN_STATE::LOGIN ? true : false;
		// 		}
		// 
		// 		bool IsCurDomainInLobby() {
		// 			return m_CurDomainState == DOMAIN_STATE::LOBBY ? true : false;
		// 		}
		// 
		// 		bool IsCurDomainInRoom() {
		// 			return m_CurDomainState == DOMAIN_STATE::ROOM ? true : false;
		// 		}
	}

}