/*
 * main.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: lexected
 */

#include <cstdlib>

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "Parser.h"
#include "InputTokenizer.h"
#include "SyntaxMatcher.h"
#include "ParserException.h"

/* albatros-common */
#include <STree.h>
#include <Tokenizer.h>

class String : public ISElement {
public:
	String(std::string string) : ISElement(), string(string) { }
	virtual ~String() = default;

	virtual std::string generate(std::string indentation) {
		return string;
	}
private:
	std::string string;
};

int main(int argc, char** argv) {
	if(argc!=2) {
		cerr << "Error: no syntax file specified, expected only a single argument" << endl;
		return EXIT_FAILURE;
	}
	ifstream file(argv[1], std::ios::binary);
	if(!file.is_open()) {
		cerr << "Error: couldn't open the input file specified" << endl;
		return EXIT_FAILURE;
	}

	try {
		Tokenizer::token_list tokens = Tokenizer::parse(file.rdbuf());
		Tokenizer::token_iterator token_iterator = tokens.begin();

		SyntaxTree* syntaxtree = Parser::parse(token_iterator);
		if(*token_iterator!=Token::Type::EndOfFile)
			throw GeneralException("Grammar parsing failed (not all tokens were consumed)");

		ifstream input_file("tokens.list");
		InputTokenizer::inputtoken_list input_tokens = InputTokenizer::parse(input_file);
		InputTokenizer::inputtoken_iterator inputtoken_iterator = input_tokens.begin();

		ISList* ast = SyntaxMatcher(syntaxtree).match(inputtoken_iterator);
		if(inputtoken_iterator!=input_tokens.end())
			throw GeneralException("Input parsing failed (not all tokens were consumed)");

		STree::generate(std::cout, ast);
	} catch(GeneralException & ex) {
		cerr << "Error: " << ex.what() << endl << flush;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
