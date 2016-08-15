#include "MainForm.h"
#include "TcpNetwork.h"
#include "ClientStateMgr.h"
#include "ClientSceneConnect.h"
#include "ClientSceneLogin.h"
#include "ClientSceneLobby.h"
#include "ClientSceneRoom.h"

MainForm::MainForm()
{

}

MainForm::~MainForm()
{
	if (m_Network)
	{
		m_Network->DisConnect();
	}
}

void MainForm::Init()
{
	m_Network = std::make_unique<TcpNetwork>();

	m_pClientSceneConnect = std::make_shared<ClientSceneConnect>();
	m_pClientSceneConnect->SetNetwork(m_Network.get());

	m_pClientSceneLogin = std::make_shared<ClientSceneLogin>();
	m_pClientSceneLogin->SetNetwork(m_Network.get());

	m_pClientSceneLobby = std::make_shared<ClientSceneLobby>();
	m_pClientSceneLobby->SetNetwork(m_Network.get());

 	m_pClientSceneRoom = std::make_shared<ClientSceneRoom>();
 	m_pClientSceneRoom->SetNetwork(m_Network.get());

	//Todo : Scene을 껍데기로 놓고 Network는 StateMgr에서 처리..

}

void MainForm::CreateGUI()
{
	m_fm = std::make_unique<form>(API::make_center(900, 700));
	m_fm->caption("Chatting Client");

	m_pClientStateMgr = std::make_shared<ClientStateMgr>();
	m_pClientStateMgr->Init(m_fm.get());

	m_pClientSceneConnect->SetClientStateMgr(m_pClientStateMgr.get());
	m_pClientSceneLogin->SetClientStateMgr(m_pClientStateMgr.get());
	m_pClientSceneLobby->SetClientStateMgr(m_pClientStateMgr.get());
	m_pClientSceneRoom->SetClientStateMgr(m_pClientStateMgr.get());

	m_pClientSceneConnect->CreateUI(m_fm.get());
	m_pClientSceneLogin->CreateUI(m_fm.get());
	m_pClientSceneLobby->CreateUI(m_fm.get());
	m_pClientSceneRoom->CreateUI(m_fm.get());

	m_timer.elapse([&]() { PacketProcess(); });  // 타이머가 Packet 받아서 32milSec로 계속 Update 호출
	m_timer.interval(32);
	m_timer.start();
}

void MainForm::ShowModal()
{
	m_fm->show();

	exec();
}

void MainForm::PacketProcess()
{
	if (!m_Network) {
		return;
	}


	auto packet = m_Network->GetPacket();

	if (packet.PacketId != 0)
	{
		m_pClientSceneConnect->ProcessPacket(packet.PacketId, packet.pData);
		m_pClientSceneLogin->ProcessPacket(packet.PacketId, packet.pData);
		m_pClientSceneLobby->ProcessPacket(packet.PacketId, packet.pData);
		m_pClientSceneRoom->ProcessPacket(packet.PacketId, packet.pData);

		if (packet.pData != nullptr) {
			delete[] packet.pData;
		}
	}

	m_pClientSceneConnect->Update();
	m_pClientSceneLogin->Update();
	m_pClientSceneLobby->Update();
	m_pClientSceneRoom->Update();
}
