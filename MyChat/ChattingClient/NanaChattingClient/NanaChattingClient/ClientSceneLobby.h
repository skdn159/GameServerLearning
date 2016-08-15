#pragma once
#include <list>


#include "ClientSceneBase.h"

class ClientSceneLobby : public ClientSceneBase
{
public:
	ClientSceneLobby();
	virtual~ClientSceneLobby();

	virtual void Update() override;
	bool ProcessPacket(const short packetId, char* pData) override;

	void CreateUI(form* pform);
	void Init(const int maxUserCount);
	void RequestRoomList(const short startIndex);
	void RequestUserList(const short startIndex);
	void SetRoomListGui();
	void SetUserListGui();
	void UpdateRoomInfo(Common::RoomSmallInfo* pRoomInfo);
	void UpdateUserInfo(bool IsRemove, std::string userID);

	void RequestCreateRoom();
	void RequestEnterRoom();


private:
	form* m_pForm = nullptr;
	std::shared_ptr<listbox> m_LobbyRoomList;
	std::shared_ptr<listbox> m_LobbyUserList;
	int m_MaxUserCount = 0;
	int m_LobbyNum = -1;

	bool m_IsRoomListWorking = false;
	std::list<Common::RoomSmallInfo> m_RoomList;

	bool m_IsUserListWorking = false;
	std::list<std::string> m_UserList;

	std::shared_ptr<label> m_LobbyIdTitleLb;


	std::shared_ptr<button> m_CreateRoombtn;
	std::shared_ptr<button> m_JoinRoombtn;
	std::shared_ptr<textbox> m_RoomNametxt;


};
