#pragma once
#include <string>

namespace ServerLogic 
{
	class User
	{
	public:
		enum class DOMAIN_STATE
		{
			NONE = 0,
			LOGIN = 1,
			LOBBY = 2,
			ROOM = 3,
		};

		User();
		virtual ~User();

		void Init(const short index) { m_Index = index; }
		void Clear();
		void Set(const int sessionIndex, const char* strID);
		void EnterLobby(const short lobbyIndex);
		void EnterRoom(const short lobbyIndex, const short roomIndex);

		void LeaveLobby();

		short GetIndex() { return m_Index; }
		short GetRoomIndex() { return m_RoomIndex; }

		int GetSessioIndex() { return m_SessionIndex; }
		std::string& GetID() { return m_ID; }
		bool IsConfirm() { return m_IsConfirm; }
		short GetLobbyIndex() { return m_LobbyIndex; }

		bool IsCurDomainState(DOMAIN_STATE domainState);

	protected:
		short m_Index = -1;
		int m_SessionIndex = -1;
		std::string m_ID;
		bool m_IsConfirm = false;
		DOMAIN_STATE m_CurDomainState = DOMAIN_STATE::NONE;
		short m_LobbyIndex = -1;
		short m_RoomIndex = -1;
	};
}

