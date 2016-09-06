#include "IOCPNetwork.h"

namespace ServerNetwork
{

	IOCPNetwork::IOCPNetwork()
	{
	}


	IOCPNetwork::~IOCPNetwork()
	{
	}

	bool IOCPNetwork::Init(const ServerConfig* pConfig, ServerLog* pLogger)
	{
		if (WSAStartup(MAKEWORD(2, 2), &m_WSAData) != 0) {
			return false;
		}

		m_ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		ZeroMemory(&m_ServerAddress, sizeof(m_ServerAddress));

		m_ServerAddress.sin_family = AF_INET;
		m_ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		m_ServerAddress.sin_port = htons(atoi("8777"));

		bind(m_ServerSocket, (SOCKADDR*)&m_ServerAddress, sizeof(m_ServerAddress));
		listen(m_ServerSocket, SOMAXCONN);
		
		m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		GetSystemInfo(&m_SystemInfo);

		for (int i = 0; i < m_SystemInfo.dwNumberOfProcessors; ++i)
		{
			auto hThread = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, (LPVOID)this, 0, NULL);
			threadHandleDeque.push_back(hThread);
		}

		auto hAccept = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, (LPVOID)this, 0, NULL);
		auto hSend = (HANDLE)_beginthreadex(NULL, 0, SendThread, (LPVOID)this, 0, NULL);

		threadHandleDeque.push_back(hAccept);
		threadHandleDeque.push_back(hSend);

		m_SessionManager = new SessionManager();
		m_SessionManager->InitSessionPool();

		std::cout << "Server Start..." << std::endl;
		return true;
	}

	void IOCPNetwork::Run()
	{

	}

	void IOCPNetwork::Release()
	{
		delete m_SessionManager;

		while (threadHandleDeque.empty() == false)
		{
			auto hThread = threadHandleDeque.front();
			CloseHandle(hThread);
			threadHandleDeque.pop_front();
		}
	}

	UINT WINAPI IOCPNetwork::AcceptThread(LPVOID param)
	{
		IOCPNetwork* pThread = (IOCPNetwork*)param;
		if (pThread == NULL)
		{
			return 0;
		}

		pThread->_AcceptThread();
		return 0;
	}

	UINT WINAPI IOCPNetwork::WorkerThread(LPVOID param)
	{
		IOCPNetwork* pThread = (IOCPNetwork*)param;
		if (pThread == NULL)
		{
			return 0;
		}

		pThread->_WorkerThread();
		return 0;
	}

	UINT WINAPI IOCPNetwork::SendThread(LPVOID param)
	{
		IOCPNetwork* pThread = (IOCPNetwork*)param;
		if (pThread == NULL)
		{
			return 0;
		}

		pThread->_SendThread();
		return 0;
	}

	void IOCPNetwork::_WorkerThread()
	{
		Session* pSession;
		Context* pContext;

		while (true)
		{
			DWORD bytes = 0;

			auto result = GetQueuedCompletionStatus(m_CompletionPort, &bytes,
				(PULONG_PTR)&pSession, (LPOVERLAPPED*)&pContext, INFINITE);

			if (result == 0 || bytes <= 0)
			{
				m_SessionManager->CloseSession(pSession);
				continue;
			}

			if (pContext == pSession->m_RecvContext)
			{
				char* tmp = new char[bytes]();

				if (pSession->recvComplete == true)
					pSession->m_tmpLen = 0;

				std::copy(
					&pSession->m_RecvContext->wsaBuffer.buf[0],
					&pSession->m_RecvContext->wsaBuffer.buf[bytes],
					&pSession->m_tmpRecvBuf[pSession->m_tmpLen]
				);

				pSession->m_tmpLen += bytes;
				

				DWORD flags = 0;
				WSARecv(pSession->m_ClientSock, &(pSession->m_RecvContext->wsaBuffer), 1,
					NULL, &flags, &(pSession->m_RecvContext->overlapped), NULL);
			}

			
		}

		return;
	}

	void IOCPNetwork::_AcceptThread()
	{
		while (true)
		{
			SOCKET hClntSock;
			SOCKADDR_IN clientAddress;
			int addrLen = sizeof(clientAddress);

			hClntSock = accept(m_ServerSocket, (SOCKADDR*)&clientAddress, &addrLen);

			if (hClntSock == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				else
				{
					// TODO: ODBC 사용시 지속적으로 accept 함수 호출하는 현상 반드시 해결해야 함
					/*if (clientAddress.sin_family == 52428)
					continue;
					*/
					std::cout << "[ Accept ERROR ] 연결 실패 : " << inet_ntoa(clientAddress.sin_addr) << std::endl;
					continue;
				}
			}

			auto pSession = m_SessionManager->GetEmptySession();
			m_SessionManager->AllocClientToSession(pSession, hClntSock, clientAddress);

			// TODO: pSession 확인
			CreateIoCompletionPort((HANDLE)hClntSock, m_CompletionPort, (ULONG_PTR)pSession, 0);

			std::cout << "[ Accept ] : " << inet_ntoa(clientAddress.sin_addr) << " " << pSession << std::endl;

			DWORD recvBytes = 0;
			DWORD flags = 0;
			// WSARecv 함수를 호출하여 해당 소켓에 일을 시키자
			WSARecv(hClntSock, &(pSession->m_RecvContext->wsaBuffer), 1,
				&recvBytes, &flags, &(pSession->m_RecvContext->overlapped), NULL);
		}

		return;
	}

	void IOCPNetwork::_SendThread()
	{
		while (true)
		{
		
		}

		return;
	}

}