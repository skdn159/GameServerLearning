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
	m_DBProcess = std::make_shared<DBProcess>();

}

void MainForm::CreateGUI()
{

}

void MainForm::ShowModal()
{
	m_fm->show();

	exec();
}
