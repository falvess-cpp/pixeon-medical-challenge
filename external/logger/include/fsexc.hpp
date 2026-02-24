#ifndef FSEXCEPTION_HPP
#define FSEXCEPTION_HPP

#include <string>
#include <list>
#include <iostream>
#include <exception>

	
#define FUNC_TRY() try {

#define FUNC_CATCH() \
	} \
	catch (FsException &ex) { \
		ex.addToCallStack(FsExceptionLocation(__FILE__, __FUNCTION__, __LINE__)); \
		std::cerr << ex.getErrorId() << " - " << ex.getErrorMessage() << std::endl;\
		ex.printCallStack(); \
		throw ex; \
	} \
	catch (const exception& ex) \
	{ \
		std::cerr << "### Exception Ocurred ###" << std::endl; \
		std::cerr << "Error: " << ex.what() << std::endl; \
		throw ex; \
	} \
	catch (...) { \
		std::cerr << "Unknow Error - " << std::endl; \
		throw; \
	} \
	
class FsExceptionLocation
{
private:
	std::string mFileName;
	std::string mMethodName;
	unsigned long mLineNumber;
	
public:
	FsExceptionLocation(const std::string& fileName, const std::string& methodName, unsigned long lineNumber);	
	FsExceptionLocation(const FsExceptionLocation& location);
	FsExceptionLocation();		
	~FsExceptionLocation() = default;	
	FsExceptionLocation& operator=(const FsExceptionLocation& location);	
	
	const std::string& getFileName() const;	
	const std::string& getMethodName() const;	
	unsigned long getLineNumber() const;	
	std::string getLocation() const;	
};

class FsException : public std::exception
{
private:
	unsigned long mErrorId;
	std::string mErrorMsg;	
	std::list<FsExceptionLocation> mCallStack;
	std::list<std::string> strCallStack;
	
public:

	FsException(unsigned long errorId, const std::string& errorMsg);
	FsException(const FsException& exception);
	FsException();	
	~FsException() = default;
		
	FsException& operator=(const FsException& fsException);
	
	unsigned long getErrorId() const;	
	const std::string& getErrorMessage() const;	
	
    void printCallStack() const;
    std::string getCallStack() const;
    const std::list<FsExceptionLocation>*  getCallStackList() const;	
	
    void addToCallStack(const FsExceptionLocation& location);		
	std::string buildError() const;
};

#endif // FSEXCEPTION_HPP

