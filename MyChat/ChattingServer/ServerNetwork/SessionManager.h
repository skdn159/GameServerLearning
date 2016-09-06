#pragma once
#include <WinSock2.h>
#include <vector>
#include <mutex>

const int MAX_BUFFER_SIZE = 1024;
const int MAX_CLIENT_COUNT = 500;

namespace ServerNetwork
{
	class Context
	{
	public:
		Context() = default;
		~Context() { delete buffer; }

		void Clear();

		OVERLAPPED	overlapped;
		char		buffer[MAX_BUFFER_SIZE] = { 0 };
		WSABUF		wsaBuffer;
	};

	class Session
	{
	public:
		Session() = default;
		~Session() { delete m_RecvContext; delete m_SendContext; }

		const bool IsUsed() const { return isUsed; }
		void InitSession(SOCKET clientSock, SOCKADDR_IN clientAddress);
		void ClearSession();

	public:
		SOCKET	m_ClientSock = INVALID_SOCKET;
		SOCKADDR_IN m_ClientAddress;
		Context* m_RecvContext = nullptr;
		Context* m_SendContext = nullptr;

		unsigned int recvPos = 0;

		char m_tmpRecvBuf[MAX_BUFFER_SIZE];
		int	 m_tmpLen;
		bool recvComplete = false;

	private:
		bool isUsed = false;
	};




	class SessionManager
	{
	public:
		SessionManager();
		~SessionManager();

		bool InitSessionPool();
		Session* GetEmptySession();
		bool AllocClientToSession(Session* pSession, SOCKET clientSocket, SOCKADDR_IN clientAddress);
		void CloseSession(Session* pSession);

	private:
		std::mutex m_Mutex;
		std::vector<Session> m_SessionPool;
	};

}