#pragma once
#include "Windows.h"
#include "sql.h"
#include "sqlext.h"
#include <iostream>
#include <tchar.h>


class DBConnector
{
public:
	DBConnector();
	~DBConnector();

	bool Init();
	bool Connect();

	void DisConnect();
		

	SQLHENV& GetHENV() { return m_hEnv; }
	SQLHDBC& GetHDBC() { return m_hDbc; }
	SQLHSTMT& GetHSTMT() { return m_hStmt; }

private:
	bool CheckRet(int ret);


private:
	SQLHENV m_hEnv;
	SQLHDBC m_hDbc;
	SQLHSTMT m_hStmt;



};

