/*
 * Tokenzier.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#include <sstream>
#include <cctype>
#include <string>
using namespace std;

#include "Tokenizer.h"

#include "EscapeSequence.h"
#include "SourceException.h"

enum TokenzierState {
	Whitespace,
	Identifier,
	String
};
Tokenizer::token_list Tokenizer::parse(std::istreambuf_iterator<char> iterator) {
	token_list ret;

	unsigned int line = 1;
	bool escape = false;

	TokenzierState state = TokenzierState::Whitespace;
	stringstream currtok;
	decltype(iterator) eos;
	while(iterator!=eos) {
		char c;
		c = *iterator;

		if(c=='\n') {
			line++;

			iterator++;
			continue;
		}

		switch(state) {
		case TokenzierState::Whitespace:
			if(isspace(c)) {
				break;
			} else if(isalpha(c) || c=='_') {
				state = TokenzierState::Identifier;
				continue;
			} else if(c=='"') {
				state = TokenzierState::String;
			} else {
				Token* punction;
				switch(c) {
				case ':':
					punction = new Token(Token::Type::Colon, string() + c, line);
					break;
				case ';':
					punction = new Token(Token::Type::Semicolon, string() + c, line);
					break;
				case '|':
					punction = new Token(Token::Type::Or, string() + c, line);
					break;
				case '-':
					punction = new Token(Token::Type::MinusSign, string() + c, line);
					break;
				case '~':
					punction = new Token(Token::Type::BitwiseNotSign, string() + c, line);
					break;
				case '!':
					punction = new Token(Token::Type::LogicNotSign, string() + c, line);
					break;

				case '(':
					punction = new Token(Token::Type::BeginRepeat, string() + c, line);
					break;
				case ')':
					punction = new Token(Token::Type::EndRepeat, string() + c, line);
					break;
				case '[':
					punction = new Token(Token::Type::BeginOptional, string() + c, line);
					break;
				case ']':
					punction = new Token(Token::Type::EndOptional, string() + c, line);
					break;
				default:
					throw SourceException(string() + "Unexpected character '" + c + "' found", line);
					break;
				}
				ret.push_back(punction);
			}
			break;
		case TokenzierState::Identifier:
			if(isalpha(c) || isdigit(c) || c=='_')
				currtok << c;
			else {
				ret.push_back(new Token(Token::Type::Identifier, currtok.str(), line));
				currtok.str("");

				state = TokenzierState::Whitespace;
				continue; //continuing prevents iterator from being incremented
			}
			break;
		case TokenzierState::String:
			if(escape) {
				currtok << EscapeSequence::unescape(c, line);

				escape = false;
				break;
			}

			if(c=='\\') {
				escape = true;
			} else if(c=='"') {
				ret.push_back(new Token(Token::Type::String, currtok.str(), line));
				currtok.str("");

				state = TokenzierState::Whitespace;
			} else {
				currtok << c;
			}
			break;
		}

		iterator++;
	}

	ret.push_back(new Token(Token::Type::EndOfFile, "", line));
	return ret;
}
