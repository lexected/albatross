/*
 * Parser.h
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <iterator>
#include <string>
#include <list>

/* albatros-common */
#include <IParsed.h>
#include <Tokenizer.h>

struct SyntaxTree;
struct Rule;
struct Alternation;
struct MemberList;

class Parser final {
public:
	static SyntaxTree* parse(Tokenizer::token_iterator & iterator);
private:
	static Rule* parse_rule(Tokenizer::token_iterator & iterator);
	static Alternation* parse_alternations(Tokenizer::token_iterator & iterator);
	static MemberList* parse_member_list(Tokenizer::token_iterator & iterator);
	static IParsed* parse_member(Tokenizer::token_iterator & iterator);
};

struct SyntaxTree : public IParsed {
	std::list<Rule*> elements;

	SyntaxTree(std::list<Rule*> elements, unsigned int line) : IParsed(line), elements(elements) { }
};
struct Rule : public IParsed {
	std::string name;
	Alternation* alternations;

	Rule(std::string name, Alternation* alternations, unsigned int line) : IParsed(line), name(name), alternations(alternations) { }
};
struct Alternation : public IParsed {
	MemberList* alternation_body;
	Alternation* alternations;

	Alternation(MemberList* alternation_body, Alternation* alternations, unsigned int line)
		: IParsed(line), alternation_body(alternation_body), alternations(alternations) { }
};
struct MemberList : public IParsed {
	IParsed* member;
	MemberList* alternation_body;

	MemberList(IParsed* member, MemberList* alternation_body, unsigned int line)
		: IParsed(line), member(member), alternation_body(alternation_body) { }
};

class Member : public IParsed {
public:
	enum Type {
		Identifier,
		TokenData,
		String,

		EndOfStream
	};
	Type getType() const { return this->type; }

	enum Output {
		FullToken,
		This,
		Inverse,
		Nothing
	};
	Output getOutput() const { return this->output; }

	std::string getString() const { return this->string; }

	Member(Type type, std::string string, unsigned int line, Output output = Output::FullToken)
		: IParsed(line), type(type), string(string), output(output) { }
	virtual ~Member() = default;
private:
	Type type;
	std::string string;
	Output output;
};

class AdjustedList : public IParsed {
public:
	enum Adjustment {
		Optional,
		Repetition
	};
	Adjustment getAdjustment() const { return this->adjustment; }

	MemberList* list;

	AdjustedList(MemberList* list, Adjustment adjustment, unsigned int line)
		: IParsed(line), list(list), adjustment(adjustment) { }
private:
	Adjustment adjustment;
};

#endif /* PARSER_H_ */
