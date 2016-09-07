#include "IOCPNetwork.h"
#include <process.h>

namespace ServerNetwork
{
	IOCPNetwork::IOCPNetwork()
	{
	}

	IOCPNetwork::~IOCPNetwork()
	{
	}

	bool IOCPNetwork::InitServer()
	{
		if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
			return false;
		}

		m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		ZeroMemory(&m_ServerAddress, sizeof(m_ServerAddress));

		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		m_ServerAddress.sin_port = htons(atoi("8777"));

		bind(m_hServerSocket, (SOCKADDR*)&m_ServerAddress, sizeof(m_ServerAddress));
		listen(m_hServerSocket, SOMAXCONN);



		m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		GetSystemInfo(&m_SystemInfo);

		for (int i = 0; i < m_SystemInfo.dwNumberOfProcessors * 2; ++i)
		{
			auto recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, (LPVOID)this, 0, NULL);
			m_threadHandleDeque.push_back(recvThread);
		}
		auto acceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, NULL);
		auto sendThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, NULL);



		printf("Server Start...\n");
		return true;
	}

	void IOCPNetwork::Run()
	{





		m_IsStarted = true;
	}

	bool IOCPNetwork::Release()
	{

	}

	UINT WINAPI IOCPNetwork::AcceptThread(LPVOID param)
	{
		IOCPNetwork *pThread = (IOCPNetwork *)param;
		if (pThread == NULL) {
			return 0;
		}

		pThread->AcceptThreadFunc();
		return 0;
	}

	UINT WINAPI IOCPNetwork::RecvThread(LPVOID param)
	{
		IOCPNetwork *pThread = (IOCPNetwork *)param;
		if (pThread == NULL) {
			return 0;
		}

		pThread->RecvThreadFunc();
		return 0;
	}

	UINT WINAPI IOCPNetwork::SendThread(LPVOID param)
	{
		IOCPNetwork *pThread = (IOCPNetwork *)param;
		if (pThread == NULL) {
			return 0;
		}

		pThread->SendThreadFunc();
		return 0;
	}

	void IOCPNetwork::AcceptThreadFunc()
	{
		while (true)
		{
			SOCKET hClntSock;
			SOCKADDR_IN clientAddr;
			int addrLen = sizeof(clientAddr);

			// blocking
			hClntSock = accept(m_hServerSocket, (SOCKADDR*)&clientAddr, &addrLen);

			if (hClntSock == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					continue;
				}
				
			}
		}

		return;
	}

	void IOCPNetwork::RecvThreadFunc()
	{

	}

	void IOCPNetwork::SendThreadFunc()
	{

	}

}