/*
 * Preprocessor.h
 *
 *  Created on: Oct 31, 2014
 *      Author: lexected
 */

#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include <list>
#include <iterator>
#include <ostream>

class Preprocessor final {
public:
	Preprocessor() = delete;
	~Preprocessor() = delete;

	class State {
	public:
		enum CharacterState {
			ReturnLogic, //a state used by ElseIf to mark that we are returning to upper level of if-end structure

			CopyHandle,
			CutHandle,
			CutComment
		};
		CharacterState charState;
		unsigned int line;

		std::list<std::string> * definitions;
		std::ostream & error;

		State(CharacterState charState, unsigned int line, std::list<std::string> * definitions, std::ostream & error)
			: charState(charState), line(line), definitions(definitions), error(error) { }
	};

	static void process(std::istreambuf_iterator<char> & it, ostream & out, State & state);
};

class Directive {
public:
	Directive() = default;
	virtual ~Directive() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state) = 0;

	static Directive* parse(std::istreambuf_iterator<char> & it, Preprocessor::State & state);
};
class DefinitionDirective : public Directive {
public:
	DefinitionDirective(std::string name) : name(name) { }
	virtual ~DefinitionDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state) = 0;
protected:
	std::string name;
};
class DefineDirective : public DefinitionDirective {
public:
	DefineDirective(std::string name) : DefinitionDirective(name) { }
	virtual ~DefineDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class UndefDirective : public DefinitionDirective {
public:
	UndefDirective(std::string name) : DefinitionDirective(name) { }
	virtual ~UndefDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class IfDefDirective : public DefinitionDirective {
public:
	IfDefDirective(std::string name) : DefinitionDirective(name) { }
	virtual ~IfDefDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class IfnDefDirective : public DefinitionDirective {
public:
	IfnDefDirective(std::string name) : DefinitionDirective(name) { }
	virtual ~IfnDefDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class ElseDirective : public DefinitionDirective {
public:
	ElseDirective(std::string name) : DefinitionDirective(name) { }
	virtual ~ElseDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class EndIfDirective : public Directive {
public:
	EndIfDirective() : Directive() { }
	virtual ~EndIfDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
};
class ErrorDirective : public Directive {
public:
	ErrorDirective(std::string errormsg, unsigned int line) : errormsg(errormsg), line(line) { }
	virtual ~ErrorDirective() = default;

	virtual Preprocessor::State::CharacterState execute(Preprocessor::State & state);
private:
	std::string errormsg;
	unsigned int line;
};

#endif /* PREPROCESSOR_H_ */
