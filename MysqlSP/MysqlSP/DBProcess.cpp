#include "DBProcess.h"
#include "time.h"
#include <string>
#include <sqltypes.h>

DBProcess::DBProcess()
{
}


DBProcess::~DBProcess()
{
}

void DBProcess::SetHandler(SQLHENV sqlhenv, SQLHDBC sqlhdbc, SQLHSTMT sqlhstmt)
{
	m_hEnv = sqlhenv;
	m_hDbc = sqlhdbc;
	m_hStmt = sqlhstmt;
}

bool DBProcess::InsertRecord(std::string id, std::string pw)
{
	int ret	= SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
	std::string qry = "CALL SP_insertuser(" + std::to_string(GenUID()) + ", '" + id + "', '" + pw + "');";
	std::wstring wQry;
	wQry.assign(qry.begin(), qry.end());
	wprintf(wQry.c_str());
	ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)wQry.c_str(), SQL_NTS);
	if (ret == SQL_SUCCESS) 
	{
		printf("\n Insert Complete");
		return true;
	}

	else
	{
		printf("\n InsertQry FAILED");
		return false;
	}
}


int DBProcess::SearchRecord(std::string id, std::string pw)
{
	int ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);

	std::string qry = "CALL SP_SearchUID('" + id + "', '" + pw + "', @ret);";
	std::wstring wQry;
	wQry.assign(qry.begin(), qry.end());
	wprintf(wQry.c_str());

	ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)wQry.c_str(), SQL_NTS);
	if (ret == SQL_SUCCESS)
	{
		ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)L"SELECT @ret", SQL_NTS);
		if (ret == SQL_SUCCESS)
		{
			ret = SQLFetch(m_hStmt);
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				SQLINTEGER uid;
				SQLLEN uidLen;
				SQLGetData(m_hStmt, 1, SQL_C_ULONG, &uid, 0, &uidLen);

				printf("Search Complete UID = %d", uid);
				return uid;
			}

			else if (ret == SQL_NO_DATA)
			{
				printf("None");
				return 0;
			}

			else
			{
				printf("Fetch Error");
				return -1;
			}
		}

		else
		{
			printf("SELECT @ret ERROR!");
			return -1;
		}
	}

	else
	{
		printf("SP_SearchUID ERROR!");
		return -1;
	}
}

int DBProcess::DeleteRecord(std::string id, std::string pw)
{
	int ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
	std::string qry = "CALL SP_DeleteUser('" + id + "', '" + pw + "', @ret);";
	std::wstring wQry;
	wQry.assign(qry.begin(), qry.end());
	wprintf(wQry.c_str());
	ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)wQry.c_str(), SQL_NTS);
	if (ret == SQL_SUCCESS)
	{
		ret = SQLExecDirect(m_hStmt, (SQLWCHAR*)L"SELECT @ret", SQL_NTS);
		if (ret == SQL_SUCCESS)
		{
			ret = SQLFetch(m_hStmt);
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				SQLINTEGER uid;
				SQLLEN uidLen;
				SQLGetData(m_hStmt, 1, SQL_C_ULONG, &uid, 0, &uidLen);

				printf("Delete Complete UID = %d", uid);
				return uid;
			}

			else
			{
				printf("Fetch Error");
				return -1;
			}

		}

		printf("Delete Complete");
		return 0;
	}

	else
	{
		printf("Deletet Qry FAILED");
		return -1;
	}
}

int DBProcess::GenUID()
{
	std::srand((unsigned int)time(NULL));
	int uid = std::rand() % 10000;
	return uid;
}

