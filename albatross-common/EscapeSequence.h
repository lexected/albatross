/*
 * EscapeSequence.h
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#ifndef ESCAPESEQUENCE_H_
#define ESCAPESEQUENCE_H_


class EscapeSequence final {
public:
	static char unescape(char c, unsigned int line);
	static std::string escape(std::string s);
private:
	EscapeSequence() = default;
	~EscapeSequence() = default;
};


#endif /* ESCAPESEQUENCE_H_ */
