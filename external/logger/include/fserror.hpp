#ifndef FSERROR_HPP
#define FSERROR_HPP

#include <string>

enum class ErrorType {
    Success = 0,
    FileNotFound = 1000,
    NullPointer = 1001,
    UnknownError = 1100
};

class FsError
{
private:
	std::string message;
	ErrorType errorType;
public:
	explicit FsError(ErrorType eType);		
	~FsError() = default;
	
    std::string getMessage() const;
    ErrorType getType() const;	
};

#endif // FSERROR_HPP   

