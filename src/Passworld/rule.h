#pragma once
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include "globalConst.h"

class Permutator { //base permutator class
private:
	double m_weight;
	int m_type;
	int m_id;
	std::string m_displayName; //name displayed on gui
public:
	Permutator(int type, double weight, int id, std::string displayName);
	virtual ~Permutator(){}
	double const getWeight();
	int const getType();
	int const getId();
	std::string& getDisplayName();
};

int resolvePermutatorPriority(int type); //return an int (priority) based on the permutator type

class PrefixRequirement {
public:
	std::string m_payload;
	double m_weight;
	PrefixRequirement(std::string payload, double weight);
};
class SuffixRequirement {
public:
	std::string m_payload;
	double m_weight;
	SuffixRequirement(std::string payload, double weight);
};
class IndexRequirement {
public:
	char m_payload;
	int m_index;
	double m_weight;
	IndexRequirement(char payload, int index, double weight);
};

class PasswordRequirement {
private:
	int m_minLength; // password minimum length
	int m_maxLength; // password maximum length
	bool m_uppercase; // password requires uppercase
	bool m_number; // passwords requires numbers
	bool m_specialSymbol; // passwords requires special symbols
	std::vector<char> m_symbolList; // array with special symbols allowed to use 
	bool m_validatePrefix;
	bool m_validatePostfix;
public:
	PasswordRequirement(int minLength, int maxLenght, bool uppercase, bool number, bool specialSymbol, std::vector<char> symbolList, bool validatePrefix, bool validatePostfix);
	PasswordRequirement() = default;
	int const getMinLength();
	int const getMaxLength();
	bool const getRequireUppercase();
	bool const getRequireNumber();
	bool const getRequireSymbol();
	bool const getValidatePrefix();
	bool const getValidatePostfix();
	std::vector<char>* const getSymbolList();
	void setMinLength(int value);
	void setMaxLenght(int value);
	void setRequireUppercase(bool value);
	void setRequireSymbol(bool value);
	void setRequireNumber(bool value);
	void setSymbolList(std::vector<char>&& list);

};


class Paradigm { 
private:
	std::string m_pattern;
	double m_weight;
public:
	Paradigm(std::string&& pattern, double weight);
	double getWeight();
	const std::string& getPattern();
};

/* probably useless classes
class Requirement {
	int m_requirementId;
public:
	Requirement(int requirementId);
};

class PatternRequirement {
	// some value to discern if the paradigm cna be used or not
	std::vector<Requirement> requirementList;
public:
	PatternRequirement();
	int addRequirement(Requirement& r);
};
*/

class Baseword {
public:
	std::string m_baseword;
	double m_weight; 
	Baseword(std::string&& baseword, double weight) : m_baseword{ std::move(baseword) }, m_weight{weight} {}
};
class NumberElement {
public:
	std::string m_number;
	double m_weight;
	NumberElement(std::string&& number, double weight);
};
class SymbolElement {
public:
	std::string m_symbol;
	double m_weight;
	SymbolElement(std::string&& symbol, double weight);
};
