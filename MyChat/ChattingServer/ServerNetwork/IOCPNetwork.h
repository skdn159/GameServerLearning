#pragma once
#include <winsock2.h>
#include <mutex>
#include <deque>
#include <process.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")
#include "Define.h"
#include "ServerNetworkErrorEnum.h"
#include "ServerLog.h"


namespace ServerNetwork
{
	class IOCPNetwork
	{
	public:
		IOCPNetwork();
		~IOCPNetwork();


		bool InitServer();
		void Run();
		bool Release();


	private:
		
		static UINT WINAPI AcceptThread(LPVOID param);
		static UINT WINAPI RecvThread(LPVOID param);
		static UINT WINAPI SendThread(LPVOID param);

		void AcceptThreadFunc();
		void RecvThreadFunc();
		void SendThreadFunc();

	private:
		bool m_IsStarted = false;

		std::mutex m_Mutex;
		WSADATA		m_wsaData;
		SYSTEM_INFO m_SystemInfo;
		SOCKADDR_IN m_ServerAddress;

		SOCKET		m_hServerSocket;
		HANDLE		m_hCompletionPort;

		std::deque<HANDLE> m_threadHandleDeque;

	};




}
// 	class TcpNetwork 
// 	{
// 	public:
// 		TcpNetwork();
// 		 ~TcpNetwork();
// 
// 		NETWORK_ERROR_CODE Init(const ServerConfig* pConfig, ServerLog* pLogger);
// 		NETWORK_ERROR_CODE SendData(const int sessionIndex, const short packetId, const short size, const char* pMsg);
// 		void Run();
// 		RecvPacketInfo GetPacketInfo();
// 		void Release();
// 
// 		void ForcingClose(const int sessionIndex); 
// 
// 		int GetClientSessionPoolSize() { return (int)m_ClientSessionPool.size(); }
// 		
// 		
// 		
// 		//friend class ConnectedUserManager;
// 
// 	protected:
// 		NETWORK_ERROR_CODE InitServerSocket();
// 		NETWORK_ERROR_CODE BindListen(short port, int backlogCount);
// 
// 		int AllocClientSessionIndex();
// 		void ReleaseSessionIndex(const int index);
// 
// 		void CreateSessionPool(const int maxClientCount);
// 		NETWORK_ERROR_CODE NewSession();
// 		void SetSockOption(const SOCKET fd);
// 		void ConnectedSession(const int sessionIndex, const int fd, const char* pIP);
// 
// 		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
// 
// 		NETWORK_ERROR_CODE RecvSocket(const int sessionIndex);
// 		NETWORK_ERROR_CODE RecvBufferProcess(const int sessionIndex);
// 		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);
// 
// 		void RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set);
// 		NetworkError FlushSendBuff(const int sessionIndex);
// 		NetworkError SendSocket(const SOCKET fd, const char* pMsg, const int size);
// 
// 		bool RunCheckSelectResult(const int result);
// 		void RunCheckSelectClients(fd_set& exc_set, fd_set& read_set, fd_set& write_set);
// 		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);
// 
// 	protected:
// 		ServerConfig m_Config;
// 
// 		SOCKET m_ServerSockfd;
// 
// 		fd_set m_Readfds;
// 		size_t m_ConnectedSessionCount = 0;
// 
// 		int64_t m_ConnectSeq = 0;
// 
// 		std::vector<ClientSession> m_ClientSessionPool;
// 		std::deque<int> m_ClientSessionPoolIndex;
// 
// 		std::deque<RecvPacketInfo> m_PacketQueue;
// 
// 		ServerLog* m_pRefLogger;
// 	};
// }
