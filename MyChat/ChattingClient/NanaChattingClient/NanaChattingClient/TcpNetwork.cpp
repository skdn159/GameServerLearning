#include "TcpNetwork.h"



TcpNetwork::TcpNetwork()
{
}


TcpNetwork::~TcpNetwork()
{
}

bool TcpNetwork::ConnectTo(const char* hostIP, int port)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET) {
		return false;
	}

	SOCKADDR_IN s_addr_in;
	ZeroMemory(&s_addr_in, sizeof(s_addr_in));
	s_addr_in.sin_family = AF_INET;
	s_addr_in.sin_port = htons(port);
	inet_pton(AF_INET, hostIP, (void *)&s_addr_in.sin_addr.s_addr);

	//connect
	if (connect(m_sock, (SOCKADDR*)&s_addr_in, sizeof(s_addr_in)) != 0) {
		return false;
	}
	//ClientLog("Connetct Server Success!\n");

	m_IsConnected = true;

	//socket Nonblocking
	NonBlock(m_sock);

	m_Thread = std::thread([&]() { Update(); });

	return true;
}

void TcpNetwork::DisConnect()
{
	if (m_IsConnected)
	{
		closesocket(m_sock);

		Clear();
	}

	if (m_Thread.joinable()) {
		m_Thread.join();
	}
}

void TcpNetwork::SendPacket(const short packetId, const short dataSize, char* pData)
{
	char data[MAX_PACKET_SIZE] = { 0, };

	PacketHeader pktHeader{ packetId, dataSize };
	memcpy(&data[0], (char*)&pktHeader, PACKET_HEADER_SIZE);

	if (dataSize > 0) {
		memcpy(&data[PACKET_HEADER_SIZE], pData, dataSize);
	}

	send(m_sock, data, dataSize + PACKET_HEADER_SIZE, 0);
}

void TcpNetwork::Update()
{
	while (m_IsConnected)
	{
		//log("running...");
		RecvData();
		RecvBufferProcess();
	}
}

RecvPacketInfo TcpNetwork::GetPacket()
{
	std::lock_guard<std::mutex> guard(m_mutex);

	if (m_PacketQueue.empty()) {
		return RecvPacketInfo();
	}

	auto packet = m_PacketQueue.front();
	m_PacketQueue.pop_front();
	return packet;
}

void TcpNetwork::NonBlock(SOCKET s)
{
	u_long u10n = 1L;
	ioctlsocket(s, FIONBIO, (unsigned long*)&u10n);
}

void TcpNetwork::RecvData()
{
	fd_set read_set;
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;

	FD_ZERO(&read_set);
	FD_SET(m_sock, &read_set);

	if (select(m_sock + 1, &read_set, NULL, NULL, &tv) < 0) {
		return;
	}

	if (FD_ISSET(m_sock, &read_set))
	{
		char recvBuff[MAX_PACKET_SIZE];

		auto recvSize = recv(m_sock, recvBuff, MAX_PACKET_SIZE, 0);

		if (recvSize == 0)
		{
			//ClientLog("RecvData Error!Connection Failed...");
			return;
		}

		if (recvSize < 0)
		{
			// NonBlock 
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				//ClientLog("WSAGetLastError()!=WSAEWOULDBLOCK Error!");
				return;
			}
		}

		//Buffer Overflow
		if ((m_recvSize + recvSize) >= MAX_SOCK_RECV_BUFFER)
		{
			//ClientLog("Buffer Overflow Error!");
			return;
		}

		memcpy(&m_RecvBuffer[m_recvSize], recvBuff, recvSize);
		m_recvSize += recvSize;
	}
}

void TcpNetwork::RecvBufferProcess()
{
	auto readPos = 0;
	const auto dataSize = m_recvSize;
	PacketHeader* pPktHeader;

	while ((dataSize - readPos) > PACKET_HEADER_SIZE)
	{
		pPktHeader = (PacketHeader*)&m_RecvBuffer[readPos];
		readPos += PACKET_HEADER_SIZE;

		if (pPktHeader->BodySize > (dataSize - readPos))
		{
			break;
		}

		if (pPktHeader->BodySize > MAX_PACKET_SIZE)
		{
			return;// NET_ERROR_CODE::RECV_CLIENT_MAX_PACKET;
		}

		AddPacketQueue(pPktHeader->Id, pPktHeader->BodySize, &m_RecvBuffer[readPos]);

		readPos += pPktHeader->BodySize;
	}

	m_recvSize -= readPos;

	if (m_recvSize > 0)
	{
		memcpy(m_RecvBuffer, &m_RecvBuffer[readPos], m_recvSize);
	}
}

void TcpNetwork::AddPacketQueue(const short pktId, const short bodySize, char* pDataPos)
{
	RecvPacketInfo packetInfo;
	packetInfo.PacketId = pktId;
	packetInfo.PacketBodySize = bodySize;
	packetInfo.pData = new char[bodySize];
	memcpy(packetInfo.pData, pDataPos, bodySize);

	std::lock_guard<std::mutex> guard(m_mutex);
	m_PacketQueue.push_back(packetInfo);
}

void TcpNetwork::Clear()
{
	m_IsConnected = false;
	m_recvSize = 0;
	m_PacketQueue.clear();
}
