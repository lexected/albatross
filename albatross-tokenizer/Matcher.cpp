/*
 * Matcher.cpp
 *
 *  Created on: Oct 30, 2014
 *      Author: lexected
 */

#include "Matcher.h"

#include <regex>
#include <string>
#include <sstream>
using namespace std;

Matcher::token_list Matcher::match(input_iterator & it, input_iterator end, Parser::rule_list rules) {
	MatchState state;
	state.it = it;
	state.end = end;
	state.line = 1;

	TokenCompound* rule_compound = new TokenCompound("", TokenRule::Action::Consume, TokenRule::Importance::Optional, TokenCompound::Type::Repetetive, rules, 1);
	match_compound(state, rule_compound);

	it = state.it;
	return state.tokens;
}
TokenRule::Action Matcher::match_compound(Matcher::MatchState & state, TokenCompound* compound) {
	MatchState saved_state = state;

	unsigned int accomplished_iterations = 0;
	bool can_carry_on;
	do {
		can_carry_on = false;
		for(TokenRule* rule : compound->rules) {
			TokenRule::Action action;

			if((dynamic_cast<TokenCompound*>(rule))!=nullptr)
				action = match_compound(state, dynamic_cast<TokenCompound*>(rule));
			else /* dynamic_cast<TokenForm*>(rule)!=nullptr */
				action = match_form(state, dynamic_cast<TokenForm*>(rule));

			if(rule->importance==TokenRule::Importance::Required && action==TokenRule::Action::None) {
				can_carry_on = false; //can't carry on, 'cause we haven't matched a required token
				break;
			} else if(rule->importance==TokenRule::Importance::Forbidden && action!=TokenRule::Action::None) {
				can_carry_on = false; //can't carry on, 'cause we've matched something what shouldn't be there
				break;
			} else if(action!=TokenRule::Action::None) //did we have a match?
				can_carry_on = true; //yeah, we did, so if this is repetetive compound, repeat! :)
		}

		if(can_carry_on)
			accomplished_iterations++;
	} while(can_carry_on && compound->type==TokenCompound::Type::Repetetive);

	if(accomplished_iterations==0) {
		/* we haven't matched anything, that means we haven't done anything */
		state = saved_state; //just to be sure
		return TokenRule::Action::None;
	}

	if((compound->action&TokenRule::Action::Consume)==0) {
		state.it = saved_state.it;
		state.line = saved_state.line;
	}
	if((compound->action&TokenRule::Action::Tokenize)!=0) {
		state.tokens.push_back(new MatchedToken(compound->name, state.production, saved_state.line));
	}
	if((compound->action&TokenRule::Action::Produce)==0) {
		state.production = saved_state.production;
	}

	return compound->action;
}
TokenRule::Action Matcher::match_form(MatchState & state, TokenForm* form) {
	smatch match;
	try {
		if(!std::regex_match(state.it, state.end, match, std::regex(form->regex, std::regex_constants::extended), regex_constants::match_continuous))
			return TokenRule::Action::None;
	} catch(regex_error & e) {
		stringstream iss;
		iss << e.code();
		throw MatcherException("regex error code: " + iss.str());
	}

	if((form->action&TokenRule::Action::Consume)!=0) {
		auto start = state.it;
		std::advance(state.it, match.length(0));
		state.line += count_linebreaks(start, state.it);
	}
	if((form->action&TokenRule::Action::Tokenize)!=0) {
		state.tokens.push_back(new MatchedToken(form->name, match[0], state.line));
	}
	if((form->action&TokenRule::Action::Produce)!=0) {
		state.production += match[0];
	}

	return form->action;
}
unsigned int Matcher::count_linebreaks(std::string::const_iterator it, std::string::const_iterator end) {
	unsigned int lbs = 0;

	while(it!=end) {
		if(*it=='\n')
			lbs++;

		it++;
	}

	return lbs;
}
