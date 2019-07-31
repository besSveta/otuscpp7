/*
 * nickname_lib.cpp
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */

#include "nickname.h"

std::string findSubstring(const std::string &first, const std::string& second,
	size_t second_shift) {
	if (first.empty() || second.empty())
		return "";
	std::stringstream s("");
	size_t indx;
	auto secondBound = second.length() - second_shift;
	for (std::size_t i = 0; i < std::min(first.length(), secondBound); i++) {
		indx = i + second_shift;
		if (first[i] == second[indx]) {
			s << second[indx];
		}
		else
			return s.str();
	}
	return s.str();
}

std::string GetSubstring(const std::string& word, size_t shift) {
	std::stringstream s("");
	for (std::size_t i = shift; i < word.length(); i++) {
		s << word[i];
	}
	return s.str();
}

// после поиска подстрок в родителях вызываем функцию для задания родителя.
Node RadixTree::SetNewParent(Node *firstChild, const std::string &word,
	const std::string &parentLabel = "") {
	auto isEnd = word.empty();
	auto t = CreateNode(parentLabel, isEnd);
	size_t indx = (size_t)(firstChild->label[0]);
	t.childs[indx] = std::unique_ptr<Node>(firstChild);
	if (!isEnd) {
		auto child = CreateNode(word, true);
		indx = (size_t)word[0];
		t.childs[indx] = std::make_unique<Node>(std::move(child));
	}
	return t;
}

Node* RadixTree::GetParent() {
	if (root != nullptr) {
		return root.get();
	}
	return nullptr;
}

void RadixTree::SetParentInner(Node &parent, const std::string &word) {
	if (word.empty())
		return;
	auto child = CreateNode(word, true);
	std::string substr;
	size_t len;
	Node* temp;
	// если есть потомки содержащие заданное слово, надо это слово вырезать.
	// также поменять  родителя.
	size_t indx = (size_t)word[0];
	if (parent.childs[indx] != nullptr) {
		temp = parent.childs[indx].get();
		substr = findSubstring(temp->label, word, 0);
		len = substr.length();
		if (len > 0) {
			temp->label = GetSubstring(temp->label, len);
			child.childs[(size_t)(temp->label[0])] = std::unique_ptr<Node>(
				parent.childs[indx].release());
		}
	}
	parent.childs[indx] = std::make_unique<Node>(std::move(child));
}

std::tuple<Node*, Node*, std::string> RadixTree::FindNode(std::string word) {
	Node* temp = root.get();
	if (word.empty())
	{
		if (temp->label.empty())
			return std::make_tuple(temp, nullptr, "");
		else
			return std::make_tuple(nullptr, nullptr, "");
	}

	Node* currentNode(root.get());
	auto substr = findSubstring(temp->label, word, 0);
	size_t currentShift;
	auto len = substr.length();

	if (len == 0 && !temp->label.empty())
		return std::make_tuple(nullptr, currentNode, "");
	if (word.length() == len)
		return std::make_tuple(nullptr, currentNode, substr);
	Node * prevNode(nullptr);
	currentShift = len;
	/// поиск совпадений в детях.
	size_t i = (size_t)word[len];
	while (currentNode->childs[i] != nullptr) {
		prevNode = currentNode;
		currentNode = currentNode->childs[i].get();
		substr = findSubstring(currentNode->label, word, currentShift);
		// далее надо посмотеть детей найденного узла, на наличие пересечений.
		len = substr.length();
		if (len == 0)
			break;

		if (len <= currentNode->label.length()) {
			return std::make_tuple(prevNode, currentNode, substr);
		}
		currentShift += len;
		if (currentShift < word.length()) {
			i = word[currentShift];
		}
		else {
			return std::make_tuple(prevNode, currentNode, substr);
		}

	}
	// prevNode - родитель того узла с которым найдено пересечение
	// currentNode - узел с которым найдено пересечение
	// substr - строка перечечения
	return std::make_tuple(prevNode, currentNode, substr);

}
// если надо перезаписать корень. вернет true
// новый корень к третьем параметре
bool RadixTree::SetParent(Node* parent, const std::string &word,
	Node& newNode) {
	std::tuple<Node*, Node*, std::string> found = FindNode(word);
	Node* prevNode = std::get<0>(found);
	Node* currentNode = std::get<1>(found);
	auto substr = std::get<2>(found);
	int len = substr.length();
	if (currentNode->label == word) {
		return false;
	}
	if (!currentNode->label.empty() && len == 0) {
		newNode = SetNewParent(parent, word);
		return true;
	}
	else {
		// substr - строка перечечения
		if (substr == currentNode->label)
			/// родитель существует уже в дереве, надо добваить в список детей
			SetParentInner(*currentNode, GetSubstring(word, len));
		else {
			currentNode->label = GetSubstring(currentNode->label, substr.length());
			//надо записать нового родителя добавляемого и узла пересекающегося с добавляемым, в список детей
			// вышестоящего узла, перезаписав.
			newNode = SetNewParent(currentNode, GetSubstring(word, len), substr);
			auto childIndx = (size_t)substr[0];
			if (childIndx >= 0 && prevNode != nullptr) {
				prevNode->childs[childIndx].release();
				prevNode->childs[childIndx] = std::make_unique<Node>(
					std::move(newNode));
				return false;
			}
			else {
				return true;
			}

		}
		return false;
	}

}

Node CreateNode(const std::string &word, bool isEnd) {
	auto temp = Node();
	temp.label = word;
	temp.is_end = isEnd;
	return temp;
}

bool RadixTree::insert(std::string word) {

	if (word.empty()) {
		return false;
	}

	if (root == nullptr) {
		auto t = CreateNode(word, true);
		root = std::make_unique<Node>(std::move(t));
		return true;
	}

	Node temp;
	Node* rootNode = root.get();
	if (SetParent(rootNode, word, temp)) {
		root.release();
		root = std::make_unique<Node>(std::move(temp));

	}
	return true;

}

void RadixTree::printNickNames(Node *parent, std::string prevstr) {
	if (parent == nullptr) {
		return;
	}
	Node * temp;
	if (parent == root.get() && !parent->label.empty()) {
		prevstr = parent->label;
	}
	for (size_t i = 0; i < parent->childs.size(); i++) {
		if (parent->childs[i] == nullptr)
			continue;
		temp = parent->childs[i].get();
		if (temp->is_end && !temp->label.empty()) {
			std::cout << prevstr << temp->label << " " << prevstr;
			if (temp->childs[0] == nullptr) {
				std::cout << temp->label[0];
			}
			else {
				std::cout << temp->label;
			}

			std::cout << std::endl;
		}
		printNickNames(temp, prevstr + temp->label);
	}
}

void RadixTree::printAsTree(Node *parent, int level) {
	if (parent == nullptr) {
		return;
	}
	Node * temp;
	int shift = level;
	if (!parent->label.empty()) {
		shift = parent->label.length() + level;
		if (level == 0) {
			std::cout << parent->label << std::endl;
		}

	}

	for (size_t i = 0; i < parent->childs.size(); i++) {
		if (parent->childs[i] == nullptr)
			continue;
		temp = parent->childs[i].get();
		if (!temp->label.empty()) {
			std::cout << std::setw(shift) << " ";
			std::cout << temp->label << std::endl;
		}
		printAsTree(temp, shift + 1);
	}
}

bool RadixTree::CheckIfExists(std::string word) {
	std::tuple<Node*, Node*, std::string> found = FindNode(word);
	auto substr = std::get<2>(found);
	if (substr == word)
		return true;

	return false;

}
std::tuple<bool, std::string> RadixTree::getParent(std::string word) {
	std::tuple<Node*, Node*, std::string> found = FindNode(word);
	Node* prevNode = std::get<0>(found);
	Node* currentNode = std::get<1>(found);
	auto substr = std::get<2>(found);
	if (substr.empty())
		return std::make_tuple(false, "");
	if (prevNode == nullptr)
	{
		if (substr == word)
			return  std::make_tuple(true, currentNode->label);
		else
			return std::make_tuple(false, "");
	}

	if (substr == word)
		return  std::make_tuple(true, currentNode->label);
	auto childIndx = (size_t)word[substr.length()];
	prevNode = currentNode->childs[childIndx].get();
	if (substr + prevNode->label == word)
		return  std::make_tuple(true, currentNode->label);
	return std::make_tuple(false, "");
}

