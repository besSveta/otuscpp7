/*
 * nickname.cpp
 *
 *  Created on: 21 июл. 2019 г.
 *      Author: sveta
 */
#include "nickname.h"
int main(int, char *[]) {
	RadixTree tr;
	for (std::string line; std::getline(std::cin, line);) {
		tr.insert(line);
	}

	tr.printNickNames(tr.GetParent(), "");
	tr.printAsTree(tr.GetParent(), 0);
}

