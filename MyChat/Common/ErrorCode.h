#pragma once

namespace Common
{
	enum class ERROR_CODE : short
	{
		NONE = 0,

		UNASSIGNED_ERROR = 201,

		MAIN_INIT_NETWORK_INIT_FAIL = 206,

		USER_MGR_ID_DUPLICATION = 211,
		USER_MGR_MAX_USER_COUNT = 212,
		USER_MGR_INVALID_SESSION_INDEX = 213,
		USER_MGR_NOT_CONFIRM_USER = 214,
		USER_MGR_REMOVE_INVALID_SESSION = 221,

		LOBBY_LIST_INVALID_DOMAIN = 226,

		LOBBY_ENTER_INVALID_DOMAIN = 231,
		LOBBY_ENTER_INVALID_LOBBY_INDEX = 232,
		LOBBY_ENTER_USER_DUPLICATION = 234,
		LOBBY_ENTER_MAX_USER_COUNT = 235,
		LOBBY_ENTER_EMPTY_USER_LIST = 236,

		LOBBY_ROOM_LIST_INVALID_START_ROOM_INDEX = 241,
		LOBBY_ROOM_LIST_INVALID_DOMAIN = 242,
		LOBBY_ROOM_LIST_INVALID_LOBBY_INDEX = 243,

		LOBBY_USER_LIST_INVALID_DOMAIN = 251, 
		LOBBY_USER_LIST_INVALID_LOBBY_INDEX = 252, 
		LOBBY_USER_LIST_INVALID_START_USER_INDEX = 253, 

		LOBBY_LEAVE_INVALID_DOMAIN = 261,
		LOBBY_LEAVE_INVALID_LOBBY_INDEX = 262,
		LOBBY_LEAVE_USER_NVALID_UNIQUEINDEX = 263,

		LOBBY_CHAT_INVALID_DOMAIN = 264,
		LOBBY_CHAT_INVALID_LOBBY_INDEX = 265,

		WHISPER_INVALID_DOMAIN = 266,
		WHISPER_INVALID_LOBBY_INDEX = 267,


		ROOM_ENTER_INVALID_DOMAIN = 271,
		ROOM_ENTER_INVALID_LOBBY_INDEX = 272,
		ROOM_ENTER_INVALID_ROOM_INDEX = 273,
		ROOM_ENTER_CREATE_FAIL = 274,
		ROOM_ENTER_NOT_CREATED = 275,
		ROOM_ENTER_MEMBER_FULL = 276,
		ROOM_ENTER_EMPTY_ROOM = 277,

		ROOM_LEAVE_INVALID_DOMAIN = 286,
		ROOM_LEAVE_INVALID_LOBBY_INDEX = 287,
		ROOM_LEAVE_INVALID_ROOM_INDEX = 288,
		ROOM_LEAVE_NOT_CREATED = 289,
		ROOM_LEAVE_NOT_MEMBER = 290,

		ROOM_CHAT_INVALID_DOMAIN = 296,	
		ROOM_CHAT_INVALID_LOBBY_INDEX = 297,
		ROOM_CHAT_INVALID_ROOM_INDEX = 298,





	};
}