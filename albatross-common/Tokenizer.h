/*
 * Tokenizer.h
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <list>
#include <istream>

class Token {
public:
	enum Type {
		Identifier,
		String,

		Semicolon,
		Colon,
		Or,
		MinusSign,
		BitwiseNotSign,
		LogicNotSign,

		BeginRepeat,
		EndRepeat,
		BeginOptional,
		EndOptional,

		EndOfFile
	};

	Token(Type type, std::string s, unsigned int line) : type(type), s(s), line(line) { }
	virtual ~Token() = default;

	std::string getString() const { return this->s; }
	Type getType() const { return this->type; }
	unsigned int getLine() const { return this->line; }
private:
	Type type;
	std::string s;
	unsigned int line;
};

inline bool operator==(const Token* tkn, Token::Type type) {
	return tkn->getType()==type;
}
inline bool operator!=(const Token* tkn, Token::Type type) {
	return !operator==(tkn, type);
}

class Tokenizer final {
public:
	typedef std::list<Token*> token_list;
	typedef token_list::iterator token_iterator;

	static token_list parse(std::istreambuf_iterator<char> iterator);
private:
	Tokenizer() = default;
	virtual ~Tokenizer() = 0;
};


#endif /* TOKENIZER_H_ */
