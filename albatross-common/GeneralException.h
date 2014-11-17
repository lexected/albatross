/*
 * GeneralException.h
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#ifndef GENERALEXCEPTION_H_
#define GENERALEXCEPTION_H_

#include <exception>
#include <string>

class GeneralException : public std::exception {
public:
	GeneralException() : std::exception(), message(nullptr) { }
	GeneralException(std::string message) : GeneralException() { this->message = new std::string(message); }

	virtual const char* what() {
		return this->message->c_str();
	}

	virtual ~GeneralException() { delete message; }
private:
	std::string* message;
};

#endif /* GENERALEXCEPTION_H_ */
