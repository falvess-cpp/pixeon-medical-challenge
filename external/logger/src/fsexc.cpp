/********************************************************************  
	@file       AppException.cpp
	@author     Fabiano Souza
	@brief      Implementation of AppException class.
	@history    1.0      Initial version 
	@date 		04-25-2022
***********************************************************************/
#include "fsexc.hpp"
#include <sstream>

FsExceptionLocation::FsExceptionLocation(const std::string& fileName, const std::string& methodName, unsigned long lineNumber)
	: mFileName(fileName), mMethodName(methodName), mLineNumber(lineNumber)
{
}
	
FsExceptionLocation::FsExceptionLocation(const FsExceptionLocation& location) 
	: mFileName(location.mFileName), mMethodName(location.mMethodName), mLineNumber(location.mLineNumber)
{	
}
	
FsExceptionLocation::FsExceptionLocation() : mLineNumber(0)
{ 
}	

FsExceptionLocation& FsExceptionLocation::operator=(const FsExceptionLocation& location)
{
	if (this != &location)
	{
		mFileName = location.mFileName;
		mMethodName = location.mMethodName;
		mLineNumber = location.mLineNumber;
	}
	
	return *this;	
}

const std::string& FsExceptionLocation::getFileName() const
{
	return mFileName;
}
	
const std::string& FsExceptionLocation::getMethodName() const
{
	return mMethodName;
}
	
unsigned long FsExceptionLocation::getLineNumber() const
{
	return mLineNumber;
}

std::string FsExceptionLocation::getLocation() const {
    return "File: " + mFileName + "  Function: " + mMethodName + "  Line: " + std::to_string(mLineNumber);
}

FsException::FsException(unsigned long errorId, const std::string& errorMsg)
	: mErrorId(errorId), mErrorMsg(errorMsg)
{
}

FsException::FsException(const FsException& exception)
	: mErrorId(exception.mErrorId), mErrorMsg(exception.mErrorMsg), mCallStack(exception.mCallStack)
{
}

FsException::FsException() : mErrorId(0)
{
}

FsException& FsException::operator=(const FsException& fsException)
{
	if (this != &fsException)
	{
		mErrorId = fsException.mErrorId;
		mErrorMsg = fsException.mErrorMsg;
		mCallStack = fsException.mCallStack;
	}
	
	return *this;
}

unsigned long FsException::getErrorId() const
{
	return mErrorId;
}

const std::string& FsException::getErrorMessage() const
{
	return mErrorMsg;
}	

void FsException::printCallStack() const {
    for (const auto& location : mCallStack) {
        std::cerr << location.getLocation() << std::endl;
    }
}

std::string FsException::getCallStack() const {
    std::ostringstream oss;
    int count = 0;
    for (const auto& location : mCallStack) {
        if (count > 0) oss << "\n";
        oss << "(" << ++count << ".) " << location.getLocation();
    }
    return oss.str();
}




const std::list<FsExceptionLocation>* FsException::getCallStackList() const
{
	return &mCallStack;
}

void FsException::addToCallStack(const FsExceptionLocation& location)
{
    mCallStack.push_front(location);	
}

std::string FsException::buildError() const {
    std::ostringstream oss;
    oss << "-- Exception Occurred --\nError ID: ";
    if (mErrorId == 0)
        oss << "none";
    else
        oss << mErrorId << " - " << mErrorMsg;
    oss << "\n";
    return oss.str();
}
