/*
 * ParserException.h
 *
 *  Created on: Aug 31, 2014
 *      Author: lexected
 */

#ifndef PARSEREXCEPTION_H_
#define PARSEREXCEPTION_H_

#include <exception>
#include <sstream>
#include <cstdint>

#include "IParsed.h"

/* albatros-common */
#include <SourceException.h>

class ParserException: public SourceException {
public:
	ParserException(std::string msg) : SourceException(msg, 0) { }
	ParserException(std::string msg, unsigned int line) : SourceException(msg, line) { }
	ParserException(std::string msg, IParsed parsed) : SourceException(msg, parsed.getLine()) { }
	virtual ~ParserException() = default;
};

#endif /* PARSEREXCEPTION_H_ */
