#pragma once
#include "Windows.h"
#include "iostream"
using namespace std;

#include "sql.h"
#include "sqlext.h"
#include <tchar.h>
#include <sqltypes.h>

int _tmain(int argc, _TCHAR* argv[]) {

	int ret;
	SQLHENV hEnv;
	SQLHDBC hDbc;
	SQLHSTMT hStmt;

	//SQL과 연결
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION,
		(SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	ret = SQLConnect(hDbc, (SQLWCHAR*)"MYODBC", SQL_NTS,
		(SQLWCHAR*)"root", SQL_NTS,
		(SQLWCHAR*)"1q2w3e4r", SQL_NTS);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		printf("SQL Connect Ok \n");
	else { // display error code when we get error… SQLGetDiagRec 참고
		printf("sqlconnect error .\n"); return(1);
	}
	// 명령핸들
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	ret = SQLExecDirect(hStmt, (SQLWCHAR*)"SELECT * FROM Users", SQL_NTS);
		if (ret == SQL_SUCCESS) {
			int iCount = 0;

			//SQLLEN iAuthorLen, iTitleLen, iPriceLen, iPrice;
			SQLLEN UID, UIDLen, UserIDLen, PassWordLen;
			char UserID[100], PassWord[100];

			while (TRUE) {
				ret = SQLFetch(hStmt); // 값을 얻어올때
				if (ret == SQL_ERROR || ret == SQL_SUCCESS_WITH_INFO)
					printf("An error occured\n");

				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
					SQLGetData(hStmt, 1, SQL_C_WCHAR, &UID, 0, &UIDLen);
					SQLGetData(hStmt, 2, SQL_C_WCHAR, UserID, 100, &UserIDLen);
					SQLGetData(hStmt, 3, SQL_C_ULONG, &PassWord, 100, &PassWordLen);
					/* Print the row of data */
					_tprintf(_T(" Row %d : "), iCount++);
					_tprintf(_T(" %ls %ls %d \n"), UID, UserID, PassWord);
				}
			}
		}
		else
			printf("An error occured during excuting query!! \n");
	//접속 종료 및 반환
	if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	if (hDbc) SQLDisconnect(hDbc);
	if (hDbc) SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return 0;
}