#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>

#include "../../../Common/ErrorCode.h"
#include "../../../Common/PacketID.h"
#include "../../../Common/Packet.h"


const int MAX_PACKET_SIZE = 1024;
const int MAX_SOCK_RECV_BUFFER = 8016;

#pragma pack(push, 1)
struct PacketHeader
{
	short Id;
	short BodySize;
};
#pragma pack(pop)

const int PACKET_HEADER_SIZE = sizeof(PacketHeader);

struct RecvPacketInfo
{
	RecvPacketInfo() {}

	short PacketId = 0;
	short PacketBodySize = 0;
	char* pData = nullptr;
};

class TcpNetwork
{
public: 
	TcpNetwork();
	~TcpNetwork();

	bool ConnectTo(const char* hostIP, int port);
	bool IsConnected() { return m_IsConnected; }
	void DisConnect();
	void SendPacket(const short packetId, const short dataSize, char* pData);
	void Update();
	RecvPacketInfo GetPacket();

private:
	void NonBlock(SOCKET s);
	void RecvData();
	void RecvBufferProcess();
	void AddPacketQueue(const short pktId, const short bodySize, char* pDataPos);
	void Clear();

private:
	bool m_IsConnected = false;
	
	std::thread m_Thread;
	std::mutex m_mutex;
	
	SOCKET m_sock;
	
	int m_recvSize = 0;
	char m_RecvBuffer[MAX_SOCK_RECV_BUFFER] = { 0, };
	
	std::deque<RecvPacketInfo> m_PacketQueue;
};
