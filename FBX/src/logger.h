#pragma once

#include <log.h>
#include <fstream>

#define FILE_LOG		"./log/debug.log"

#define LOG_W			Logger::proxy
#define LOG_ISWRITE		Logger::isWrite

#ifdef LOG_W_DISABLE
#define LOG_WRITE_ENABLE	false
#else
#define LOG_WRITE_ENABLE	true
#endif

class Logger
{
public:
	void open();
	void close();

	Logger& operator=(Logger& other) = delete;
	Logger& operator()(void);

	template <class T>
	friend Logger& operator<<(Logger &logger, const T& code);

	static Logger proxy;
	static bool isWrite;
private:
	std::ofstream os;
	std::string codes;
};

template<class T>
inline Logger &operator<<(Logger &logger, const T& code)
{
	// TODO: insert return statement here
	if (!LOG_WRITE_ENABLE) {
		return logger;
	}
	logger.open();
	if (logger.os.is_open()) {
		logger.os << code;
	}
	logger.close();
	return logger;
}
