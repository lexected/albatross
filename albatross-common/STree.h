/*
 * Stree.h
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#ifndef STREE_H_
#define STREE_H_

#include <string>
#include <list>

#include <istream>
#include <ostream>

class ISElement {
public:
	ISElement() = default;
	virtual ~ISElement() = default;

	virtual std::string generate(std::string indentation) = 0;
};
class ISList : public ISElement {
public:
	std::list<ISElement*> elements;

	ISList(bool isTopList = false) : ISElement(), elements(), topList(isTopList) { }
	ISList(std::list<ISElement*> elements, bool isTopList = false) : ISElement(), elements(elements), topList(isTopList) { }
	virtual ~ISList();

	virtual std::string generate(std::string indentation);

	bool isTopList() const { return this->topList; }
private:
	bool topList;
};
class ISTerminal : public ISElement {
public:
	ISTerminal(std::string string) : ISElement(), string(string) { }
	virtual ~ISTerminal() = default;

	std::string getString() const { return this->string; }
	virtual std::string generate(std::string indentation);
private:
	std::string string;
};
class ISEmpty : public ISElement {
public:
	ISEmpty() : ISElement() {}
	virtual ~ISEmpty() = default;

	virtual std::string generate(std::string indentation) { return ""; }
};

class STree final {
public:
	static ISElement* parse(std::istream & is);
	static void generate(std::ostream & os, ISElement* elements);

private:
	STree() = default;
	~STree() = default;
};


#endif /* STREE_H_ */
