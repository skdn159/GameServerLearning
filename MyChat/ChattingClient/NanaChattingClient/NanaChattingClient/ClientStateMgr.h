#pragma once
#include <string.h>

#include "MainForm.h"
// 내 State 를 관리하고 caption 처리를함
//Todo : Scene을 껍데기로 놓고 Network는 StateMgr에서 처리..



enum class CLIENT_SCENE_TYPE
{
	CONNECT = 0,
	LOGIN = 1,
	LOBBY = 2,
	ROOM = 3,
};

class ClientStateMgr
{
public:
	ClientStateMgr();
	~ClientStateMgr();

	void Init(form* pform);
	void ChangeTxtBoxCurState(std::string addStr) { m_pTxtBoxCurState->caption("State = " + addStr); }
	
	void SetLobbyNum(int lobbyNum) { m_LobbyNum = lobbyNum; }
	int GetLobbyNum() { return m_LobbyNum; }
// 	void SetRoomNum(int roomNum) { m_RoomNum = roomNum; }
// 	int GetRoomNum() { return m_RoomNum; }



private:

	form* m_pForm;
	std::unique_ptr<textbox> m_pTxtBoxCurState;

	int m_LobbyNum;
	int m_RoomNum;
	// bool Login Y/N
	// bool Connect Y/N

};

