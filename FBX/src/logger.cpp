#include <logger.h>

Logger Logger::proxy;
bool Logger::isWrite = false;

void Logger::open()
{
	os.open(FILE_LOG, std::ios::out | std::ios::app);
}

void Logger::close()
{
	os.close();
}

Logger& Logger::operator()(void)
{
	return *this;
}

