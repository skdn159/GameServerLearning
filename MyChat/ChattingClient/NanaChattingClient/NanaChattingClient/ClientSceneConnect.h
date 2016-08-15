#pragma once

#include "ClientSceneBase.h"
#include "TcpNetwork.h"

class ClientSceneConnect : public ClientSceneBase
{
public:
	ClientSceneConnect();
	virtual ~ClientSceneConnect();

	virtual void Update() override;
	bool ProcessPacket(const short packetId, char* pData) override;
	void CreateUI(form* pform);


private:
	void ConnectOrDisConnect();
	void LogInOut();



private:
	bool m_IsLogined = false;
	form* m_pForm = nullptr;
	form m_form;

	std::shared_ptr<label> m_IPlb;
	std::shared_ptr<textbox> m_IPtxt;

	std::shared_ptr<label> m_Portlb;
	std::shared_ptr<textbox> m_Porttxt;

	std::shared_ptr<button> m_Connectbtn;


	std::shared_ptr<label> m_IDlb;
	std::shared_ptr<textbox> m_IDtxt;

	std::shared_ptr<label> m_PWlb;
	std::shared_ptr<textbox> m_PWtxt;

	std::shared_ptr<button> m_Loginbtn;


};

