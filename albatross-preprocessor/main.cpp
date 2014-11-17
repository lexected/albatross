/*
 * main.cpp
 *
 *  Created on: Oct 30, 2014
 *      Author: lexected
 */

#include <cstdlib>
#include <string>
#include <list>
#include <fstream>
#include <iterator>
#include <iostream>
using namespace std;

#include "Preprocessor.h"

/* albatros-common */
#include <GeneralException.h>

int main(int argc, char** argv) {
	ifstream input("input.txt", std::ios::binary);
	istreambuf_iterator<char> it = input.rdbuf();

	try {
		Preprocessor::State state(Preprocessor::State::CharacterState::CopyHandle, 1, new list<string>(), std::cerr);
		Preprocessor::process(it, std::cout, state);
	} catch(GeneralException & ex) {
		std::cerr << ex.what() << endl;
	}

	return EXIT_SUCCESS;
}
