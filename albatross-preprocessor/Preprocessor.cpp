/*
 * Preprocessor.cpp
 *
 *  Created on: Oct 31, 2014
 *      Author: lexected
 */

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
using namespace std;

#include "Preprocessor.h"

/* albatros-common */
#include <SourceException.h>

void Preprocessor::process(std::istreambuf_iterator<char> & it, ostream & out, Preprocessor::State & state) {
	std::istreambuf_iterator<char> end;

	bool freshline = true;
	while(it!=end) {
		if(*it=='\n') {
			freshline = true;
			state.line++;

			it++;
			continue;
		}

		switch(state.charState) {
		case Preprocessor::State::CharacterState::CopyHandle:
			if(freshline && *it=='#') {
				it++; //skip '#'
				/* handle and execute a directive */
				Directive* directive = Directive::parse(it, state);
				Preprocessor::State::CharacterState tmps = directive->execute(state);
				if(tmps > state.charState) {
					Preprocessor::State::CharacterState tmp_tmps = tmps;
					tmps = state.charState;
					state.charState = tmp_tmps;

					it++;//parse() returns it pointing on the newline, we need it to be skipped when starting a new context, but not when we're staying in the old one
					process(it, out, state);
					state.charState = tmps;
				} else if(tmps==Preprocessor::State::CharacterState::ReturnLogic)
					return;
			} else {
				if(*it=='/') {
					auto old = it++;
					if(*it=='*') {
						state.charState = Preprocessor::State::CharacterState::CutComment;
						continue;
					} else it = old;
				}
				out << *it;
			}
			break;
		case Preprocessor::State::CharacterState::CutHandle:
			if(freshline && *it=='#') {
				it++; //skip '#'
				/* handle and execute a directive */
				Directive* directive = Directive::parse(it, state);
				Preprocessor::State::CharacterState tmps = directive->execute(state);
				if(tmps == Preprocessor::State::CharacterState::ReturnLogic)
					return;
				else
					state.charState = tmps;
			}
			break;
		case Preprocessor::State::CharacterState::CutComment:
			if(*it == '*') {
				auto old = it++;
				if(*it=='/')
					state.charState = Preprocessor::State::CharacterState::CopyHandle;
				else
					it = old;
			}
			break;
		case Preprocessor::State::CharacterState::ReturnLogic:
			throw GeneralException("Something unbelievable happened, and I have no idea how could this piece of code occur"); //now this is bad!!!!
			break;
		}

		if(!isspace(*it))
			freshline = false;
		it++;
	}
}

Directive* Directive::parse(std::istreambuf_iterator<char> & it, Preprocessor::State & state) {
	Directive* ret;
	std::istreambuf_iterator<char> end;

	stringstream ss;
	while(it!=end && !isspace(*it))
		ss << *it++;
	if(*it=='\n')
		state.line++;

	string directive = ss.str();
	if(directive=="endif")
		ret = new EndIfDirective();
	else if(directive=="else")
		ret = new ElseDirective(ss.str());
	else {
		do { it++; }
		while(it!=end && isspace(*it));

		ss.str("");
		bool backslash = false;
		while(it!=end) {
			if(*it=='\\')
				backslash = !backslash;
			else if(*it=='\n') {
				state.line++;
				if(backslash) {
					backslash = false;
				} else {
					break;
				}
			} else {
				backslash = false;
			}
			ss << *it++;
		}

		string param = ss.str();
		auto last_char = find_if(param.rbegin(), param.rend(), [](char c) { return !isspace(c); });
		param.erase(last_char.base(), param.end());

		if(directive=="define")
			ret = new DefineDirective(param);
		else if(directive=="undef")
			ret = new UndefDirective(param);
		else if(directive=="error")
			ret = new ErrorDirective(param, state.line-1/*-1 for the line we found on at the end of directive */);
		else if(directive=="ifdef")
			ret = new IfDefDirective(param);
		else if(directive=="ifndef")
			ret = new IfnDefDirective(param);
		else
			throw SourceException("Unknown directive '" + directive + "'", state.line);
	}

	return ret;
}

Preprocessor::State::CharacterState EndIfDirective::execute(Preprocessor::State & state) {
	return Preprocessor::State::CharacterState::ReturnLogic;
}
Preprocessor::State::CharacterState DefineDirective::execute(Preprocessor::State & state) {
	if(state.charState!=Preprocessor::State::CharacterState::CutHandle)
		state.definitions->push_back(this->name);

	return state.charState;
}
Preprocessor::State::CharacterState UndefDirective::execute(Preprocessor::State & state) {
	if(state.charState!=Preprocessor::State::CharacterState::CutHandle) {
		std::list<std::string>::iterator fit;
		if((fit = find_if(state.definitions->begin(), state.definitions->end(),
				[this](std::string & elem) { return elem==this->name; })) != state.definitions->end()) {
			state.definitions->erase(fit);
		} else
			throw SourceException("Trying to undefine an identifier that hasn't been defined", state.line);
	}

	return state.charState;
}
Preprocessor::State::CharacterState ErrorDirective::execute(Preprocessor::State & state) {
	if(state.charState!=Preprocessor::State::CharacterState::CutHandle)
		state.error << "Error: " << this->line << ": " << this->errormsg << '\n';

	return state.charState;
}
Preprocessor::State::CharacterState IfDefDirective::execute(Preprocessor::State & state) {
	std::list<std::string>::iterator fit;
	if((fit = find_if(state.definitions->begin(), state.definitions->end(),
			[this](std::string & elem) { return elem==this->name; })) != state.definitions->end()) {
		return Preprocessor::State::CharacterState::CopyHandle;
	} else
		return Preprocessor::State::CharacterState::CutHandle;
}
Preprocessor::State::CharacterState IfnDefDirective::execute(Preprocessor::State & state) {
	std::list<std::string>::iterator fit;
	if((fit = find_if(state.definitions->begin(), state.definitions->end(),
			[this](std::string & elem) { return elem==this->name; })) != state.definitions->end()) {
		return Preprocessor::State::CharacterState::CutHandle;
	} else
		return Preprocessor::State::CharacterState::CopyHandle;
}
Preprocessor::State::CharacterState ElseDirective::execute(Preprocessor::State & state) {
	if(state.charState==Preprocessor::State::CharacterState::CopyHandle)
		return Preprocessor::State::CharacterState::CutHandle;
	else
		return Preprocessor::State::CharacterState::CopyHandle;
}
