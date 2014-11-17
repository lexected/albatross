/*
 * Parser.h
 *
 *  Created on: Oct 19, 2014
 *      Author: lexected
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <list>
#include <string>

/* albatros-common */
#include <IParsed.h>
#include <ParserException.h>
#include <Tokenizer.h>

class TokenRule;
class TokenCompound;
class TokenForm;

class TokenRule : public IParsed {
public:
	std::string name;

	enum Action {
		/* flags */
		None,

		Consume = 1,
		Tokenize = 2,
		Produce = 4
	};
	Action action;

	enum Importance {
		Optional,
		Required,
		Forbidden
	};
	Importance importance;

	TokenRule(std::string name, Action actions, Importance importance, unsigned int line)
		: IParsed(line), name(name), action(actions), importance(importance) { }
	virtual ~TokenRule() = default;
};
inline TokenRule::Action operator|(TokenRule::Action a, TokenRule::Action b) {
	return static_cast<TokenRule::Action>(static_cast<int>(a) | static_cast<int>(b));
}
inline TokenRule::Importance operator|(TokenRule::Importance a, TokenRule::Importance b) {
	return static_cast<TokenRule::Importance>(static_cast<int>(a) | static_cast<int>(b));
}

class TokenCompound : public TokenRule {
public:
	enum Type {
		Simple,
		Repetetive
	};
	Type type;
	std::list<TokenRule*> rules;

	TokenCompound(std::string name, Action actions, Importance importance, Type type, std::list<TokenRule*> rules, unsigned int line)
		: TokenRule(name, actions, importance, line), type(type), rules(rules) { }
	virtual ~TokenCompound() {
		for(TokenRule* r : this->rules)
			delete r;
	}
};
class TokenForm : public TokenRule {
public:
	std::string regex;

	TokenForm(std::string name, Action actions, Importance importance, std::string regex, unsigned int line)
		: TokenRule(name, actions, importance, line), regex(regex) { }
	virtual ~TokenForm() = default;
};

class Parser final {
public:
	Parser() = delete;
	~Parser() = delete;

	typedef std::list<TokenRule*> rule_list;
	typedef rule_list::iterator rule_iterator;

	static rule_list parse(Tokenizer::token_iterator & it);
private:
	static TokenForm* parse_tokenform(Tokenizer::token_iterator & it);
	static TokenCompound* parse_tokencompound(Tokenizer::token_iterator & it);
	static rule_list* parse_tokenrulelist(Tokenizer::token_iterator & it);

	static TokenRule::Action parse_actions(Tokenizer::token_iterator & it);
	static TokenRule::Importance parse_importance(Tokenizer::token_iterator & it);
};

#endif /* PARSER_H_ */
