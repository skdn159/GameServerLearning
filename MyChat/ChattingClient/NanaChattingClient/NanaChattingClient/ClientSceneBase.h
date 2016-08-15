#pragma once

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


#include "../../../Common/PacketID.h"
using PACKET_ID = Common::PACKET_ID;

#include "TcpNetwork.h"
#include "ClientStateMgr.h"

static CLIENT_SCENE_TYPE m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;

class ClientSceneBase
{
public:
	ClientSceneBase() {}
	virtual ~ClientSceneBase() {}

	virtual void Update() {}
	virtual bool ProcessPacket(const short packetId, char* pData) { return false; }

	void SetNetwork(TcpNetwork* pNetwork) { m_pRefNetwork = pNetwork; }
	void SetClientStateMgr(ClientStateMgr* pCLmgr) { m_pRefClientStateMgr = pCLmgr; }

	static void UnicodeToAnsi(const wchar_t* pszText, const int destSize, char* pszDest)
	{
		_snprintf_s(pszDest, destSize, _TRUNCATE, "%S", pszText);
	}

	static CLIENT_SCENE_TYPE GetCurSceneType() { return m_CurSceneType; }
	static void SetCurSceneType(const CLIENT_SCENE_TYPE sceneType) { m_CurSceneType = sceneType; ChangeUI(sceneType); }

	static void ChangeUI(const CLIENT_SCENE_TYPE sceneType) { // do sth...e.g highlight
	}




protected:
	TcpNetwork* m_pRefNetwork;
	//static CLIENT_SCENE_TYPE m_CurSceneType;

	ClientStateMgr* m_pRefClientStateMgr;
};

//CLIENT_SCENE_TYPE ClientSceneBase::m_CurSceneType = CLIENT_SCENE_TYPE::CONNECT;