#include "ClientSceneConnect.h"
#include <iostream>
#include <algorithm>

//CLIENT_SCENE_TYPE ClientSceneBase::m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;

ClientSceneConnect::ClientSceneConnect()
{
}


ClientSceneConnect::~ClientSceneConnect()
{
}

void ClientSceneConnect::Update()
{
	if (GetCurSceneType() != CLIENT_SCENE_TYPE::CONNECT) {
		return;
	}
}

bool ClientSceneConnect::ProcessPacket(const short packetId, char* pData)
{
	switch (packetId)
	{
	case (short)PACKET_ID::LOGIN_IN_RES:
	{

		auto pktRes = (Common::PktLogInRes*)pData;

		if (pktRes->ErrorCode == (short)Common::ERROR_CODE::NONE)
		{
			m_pRefClientStateMgr->ChangeTxtBoxCurState("Logined");
			m_Loginbtn->caption("LogOut");
			m_Loginbtn->enabled(true);

			SetCurSceneType(CLIENT_SCENE_TYPE::LOGIN);  // Login 되면 SceneLogin에서 Update가 돌아감
		}
		else
		{
			nana::msgbox m((form&)*m_pForm, "Fail LOGIN_IN_REQ", nana::msgbox::ok);
			m.icon(m.icon_warning);
			m << "ErrorCode: " << pktRes->ErrorCode;
			m.show();
		}
	}
	break;
	default:
		return false;
	}

	return true;
}

void ClientSceneConnect::CreateUI(form* pform)
{
	m_pForm = pform;

	m_IPlb = std::make_shared<label>((form&)*m_pForm, nana::rectangle(22, 17, 18, 18));
	m_IPlb->caption("IP");
	m_IPtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(43, 15, 128, 20));
	m_IPtxt->caption("127.0.0.1");

	m_Portlb = std::make_shared<label>((form&)*m_pForm, nana::rectangle(187, 17, 30, 18));
	m_Portlb->caption("Port");
	m_Porttxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(214, 15, 60, 20));
	m_Porttxt->caption("23452");

	m_Connectbtn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(283, 14, 102, 23));
	m_Connectbtn->caption("Connent");
	m_Connectbtn->events().click([&]() {
		this->ConnectOrDisConnect();
	});


	m_IDlb = std::make_shared<label>((form&)*m_pForm, nana::rectangle(22, 58, 18, 18));
	m_IDlb->caption("ID");
	m_IDtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(43, 56, 115, 20));
	//m_IDtxt->caption("Input ID");

	m_PWlb = std::make_shared<label>((form&)*m_pForm, nana::rectangle(170, 58, 69, 18));
	m_PWlb->caption("PassWord:");
	m_PWtxt = std::make_shared<textbox>((form&)*m_pForm, nana::rectangle(230, 56, 115, 20));
	//m_PWtxt->caption("Input Pssw");

	m_Loginbtn = std::make_shared<button>((form&)*m_pForm, nana::rectangle(353, 54, 102, 23));
	m_Loginbtn->caption("Login");
	m_Loginbtn->events().click([&]() {
		this->LogInOut();
	});
	m_Loginbtn->enabled(false);
}

void ClientSceneConnect::ConnectOrDisConnect()
{
	m_IsLogined = false; // logined를 false로 세팅

	if (m_pRefNetwork->IsConnected() == false) //TcpNetwork가 연결 안되어 있으면
	{
		char szIP[64] = { 0, };
		UnicodeToAnsi(m_IPtxt->caption_wstring().c_str(), 64, szIP);

		if (m_pRefNetwork->ConnectTo(szIP, (unsigned short)m_Porttxt->to_int()))  // Connect 성공시
		{
			m_Connectbtn->caption("DisConnect"); 
			
			m_pRefClientStateMgr->ChangeTxtBoxCurState("Connected");
			m_Loginbtn->enabled(true);
		}
		else
		{
			nana::msgbox mbox((form&)*m_pForm, "Fail Connect", nana::msgbox::ok); // Connect 실패시
			mbox.icon(mbox.icon_warning);
			mbox << "Connect Fail";
			mbox.show();
			m_Loginbtn->enabled(false);

			m_pRefClientStateMgr->ChangeTxtBoxCurState("Disconnected");
		}
	}
	else
	{
		m_pRefNetwork->DisConnect(); //TcpNetwork가 이미 연결되어 있으면

		m_Connectbtn->caption("Connect");
		m_Loginbtn->enabled(false);
	}
}

void ClientSceneConnect::LogInOut()
{
	if (m_IsLogined == false)
	{
		char szID[64] = { 0, };
		UnicodeToAnsi(m_IDtxt->caption_wstring().c_str(), 64, szID);

		char szPW[64] = { 0, };
		UnicodeToAnsi(m_PWtxt->caption_wstring().c_str(), 64, szPW);

		Common::PktLogInReq reqPkt;
		strncpy_s(reqPkt.szID, Common::MAX_USER_ID_SIZE + 1, szID, Common::MAX_USER_ID_SIZE);
		strncpy_s(reqPkt.szPW, Common::MAX_USER_PASSWORD_SIZE + 1, szPW, Common::MAX_USER_PASSWORD_SIZE);

		m_pRefNetwork->SendPacket((short)PACKET_ID::LOGIN_IN_REQ, sizeof(reqPkt), (char*)&reqPkt);

		m_Loginbtn->enabled(false); //보내고 버튼 막아서 packet 막음
	}
	else
	{
		nana::msgbox mbox((form&)*m_pForm, "Unimplemented", nana::msgbox::ok);
		mbox.icon(mbox.icon_warning);
		mbox << "Something Wrong";
		mbox.show();
	}
}
