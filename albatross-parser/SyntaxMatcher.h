/*
 * SyntaxMatcher.h
 *
 *  Created on: Sep 28, 2014
 *      Author: lexected
 */

#ifndef SYNTAXMATCHER_H_
#define SYNTAXMATCHER_H_

#include <string>

#include "InputTokenizer.h"
#include "Parser.h"

/* albatros-common */
#include <STree.h>

class SyntaxMatcher {
public:
	SyntaxMatcher(SyntaxTree* syntax) : syntax(syntax) { }
	virtual ~SyntaxMatcher() = default;

	ISList* match(InputTokenizer::inputtoken_iterator & it);
private:
	ISList* match_rule(InputTokenizer::inputtoken_iterator & it, std::string name, unsigned int line);
	ISList* match_member_list(InputTokenizer::inputtoken_iterator & it, MemberList* body);
	ISElement* match_member(InputTokenizer::inputtoken_iterator & it, IParsed* member);

	SyntaxTree* syntax;
};

#endif /* SYNTAXMATCHER_H_ */
