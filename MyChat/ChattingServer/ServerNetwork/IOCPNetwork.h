#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <mutex>
#pragma comment(lib,"ws2_32.lib")

#include "Define.h"
#include "ServerNetworkErrorEnum.h"
#include "ServerLog.h"
#include "SessionManager.h"

namespace ServerNetwork
{
	class IOCPNetwork
	{
	public:
		IOCPNetwork();
		~IOCPNetwork();

		bool Init(const ServerConfig* pConfig, ServerLog* pLogger);
		void Run();
		void Release();

	protected:
		static UINT WINAPI AcceptThread(LPVOID param);
		static UINT WINAPI WorkerThread(LPVOID param);
		static UINT WINAPI SendThread(LPVOID param);

		void _WorkerThread();
		void _AcceptThread();
		void _SendThread();



	private:
		WSADATA		m_WSAData;
		SYSTEM_INFO m_SystemInfo;
		SOCKADDR_IN m_ServerAddress;
		SOCKET		m_ServerSocket;
		HANDLE		m_CompletionPort;

		std::mutex m_Mutex;
		std::deque<HANDLE> threadHandleDeque;
		//ServerLog* m_pRefLogger;

		SessionManager*	m_SessionManager;

		std::deque<RecvPacketInfo> RecvPacketQueue;
		std::deque<RecvPacketInfo> SendPacketQueue;

		

	};
}


namespace ServerNetwork
{
	class TcpNetwork
	{
	public:
		TcpNetwork();
		~TcpNetwork();

		NETWORK_ERROR_CODE Init(const ServerConfig* pConfig, ServerLog* pLogger);
		NETWORK_ERROR_CODE SendData(const int sessionIndex, const short packetId, const short size, const char* pMsg);
		void Run();
		RecvPacketInfo GetPacketInfo();
		void Release();

		void ForcingClose(const int sessionIndex);

		int GetClientSessionPoolSize() { return (int)m_ClientSessionPool.size(); }



		//friend class ConnectedUserManager;

	protected:
		NETWORK_ERROR_CODE InitServerSocket();
		NETWORK_ERROR_CODE BindListen(short port, int backlogCount);

		int AllocClientSessionIndex();
		void ReleaseSessionIndex(const int index);

		void CreateSessionPool(const int maxClientCount);
		NETWORK_ERROR_CODE NewSession();
		void SetSockOption(const SOCKET fd);
		void ConnectedSession(const int sessionIndex, const int fd, const char* pIP);

		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);

		NETWORK_ERROR_CODE RecvSocket(const int sessionIndex);
		NETWORK_ERROR_CODE RecvBufferProcess(const int sessionIndex);
		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);

		void RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set);
		NetworkError FlushSendBuff(const int sessionIndex);
		NetworkError SendSocket(const SOCKET fd, const char* pMsg, const int size);

		bool RunCheckSelectResult(const int result);
		void RunCheckSelectClients(fd_set& exc_set, fd_set& read_set, fd_set& write_set);
		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);

	protected:
		ServerConfig m_Config;

		SOCKET m_ServerSockfd;

		fd_set m_Readfds;
		size_t m_ConnectedSessionCount = 0;

		int64_t m_ConnectSeq = 0;

		std::vector<ClientSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;

		std::deque<RecvPacketInfo> m_PacketQueue;

		ServerLog* m_pRefLogger;
	};
}
