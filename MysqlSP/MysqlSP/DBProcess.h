#pragma once
#include "DBConnector.h"


class DBProcess
{
public:
	DBProcess();
	~DBProcess();


	void SetHandler(SQLHENV sqlhenv, SQLHDBC sqlhdbc, SQLHSTMT sqlhstmt);
	
	bool InsertRecord(std::string id, std::string pw);
	int SearchRecord(std::string id, std::string pw);
	int DeleteRecord(std::string id, std::string pw);

	int GenUID();

private:
	SQLHENV m_hEnv;
	SQLHDBC m_hDbc;
	SQLHSTMT m_hStmt;

};

