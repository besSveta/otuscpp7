/*
 * nickname.h
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */
#pragma once
#include <iostream>
#include <map>
#include <unordered_set>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
struct Node;
class RadixTree {
	std::map<std::string, std::shared_ptr<Node>> members;

	void SetParentInner(std::shared_ptr<Node> first,
			std::shared_ptr<Node> second, std::string substr);
	void SetParent(std::shared_ptr<Node> first, std::shared_ptr<Node> second);
	void printParents();
public:
	RadixTree() {

	}
	bool insert(std::string word);
	void printNickNames();

	void printAsTree();
    std::string getParent(std::string value);
	};
