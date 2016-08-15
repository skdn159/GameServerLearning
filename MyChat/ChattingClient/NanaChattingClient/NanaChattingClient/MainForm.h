#pragma once
#include <memory>
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui.hpp>


#ifdef NDEBUG
#pragma comment(lib, "nana_v140_Release_x86.lib")
#else
#pragma comment(lib, "nana_v140_Debug_x86.lib")
#endif

using namespace nana;
class TcpNetwork;


class ClientBase;
class ClientSceneConnect;
class ClientSceneLogin;
class ClientSceneLobby;
class ClientSceneRoom;
class ClientStateMgr;

class MainForm
{
public:
	MainForm();
	~MainForm();

	void Init();

	void CreateGUI();

	void ShowModal();


private:
	void PacketProcess();

private:
	std::unique_ptr<TcpNetwork> m_Network;

	bool m_IsLogined = false;


private:
	std::unique_ptr<form> m_fm; 
	timer m_timer;



	std::shared_ptr<ClientStateMgr> m_pClientStateMgr;
 	std::shared_ptr<ClientSceneConnect> m_pClientSceneConnect;
 	std::shared_ptr<ClientSceneLogin> m_pClientSceneLogin;
 	std::shared_ptr<ClientSceneLobby> m_pClientSceneLobby;
	std::shared_ptr<ClientSceneRoom> m_pClientSceneRoom;

};

