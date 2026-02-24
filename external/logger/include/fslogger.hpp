#ifndef FS_LOGGER_HPP
#define FS_LOGGER_HPP

#include <string>

enum class LogType {
    Error = 0,
    Warn,
    Inform,
    Trace
};

class FsLogger
{
public:
	FsLogger() = delete;
	~FsLogger() = default;
	
    static void trace(const std::string& fileName, unsigned long lineNumber, const std::string& msg);
	static void info(const std::string& fileName, unsigned long lineNumber, const std::string& msg);
	static void warning(const std::string& fileName, unsigned long lineNumber, const std::string& msg);
	static void error(const std::string& fileName, unsigned long lineNumber, const std::string& msg);
	static void funcExit(const std::string& function, const std::string& fileName, unsigned long lineNumber);
	static void funcEntry(const std::string& prettyFunction, const std::string& fileName, unsigned long lineNumber);
	static bool isTraceEnabled();
	
private:
	static bool isLogTrace;
	static bool getConfigTrace();
	static void log(LogType logType, const std::string& fileName, unsigned long lineNumber, const std::string& msg);
	static void logFuncEntry(const std::string& prettyFunction, const std::string& fileName, unsigned long lineNumber);
	static void logFuncExit(const std::string& function, const std::string& fileName, unsigned long lineNumber);
	static std::string getLogType(LogType logType);	
};

#endif // FS_LOGGER_HPP 

