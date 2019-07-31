// nickname.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "nickname.h"
int main(int, char *[]) {
	RadixTree tr;
	for (std::string line; std::getline(std::cin, line);) {
		tr.insert(line);
	}	
	tr.printNickNames(tr.GetParent(), "");
	tr.printAsTree(tr.GetParent(), 0);

}
