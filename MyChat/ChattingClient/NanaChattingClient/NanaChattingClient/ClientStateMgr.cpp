#include "ClientStateMgr.h"



ClientStateMgr::ClientStateMgr()
{
}


ClientStateMgr::~ClientStateMgr()
{
}

void ClientStateMgr::Init(form* pform)
{
	m_pForm = pform;
	m_pTxtBoxCurState = std::make_unique<textbox>((form&)*m_pForm, nana::rectangle(450, 15, 180, 23));
	m_pTxtBoxCurState->caption("State = Disconnect");
}

