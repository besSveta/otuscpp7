/*
 * nickname_lib.cpp
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */

#include "nickname.h"

std::string findSubstring(const std::string &first, const std::string& second,
		size_t second_shift) {
	std::stringstream s("");
	size_t indx;
	auto secondBound = second.length() - second_shift;
	for (std::size_t i = 0; i < std::min(first.length(), secondBound); i++) {
		indx = i + second_shift;
		if (first[i] == second[indx]) {
			s << second[indx];
		} else
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
	t.childs[0] = std::unique_ptr<Node>(firstChild);
	t.childsCount = 1;
	if (!isEnd) {
		auto child = CreateNode(word, true);
		t.childs[1] = std::make_unique<Node>(std::move(child));
		t.childsCount++;
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
	// также поменять им родителя.
	for (size_t i = 0; i < parent.childsCount; i++) {
		temp = parent.childs[i].get();
		substr = findSubstring(temp->label, word, 0);
		len = substr.length();
		if (len > 0) {
			child.childs[child.childsCount++] = std::unique_ptr<Node>(
					parent.childs[i].release());
			for (size_t j = i + 1; j < parent.childsCount; j++) {
				parent.childs[i].reset(parent.childs[i + 1].release());
			}
			parent.childsCount--;
			temp->label = GetSubstring(temp->label, len);
		}

	}
	parent.childs[parent.childsCount++] = std::make_unique<Node>(
			std::move(child));
}

// если надо перезаписать корень. вернет true
// новый корень к третьем параметре
bool RadixTree::SetParent(Node* parent, const std::string &word,
		Node& newNode) {
	auto substr = findSubstring(parent->label, word, 0);
	auto len = substr.length();
	if (!parent->label.empty() && len == 0) {
		newNode = SetNewParent(parent, word);
		return true;
	} else {
		size_t currentShift;
		Node* currentNode(parent);
		Node* prevNode(currentNode);
		Node* temp;
		currentShift = len;
		auto prevLen = len;
		int childIndx = -1;
		/// поиск совпадений в детях.
		for (size_t i = 0; i < currentNode->childsCount; i++) {
			if (currentNode->childs[i] == nullptr) {
				break;
			}
			temp = currentNode->childs[i].get();
			substr = findSubstring(temp->label, word, currentShift);
			len = substr.length();
			if (len > 0 && len >= prevLen) {
				prevLen = len;
				prevNode = currentNode;
				// далее надо посмотеть детей найденного узла, на наличие пересечений.
				currentNode = temp;
				childIndx = i;
				currentShift += len;
				break;
			}

		}

		if (substr == currentNode->label)
			/// родитель существует уже в дереве, надо добваить в список детей
			SetParentInner(*currentNode, GetSubstring(word, currentShift));
		else {
			currentNode->label = GetSubstring(currentNode->label, len);
			//надо записать нового родителя добавляемого и узла пересекающегося с добавляемым, в список детей
			// вышестоящего узла, перезаписав.
			newNode = SetNewParent(currentNode,
					GetSubstring(word, currentShift), substr);
			if (childIndx >= 0) {
				prevNode->childs[childIndx].release();
				prevNode->childs[childIndx] = std::make_unique<Node>(
						std::move(newNode));
				return false;
			} else {
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
	if (exitingLabels.find(word) != exitingLabels.end()) {
		return false;
	}
	if (root == nullptr) {
		auto t = CreateNode(word, true);
		root = std::make_unique<Node>(std::move(t));
		exitingLabels.insert(word);
		return true;
	}

	Node temp;
	Node* rootNode = root.get();
	if (SetParent(rootNode, word, temp)) {
		root.release();
		root = std::make_unique<Node>(std::move(temp));

	}
	exitingLabels.insert(word);
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
	for (size_t i = 0; i < parent->childsCount; i++) {
		if (parent->childs[i] == nullptr)
			return;
		temp = parent->childs[i].get();
		if (temp->is_end && !temp->label.empty()) {
			std::cout << prevstr << temp->label << " " << prevstr;
			if (temp->childs[0] == nullptr) {
				std::cout << temp->label[0];
			} else {
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

	for (size_t i = 0; i < parent->childsCount; i++) {
		if (parent->childs[i] == nullptr)
			return;
		temp = parent->childs[i].get();
		if (!temp->label.empty()) {
			std::cout << std::setw(shift) << " ";
			std::cout << temp->label << std::endl;
		}
		printAsTree(temp, shift + 1);
	}
}

std::tuple<bool, std::string> RadixTree::getParent(std::string value) {
	if (root == nullptr) {
		return std::make_tuple(false, "");
	}
	if (exitingLabels.find(value) == exitingLabels.end()) {
		return std::make_tuple(false, "");
	}
	Node * currentNode = root.get();
	auto substr = findSubstring(currentNode->label, value, 0);
	if (substr == value) {
		return std::make_tuple(true, currentNode->label);
	}

	Node * temp;
	size_t len;
	size_t currentShift = 0;

	for (size_t i = 0; i < currentNode->childsCount; i++) {
		if (currentNode->childs[i] == nullptr) {
			break;
		}
		temp = currentNode->childs[i].get();
		substr = findSubstring(temp->label, value, currentShift);
		len = substr.length();
		if (len > 0) {
			currentShift += len;
			if (currentShift == value.length()) {
				return std::make_tuple(true, currentNode->label);
			}
			currentNode = temp;
		}

	}
	return std::make_tuple(false, "");
}

