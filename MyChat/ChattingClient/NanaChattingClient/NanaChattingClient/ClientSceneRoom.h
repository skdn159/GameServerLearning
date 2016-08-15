#pragma once
#include <list>

#include "ClientSceneBase.h"
class ClientSceneRoom : public ClientSceneBase
{
public:
	ClientSceneRoom();
	~ClientSceneRoom();


	virtual void Update() override;
	bool ProcessPacket(const short packetId, char* pData) override;


	void CreateUI(form* pform);

	//void Set

	void UpdateUserInfo(bool IsRemove, std::string userID);

	void RequestRoomUserList();
	void RequestRoomChattingMsg();


private:

	form* m_pForm = nullptr;
	int m_RoomNum = -1;

	std::shared_ptr<listbox> m_RoomUserList;

	std::shared_ptr<textbox> m_ChattingWindow;
	std::shared_ptr<textbox> m_Sendingtxt;
	std::shared_ptr<button> m_ChattingSendBttn;

	std::list<Common::UserSmallInfo> m_UserInfoList;
	std::list<std::string> m_UserIDList;

	
	//bool m_IsUserListWorking = false;

};

