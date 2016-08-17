#include "MainForm.h"
#include "DBConnector.h"
#include "DBProcess.h"


MainForm::MainForm()
{
}


MainForm::~MainForm()
{
}

void MainForm::Init()
{
	m_DBConnector = std::make_shared<DBConnector>();
	
	if (m_DBConnector->Init())
	{
		m_DBProcess = std::make_shared<DBProcess>();
		m_DBProcess->SetHandler(m_DBConnector->GetHENV(), m_DBConnector->GetHDBC(), m_DBConnector->GetHSTMT());
	}

	else
	{
		printf("SQL DB Init Error");
	}
}

void MainForm::CreateGUI()
{
	m_fm = std::make_unique<form>(API::make_center(400, 400));
	m_fm->caption("DB Procedure");

	m_ConnectBtn = std::make_shared<button>(*m_fm, rectangle(10, 10, 80, 23));
	m_ConnectBtn->caption("Connect DB");
	m_ConnectBtn->events().click([&]() { m_DBConnector->Connect(); m_CurStateLabel->caption("Connected"); });

 	m_CurStateLabel = std::make_shared<label>(*m_fm, rectangle(100, 13, 150, 23));
 	m_CurStateLabel->caption("CurState");
 
 	m_IDtxtBox = std::make_shared<textbox>(*m_fm, rectangle(10, 50, 90, 23));;
 	m_PWtxtBox = std::make_shared<textbox>(*m_fm, rectangle(110, 50, 90, 23));;
 	
 	m_InsertBtn = std::make_shared<button>(*m_fm, nana::rectangle(10, 90, 60, 23));
 	m_SearchBtn = std::make_shared<button>(*m_fm, nana::rectangle(90, 90, 60, 23));
 	m_DeleteBtn = std::make_shared<button>(*m_fm, nana::rectangle(170, 90, 60, 23));
 
 	m_InsertBtn->caption("Insert");
 	m_InsertBtn->events().click([&]() {this->InsertRecords(); });
 
 	m_SearchBtn->caption("Search");
 	m_SearchBtn->events().click([&]() {this->SearchRecords(); });
 
 	m_DeleteBtn->caption("Delete");
 	m_DeleteBtn->events().click([&]() {this->DeleteRecords(); });


}

void MainForm::ShowModal()
{
	m_fm->show();

	exec();
}

void MainForm::InsertRecords()
{
	bool ret;
	std::string ID = m_IDtxtBox->caption();
	std::string PW = m_PWtxtBox->caption();
    ret = m_DBProcess->InsertRecord(ID, PW);

	if (ret == true)
	{
		m_CurStateLabel->caption("Insert Complete");
	}
	else
	{
		m_CurStateLabel->caption("Insert Failed");
	}

}

void MainForm::SearchRecords()
{
	int ret;
	std::string ID = m_IDtxtBox->caption();
	std::string PW = m_PWtxtBox->caption();
	ret = m_DBProcess->SearchRecord(ID, PW);

	if (ret == -1 || ret ==0)
	{
		m_CurStateLabel->caption("None such that User");
	}

	else
	{
		std::string uid = std::to_string(ret);
		m_CurStateLabel->caption("Searched UID = "+uid);
	}
}

void MainForm::DeleteRecords()
{
	int ret;
	std::string ID = m_IDtxtBox->caption();
	std::string PW = m_PWtxtBox->caption();
	ret = m_DBProcess->DeleteRecord(ID, PW);

	if (ret == -1 || ret == 0)
	{
		m_CurStateLabel->caption("Delete Failed");
	}
	else 
	{
		std::string uid = std::to_string(ret);
		m_CurStateLabel->caption("Deleted UID = " + uid);
	}
}

