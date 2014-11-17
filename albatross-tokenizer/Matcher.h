/*
 * Matcher.h
 *
 *  Created on: Oct 30, 2014
 *      Author: lexected
 */

#ifndef MATCHER_H_
#define MATCHER_H_

#include <iterator>
#include <sstream>
#include <list>

#include "Parser.h"

/* albatros-common */
#include <GeneralException.h>

struct MatchedToken {
	std::string type;
	std::string contents;
	unsigned int line;

	MatchedToken(std::string type, std::string contents, unsigned int line)
		: type(type), contents(contents), line(line) { }
};

class Matcher final {
public:
	Matcher() = delete;
	~Matcher() = delete;

	typedef std::list<MatchedToken*> token_list;
	typedef token_list::iterator token_iterator;

	typedef std::string::const_iterator input_iterator;
	static Matcher::token_list match(input_iterator & it, input_iterator end, Parser::rule_list rules);
private:
	class MatchState {
	public:
		/* consume */
		input_iterator it, end;
		/* tokenize */
		std::list<MatchedToken*> tokens;
		/* produce */
		std::string production;

		unsigned int line;

		MatchState()
			: it(), end(), tokens(), production() { }
		MatchState(const MatchState & original)
			: it(original.it), end(original.end), tokens(original.tokens), production(original.production) { }
	};
	static TokenRule::Action match_compound(MatchState & state, TokenCompound* compound);
	static TokenRule::Action match_form(MatchState & state, TokenForm* form);

	static unsigned int count_linebreaks(std::string::const_iterator it, std::string::const_iterator end);
};

class MatcherException : public GeneralException {
public:
	MatcherException(std::string msg) : GeneralException(msg) {	}
	virtual ~MatcherException() = default;
};

#endif /* MATCHER_H_ */
