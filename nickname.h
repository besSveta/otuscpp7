/*
 * nickname.h
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */
#pragma once
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <sstream>
#include <tuple>
#include<iomanip>
struct Node {
	std::string label;
	bool is_end;
	std::array<std::unique_ptr<Node>, 256> childs;

};
Node CreateNode(const std::string &word, bool isEnd);

class RadixTree {
	std::unique_ptr<Node> root;
	Node SetNewParent(Node* firstChild, const std::string &word,
		const std::string &parentLabel);
	void SetParentInner(Node &parent, const std::string &word);
	bool SetParent(Node* parent, const std::string &word, Node& newNode);
	void printParents();
	std::tuple<Node*, Node*, std::string> FindNode(std::string word);
	bool CheckIfExists(std::string word);
public:
	RadixTree() :
		root(nullptr) {

	}
	bool insert(std::string word);
	void printNickNames(Node *parent, std::string ss);

	void printAsTree(Node *parent, int level, int childNum);
	Node* GetParent();
	std::tuple<bool, std::string> getParent(std::string value);
};

