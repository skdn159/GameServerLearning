#include "DBConnector.h"
#include <sqltypes.h>

#include <string>

DBConnector::DBConnector()
{
}


DBConnector::~DBConnector()
{
	DisConnect();
}

bool DBConnector::Init()
{
	int ret;



	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION,
		(SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
	
	//CheckRet(ret);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		//m_hEnv = &hEnv;






		printf("SQL SET SUCCESS!");
		return true;
	}
	else
	{
		printf("SQL SET FAILED!");
		return false;
	}
}

bool DBConnector::Connect()
{
	int ret;
	ret = SQLConnect(m_hDbc, (SQLWCHAR*)L"MYODBC", SQL_NTS,
		(SQLWCHAR*)L"root", SQL_NTS,
		(SQLWCHAR*)L"1q2w3e4r", SQL_NTS);

	//ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
	//ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)L"USE gameserver;", SQL_NTS);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		printf("SQL Connect Ok \n");
		return true;
	}
	else { 
		printf("SQL Connect ERROR .\n");
		return false;
	}

}

bool DBConnector::CheckRet(int ret)
{
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void DBConnector::DisConnect()
{

}
// bool DBConnector::dbsearch(std::string id, std::string pw)
// {
// 		int ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
// 
// 		std::string qry = "CALL SP_SearchUID('" + id + "', '" + pw + "', @ret);";
// 		
// 		//std::string qry = "CALL SP_SearchUID('testId','asdf1234',@ret);";
// 		std::wstring wQry;
// 		
// 		wQry.assign(qry.begin(), qry.end());
// 	
// 
// 		//wQry = L"CALL SP_SearchUID('testId','asdf1234',@ret);";
// 
// 		ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)wQry.c_str(), SQL_NTS);
// 		//ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)L"CALL SP_SearchUID('testId','asdf1234',@ret);", SQL_NTS);
// 
// 		if (ret == SQL_SUCCESS)
// 		{
// 			ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)L"SELECT @ret", SQL_NTS);
// 
// 		}
// 
// 
// 
// 		return true;
// 
// }

