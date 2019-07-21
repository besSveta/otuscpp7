/*
 * nickname_lib.cpp
 *
 *  Created on: 22 июл. 2019 г.
 *      Author: sveta
 */
#include "nickname.h"
struct Node  {
	std::string label;
	std::shared_ptr<Node> parent;
	bool hasChildren;
	bool isFake;

	Node() {
		label = "";
		hasChildren = false;
		parent = nullptr;
		isFake = false;
	}

	Node(std::string word, bool fake = false) :
			label(word) {
		hasChildren = false;
		parent = nullptr;
		isFake = fake;

	}
	Node(const Node&nd) :
			label(nd.label), parent(nd.parent), hasChildren(nd.hasChildren), isFake(
					nd.isFake) {

	}
};

std::string findSubstring(const std::string& first, const std::string& second) {
	std::stringstream s("");
	for (std::size_t i = 0; i < std::min(first.length(), second.length());
			i++) {
		if (first[i] == second[i]) {
			s << first[i];
		} else
			return s.str();
	}
	return s.str();
}

	// после поиска подстрок в родителях вызываем функцию для задания родителя.
	void RadixTree::SetParentInner(std::shared_ptr<Node> first,
			std::shared_ptr<Node> second, std::string substr) {

		if (substr.empty()) {
			return;
		}

		if (substr == first->label) {
			// смогли найти родителя добавляемой строки.
			// если  нашли подстроку в имеющихся
			second->parent = first;
			first->hasChildren = true;
		} else {
			// родителя не нашли, но есть общая часть,
			// создаем для нее узел и делаем родителем добавляемой и наименьшей найденной строки.
			second->parent = std::make_shared<Node>(substr, true); // указать, что этот элемент не из потока данных.
			first->parent = second->parent;
			members.insert(
					std::pair<std::string, std::shared_ptr<Node>>(substr,
							second->parent));
		}
	}

	void RadixTree::SetParent(std::shared_ptr<Node> first, std::shared_ptr<Node> second) {
		// содержит ли добавляемая строка строку из уже имеющихся.
		auto substr = findSubstring(second->label, first->label);
		auto len = substr.length();
		auto prevSubstr=substr;
		auto prevlen = len;;
		std::shared_ptr<Node> firstPtr(first->parent);
		std::shared_ptr<Node> currentPtr(first);
		//если есть строка,которая содержит добавляемую, посмотрим родителей.
		while (len >= 1 && firstPtr != nullptr) {
			substr = findSubstring(second->label, firstPtr->label);
			len = substr.length();
			// стоит продолжить смотреть родителей, если длина пересечения строк не поменялась.
			if (len == prevlen) {
				currentPtr = firstPtr;
				firstPtr = firstPtr->parent;
			} else{
				substr=prevSubstr;
				break;
			}
		}
		SetParentInner(currentPtr, second, substr);

	}

bool RadixTree::insert(std::string word) {

		auto it = members.lower_bound(word);

		if (it != members.end()) {
			if (it->first == word)
				return false;
			auto newMember = std::make_shared<Node>(word);
			// если есть строки, которые лексически меньше добавляемой.
			if (it != members.begin()) {
				auto prev = std::prev(it);
				// содержит ли добавляемая строка строку из уже имеющихся.
				SetParent(prev->second, newMember);
			} else {
				// нет строк, которые меньше добавляемой.
				newMember->parent = nullptr;
			}
			// рассматриваем строку, большую чем добавляемая.
			// добавляемая содержится в одной из имеющихся.
			SetParent(newMember, it->second);
			members[word] = newMember;
		} else {
			// нет строк, которые больше добавляемой.
			auto newMember = std::make_shared<Node>(word);
			newMember->hasChildren = false;
			if (members.size() > 0) {
				SetParent(members.begin()->second, newMember);
			}
			members[word] = newMember;
		}
		return true;

	}
	void RadixTree::printParents() {
		for (auto m : members) {
			if (m.second->parent != nullptr)
				std::cout << m.first << " parent: " << m.second->parent->label
						<< std::endl;
			else {
				std::cout << m.first << std::endl;
			}
		}
	}
	std::string RadixTree::getParent(std::string value){
		auto it=members.find(value);
		if (it != members.end()){
			return it->second->parent->label;
		}
		return "";
	}

	void RadixTree::printNickNames() {
		for (auto m : members) {
			if (!m.second->isFake) {
				if (!m.second->hasChildren) {
					if (m.second->parent != nullptr) {

						auto len = m.second->parent->label.length();
						if (m.second->label.length() > len) {
							std::cout << m.second->label << " "
									<< m.second->parent->label
									<< m.second->label[len] << std::endl;
						}
					} else if (!m.second->label.empty()) {
						std::cout << m.second->label << " "
								<< m.second->label[0] << std::endl;
					}
				} else {
					std::cout << m.second->label << " " << m.second->label
							<< std::endl;
				}
			}
		}
	}

	void RadixTree::printAsTree() {
		   std::shared_ptr<Node> currentPtr;
		   std::unordered_set<std::string> used;
		   std::stringstream ss;
		   std::stringstream ss2;
		   std::string tempstr;
			for (auto m : members) {
				ss.str("");
				currentPtr=m.second;
				while(currentPtr->parent != nullptr){
					auto len=currentPtr->parent->label.length();
					 ss2.str("");
					for (std::size_t i =0; i<currentPtr->label.length(); i++){
						if (i<len){
							ss<<" ";
						}
						else
							{

							  ss2<<currentPtr->label[i];
							}
					}
					tempstr=ss2.str();
					if(used.find(tempstr)==used.end())
					{
						ss<<tempstr;
						if (!currentPtr->isFake)
							ss<<"$";
					}

					used.insert(tempstr);
					currentPtr=currentPtr->parent;
				}
				if (used.find(currentPtr->label)==used.end()){
					ss<<currentPtr->label;
					if (!currentPtr->isFake)
						ss<<"$";
				}
				std::cout<<ss.str()<<std::endl;
				used.insert(currentPtr->label);
			}

		}



