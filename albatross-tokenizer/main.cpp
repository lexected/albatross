/*
 * main.cpp
 *
 *  Created on: Oct 19, 2014
 *      Author: lexected
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;

#include "Parser.h"
#include "Matcher.h"

/* albatros-common */
#include <GeneralException.h>
#include <Tokenizer.h>
#include <STree.h>

int main(int argc, char** argv) {
	try {
		Tokenizer::token_list tokens = Tokenizer::parse(ifstream(argv[1]).rdbuf());
		Tokenizer::token_iterator token_iterator = tokens.begin();

		Parser::rule_list rules = Parser::parse(token_iterator);
		if(*token_iterator!=Token::Type::EndOfFile)
			throw GeneralException("Token rule parsing failed (not all tokens were consumed)");

		/* reading the input into memory */
		ifstream source("someinput.txt");
		source.seekg(0, std::ios::end);
		size_t size = source.tellg();
		source.seekg(0, std::ios::beg);
		string input(size, ' ');
		source.read(&input[0], size);

		Matcher::input_iterator input_iterator = input.cbegin();
		Matcher::token_list matched_tokens = Matcher::match(input_iterator, input.cend(), rules);

		ISList* root = new ISList();
		for(MatchedToken* t : matched_tokens) {
			stringstream ss;
			ss << t->line;
			root->elements.push_back(new ISList({ new ISTerminal(t->type), new ISTerminal(t->contents), new ISTerminal(ss.str())}));
		}
		STree::generate(cout, root);
	} catch(GeneralException & ex) {
		cerr << "Error: " << ex.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
