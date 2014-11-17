/*
 * Parser.h
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#ifndef IPARSED_H_
#define IPARSED_H_

class IParsed {
public:
	unsigned int getLine() const { return this->line; }
protected:
	IParsed(unsigned int line) : line(line) { }
	virtual ~IParsed() = default;
private:
	unsigned int line;
};

#endif /* IPARSED_H_ */
