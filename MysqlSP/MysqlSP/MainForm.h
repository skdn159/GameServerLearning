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

class DBConnector;
class DBProcess;

class MainForm
{
public:
	MainForm();
	~MainForm();

	void Init();
	void CreateGUI();
	void ShowModal();

private:
	void InsertRecords();
	void SearchRecords();
	void DeleteRecords();


private:
	std::unique_ptr<form> m_fm;

	std::shared_ptr<DBConnector> m_DBConnector;
	std::shared_ptr<DBProcess> m_DBProcess;

	std::shared_ptr<button> m_ConnectBtn;

	std::shared_ptr<label> m_CurStateLabel;
	std::shared_ptr<textbox> m_IDtxtBox;
	std::shared_ptr<textbox> m_PWtxtBox;

	std::shared_ptr<button> m_InsertBtn;
	std::shared_ptr<button> m_SearchBtn;
	std::shared_ptr<button> m_DeleteBtn;



};

