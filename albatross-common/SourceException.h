/*
 * SourceException.h
 *
 *  Created on: Oct 18, 2014
 *      Author: lexected
 */

#ifndef SOURCEEXCEPTION_H_
#define SOURCEEXCEPTION_H_

#include <string>

#include "GeneralException.h"

class SourceException : public GeneralException {
public:
	SourceException(std::string msg, unsigned int line) : GeneralException(msg), line(line) { }
	virtual ~SourceException() = default;

	unsigned int getLine() const { return this->line; }
	virtual const char* what() {
		std::stringstream ss;
		ss << this->line << ": " << GeneralException::what();

		std::string* retc_str = new std::string(ss.str());
		return retc_str->c_str();
	}

private:
	unsigned int line;
};


#endif /* SOURCEEXCEPTION_H_ */
