#include "rule.h"


Permutator::Permutator(int type, double weight, int id, std::string displayName) : m_weight{ weight }, m_type{ type }, m_id{ id }, m_displayName{displayName} {}
double const Permutator::getWeight() { return m_weight; }
int const Permutator::getType() { return m_type; }
int const Permutator::getId() { return m_id; }
std::string& Permutator::getDisplayName() { return m_displayName; }

PrefixRequirement::PrefixRequirement(std::string payload, double weight) : m_payload{ payload }, m_weight{ weight } {}
SuffixRequirement::SuffixRequirement(std::string payload, double weight) : m_payload{ payload }, m_weight{ weight } {}
IndexRequirement::IndexRequirement(char payload, int index, double weight) : m_payload{ payload }, m_weight{ weight }, m_index{ index } {}

PasswordRequirement::PasswordRequirement(int minLength, int maxLenght, bool uppercase, bool number, bool specialSymbol, std::vector<char> symbolList/*, std::vector<CustomPasswordRequirement*> customRequirementList*/, bool validatePrefix, bool validatePostfix) : m_minLength{ minLength }, m_maxLength{ maxLenght },
m_uppercase{ uppercase }, m_number{ number }, m_specialSymbol{ specialSymbol }, m_symbolList{ symbolList }/*, m_customRequirementList{ customRequirementList }*/, m_validatePrefix{ validatePrefix }, m_validatePostfix{ validatePostfix } {}
int const PasswordRequirement::getMinLength() { return m_minLength; }
int const PasswordRequirement::getMaxLength() { return m_maxLength; }
bool const PasswordRequirement::getRequireUppercase() { return m_uppercase; }
bool const PasswordRequirement::getRequireNumber() { return m_number; }
bool const PasswordRequirement::getRequireSymbol() { return m_specialSymbol; }
bool const PasswordRequirement::getValidatePrefix() { return m_validatePrefix; }
bool const PasswordRequirement::getValidatePostfix() { return m_validatePostfix; }
std::vector<char>* const PasswordRequirement::getSymbolList() { return &m_symbolList; }
void PasswordRequirement::setMinLength(int value) { m_minLength = value; } //here could add some checks on input 
void PasswordRequirement::setMaxLenght(int value) { m_maxLength = value; }
void PasswordRequirement::setRequireUppercase(bool value) { m_uppercase = value; }
void PasswordRequirement::setRequireSymbol(bool value) { m_specialSymbol = value; }
void PasswordRequirement::setRequireNumber(bool value) { m_number = value; }
void PasswordRequirement::setSymbolList(std::vector<char>&& list) { m_symbolList = std::move(list); }
//std::vector<CustomPasswordRequirement*> const PasswordRequirement::getCustomRequirementList() { return m_customRequirementList; }

//--------------------------------------------------------------------------------------------------
Paradigm::Paradigm(std::string&& pattern, double weight) : m_pattern{ std::move(pattern) }, m_weight{ weight } {}
const std::string& Paradigm::getPattern() { return m_pattern; }
double Paradigm::getWeight() { return m_weight; }

NumberElement::NumberElement(std::string&& number, double weight) : m_number{ std::move(number) }, m_weight{ weight } {}
SymbolElement::SymbolElement(std::string&& symbol, double weight) : m_symbol{ std::move(symbol) }, m_weight{ weight } {}

int resolvePermutatorPriority(int type) {
	switch (type) {
	case KEYWORDCOMBO_PERMUTATOR:
		return -1;
	case PARADIGM_PERMUTATOR:
		return 0;
	case UPPERCASE_PERMUTATOR:
		return -3;
	case LEET_PERMUTATOR:
		return 9;
	case BASEWORDCUTTER_PERMUTATOR:
		return -5;
	case INDEX_REQUIREMENT:
		return 16;
	case PREFIX_REQUIREMENT:
		return 15;
	case SUFFIX_REQUIREMENT:
		return 14;
	default:
		assert(false && "no priority setted for permutator type");
	    return 0;
	}
}