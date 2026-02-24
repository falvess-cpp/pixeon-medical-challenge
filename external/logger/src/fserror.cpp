/********************************************************************  
	@file       fserror.cpp
	@author     Fabiano Souza
	@brief      Implementation of FsError class.
	@history    1.0      Initial version 
	@date 		04-25-2022
***********************************************************************/
#include "fserror.hpp"

FsError::FsError(ErrorType type) : errorType(type)
{ 
	switch(type)
	{
        case ErrorType::Success:
            message = "SUCCESS";
            break;
        case ErrorType::FileNotFound:
            message = "FILE_NOT_FOUND";
            break;
        case ErrorType::NullPointer:
            message = "NULL_POINTER";
            break;
        case ErrorType::UnknownError:
            message = "UNKNOWN_ERROR";
            break;
        default:
            message = "UNRECOGNIZED_ERROR";
            break;

	}
}	
	
std::string FsError::getMessage() const {
    return message;
}

ErrorType FsError::getType() const {
    return errorType;
}
