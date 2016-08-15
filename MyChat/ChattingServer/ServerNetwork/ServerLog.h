#pragma once

#include <stdio.h>
#include <stdarg.h>

#pragma warning (disable: 4100)

namespace ServerNetwork
{
	const int MAX_LOG_STRING_LENGTH = 256;

	enum class LOG_TYPE : short
	{
		LOG_TRACE = 1
		, LOG_DEBUG = 2
		, LOG_WARN = 3
		, LOG_ERROR = 4
		, LOG_INFO = 5,
	};

	class ServerLog
	{
	public:
		ServerLog() {}
		virtual ~ServerLog() {}

		virtual void Write(const LOG_TYPE nType, const char* pFormat, ...)
		{
			char szText[MAX_LOG_STRING_LENGTH];

			va_list args;
			va_start(args, pFormat);
			vsprintf_s(szText, MAX_LOG_STRING_LENGTH, pFormat, args);
			va_end(args);

			switch (nType)
			{
			case LOG_TYPE::LOG_INFO:
				Info(szText);
				break;
			case LOG_TYPE::LOG_ERROR:
				Error(szText);
				break;
			case LOG_TYPE::LOG_WARN:
				Warn(szText);
				break;
			case LOG_TYPE::LOG_DEBUG:
				Debug(szText);
				break;
			case LOG_TYPE::LOG_TRACE:
				Info(szText);
				break;
			default:
				break;
			}
		}

	protected:
		virtual void Error(const char* pText) = 0;
		virtual void Warn(const char* pText) = 0;
		virtual void Debug(const char* pText) = 0;
		virtual void Trace(const char* pText) = 0;
		virtual void Info(const char* pText) = 0;

	};
}