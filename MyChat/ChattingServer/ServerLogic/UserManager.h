#pragma once
#include <unordered_map>
#include <deque>
#include <string>

namespace Common
{
	enum class ERROR_CODE : short;
}
using ERROR_CODE = Common::ERROR_CODE;

namespace ServerLogic
{
	class User;

	class UserManager
	{
	public:
		UserManager();
		virtual ~UserManager();

		void Init(const int maxUserCount);

		ERROR_CODE AddUser(const int sessionIndex, const char* pszID);
		ERROR_CODE RemoveUser(const int sessionIndex);

		std::tuple<ERROR_CODE, User*> GetUser(const int sessionIndex);
		std::tuple<ERROR_CODE, User*> GetUser(const char* pszID);


		User* FindUser(const int sessionIndex);
		//User* FindUser(std::string pszID);
		User* FindUser(const char* pszID);


	private:
		User* AllocUserObjPoolIndex();
		void ReleaseUserObjPoolIndex(const int index);


	private:
		std::vector<User> m_UserObjPool;
		std::deque<int> m_UserObjPoolIndex;

		std::unordered_map<int, User*> m_UserSessionDic;
		std::unordered_map<const char*, User*> m_UserIDDic; //char*�� key�� ������
	};
}

