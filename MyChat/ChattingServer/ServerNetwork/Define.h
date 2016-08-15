#pragma once

namespace ServerNetwork
{
	struct ServerConfig
	{
		unsigned short port;
		int backLogCount;

		int maxClientCount;
		int extraClientCount; // �����ϸ� �α��ο��� ¥������ MaxClientCount + �������� �غ��Ѵ�.

		short maxClientSockOptRecvBufferSize;
		short maxClientSockOptSendBufferSize;
		short maxClientRecvBufferSize;
		short maxClientSendBufferSize;

		int maxLobbyCount;
		int maxLobbyUserCount;
		int maxRoomCountByLobby;
		int maxRoomUserCount;
	};

	const int MAX_IP_LEN = 32; // IP ���ڿ� �ִ� ����
	const int MAX_PACKET_SIZE = 1024; // �ִ� ��Ŷ ũ��

	//const int MAX_SOCK_OPT_RECV_BUUER_SIZE = MAX_PACKET_SIZE * 10; // ���� �ɼǿ� �ޱ� ���� ũ��
	//const int MAX_SOCK_OPT_SEND_BUUER_SIZE = MAX_PACKET_SIZE * 10; // ���� �ɼǿ� ������ ���� ũ��
	//const int MAX_CLIENT_RECV_BUFFER_SIZE = MAX_PACKET_SIZE * 8; // Ŭ���̾�Ʈ �ޱ�� �ִ� ���� ũ��
	//const int MAX_CLIENT_SEND_BUFFER_SIZE = MAX_PACKET_SIZE * 8; // 4k. Ŭ���̾�Ʈ ������� �ִ� ���� ũ��

	struct ClientSession
	{
		bool IsConnected() { return SocketFD > 0 ? true : false; }

		void Clear()
		{
			Seq = 0;
			SocketFD = 0;
			IP[0] = '\0';
			RemainingDataSize = 0;
			SendSize = 0;
		}

		int Index = 0;
		long long Seq = 0;
		int		SocketFD = 0;
		char    IP[MAX_IP_LEN] = { 0, };

		char*   pRecvBuffer = nullptr;
		int     RemainingDataSize = 0;

		char*   pSendBuffer = nullptr;
		int     SendSize = 0;
	};

	struct RecvPacketInfo
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};
	struct RecvPacketInfo2
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};


	enum class SOCKET_CLOSE_CASE : short 
	{
		SESSION_POOL_EMPTY = 1,
		SELECT_ERROR = 2,
		SOCKET_RECV_ERROR = 3,
		SOCKET_RECV_BUFFER_PROCESS_ERROR = 4,
		SOCKET_SEND_ERROR = 5,
		FORCING_CLOSE = 6,

	};


	enum class PACKET_ID : short // ���� ������ socket�Ǵ��ؼ� PacketQue�� �־ ó���ϵ��� ����
	{
		NTF_SYS_CLOSE_SESSION = 3,

	};

#pragma pack(push, 1)
	struct PacketHeader // ���������� packet header size�� ������� ���
	{
		short Id;
		short BodySize;
	};

	const int PACKET_HEADER_SIZE = sizeof(PacketHeader);


	struct PktNtfSysCloseSession : PacketHeader
	{
		int SockFD;
	};
#pragma pack(pop)
}