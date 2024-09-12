#pragma once
#include <iostream>
#include <thread>
#include <deque>
#include "saveHandler.h"
#include "generator.h"
#include "rule.h"

class Generator;
class Baseword;
class NumberElement;
class Paradigm;
class SaveHandler;
class GeneratorWorker;
class PostGeneratorWorker;
class Wordlist;
class Permutator;
class LeetPermutatorWindow;

class GeneratorConfig {  // should define default value (list)
public:
	GeneratorConfig(std::vector<Baseword>& basewordList, std::vector<NumberElement>& numberList, std::vector<SymbolElement>& symbolList, std::vector<Paradigm>& paradigmList); //not used
	GeneratorConfig(){}
	std::vector<Baseword> m_basewordList;
	std::vector<NumberElement> m_numberList;
	std::vector<SymbolElement> m_symbolList;
	std::vector<Paradigm> m_paradigmList;
	int threadNumber{static_cast<int>(std::thread::hardware_concurrency()) -1};
	size_t preChunkSize{ 0 };
	size_t postChunkSize{ 0 };
	bool m_multiPermutationSwitch{ true }; //use just some words when 'c' appears more than once in a paradigm or all the words
};

class LeetConfig {
public: //(todo LeetChar with weight)
	//lists with chars to replace each
	std::vector<std::string> m_aPermutation{};
	std::vector<std::string> m_ePermutation{};
	std::vector<std::string> m_oPermutation{};
	std::vector<std::string> m_iPermutation{};
	std::vector<std::string> m_bPermutation{};
	std::vector<std::string> m_sPermutation{};
	//switch to permutate also Capitals
	bool m_senseCaseA{};
	bool m_senseCaseB{};
	bool m_senseCaseI{};
	bool m_senseCaseO{};
	bool m_senseCaseE{};
	bool m_senseCaseS{};
	bool m_passDefaultCandidate{};
	LeetConfig(){}
};

class Wordlist {
private:
	//SaveConfig* m_config;
	GeneratorWorker* m_generatorWorker;
	SaveHandler* m_saveHandler;
	//validator config here
public:
	Wordlist();

};

class CombinerPermutator : public Permutator { //maybe add an option to combine basewords or full candidates
private:
	std::vector<std::string> m_combineWordList{}; //TODO change std::string with Baseword
	bool m_appendCombination{};
	bool m_prependCombination{};
public:
	CombinerPermutator(int type, double weight, int id, bool appendCombination, bool prependCombination, std::string displayName);
	void pushCombineWord(std::string* word); //TODO change std::string with Baseword
	std::vector<std::string>* const getCombineWordList() { return &m_combineWordList; }
	bool getAppendSwitch() { return m_appendCombination; }
	bool getPrependSwitch() { return m_prependCombination; }
};

class CapitalPermutator : public Permutator {
private:
	bool m_firstCapital{};
	bool m_lastCapital{};
	bool m_allCapital{};
	bool m_firstNlastCapital{};
public:
	CapitalPermutator(int type, double weight, int id, std::string displayName, bool firstCapital, bool lastCapital, bool allCapital, bool firstNlastCapital);
	bool getFirstCapitalSwitch() { return m_firstCapital; }
	bool getLastCapitalSwitch() { return m_lastCapital; }
	bool getAllCapitalSwitch() { return m_allCapital; }
	bool getFirstNLastCapitalSwitch() { return m_firstNlastCapital; }
};

class BasewordCutterPermutator : public Permutator {
private:
	bool m_threeCut{};
	bool m_fourCut{};
	bool m_fiveCut{};
public:
	BasewordCutterPermutator(int type, double weight, int id, std::string displayName, bool threeCut, bool fourCut, bool fiveCut);
	bool getThreeCutSwitch() { return m_threeCut; }
	bool getFourCutSwitch() { return m_fourCut; }
	bool getFiveCutSwitch() { return m_fiveCut; }
};

class LeetPermutator : public Permutator { //possile feature: fully custom leet permutations
private:
	LeetConfig* m_config{};
public:
	Generator* generatorPTR{};
	LeetPermutator(int type, double weight, int id, std::string displayName, LeetConfig* config, Generator* generatorPTR);
	~LeetPermutator() { delete m_config; } //delete dynamically allocated config
	void fillConfig(LeetPermutatorWindow* l);
	LeetConfig* getConfig() { return m_config; }
};

class LeetWorker { //move tmp data members and method from LeetPermutator
private:
	int m_charIndex{ 0 };
	std::vector<std::string> m_leetStack{};
	std::string m_baseword{};
	LeetPermutator* m_leetPermutator{};
public:
	LeetWorker(LeetPermutator* permutator) : m_leetPermutator{permutator} {}
	void setCharIndex(int newIndex) { m_charIndex = newIndex; }
	void pushBaseword(std::string* baseword) { m_leetStack.push_back(*baseword); }
	std::string popBaseword() { std::string tmp{ std::move(m_leetStack.back()) }; m_leetStack.pop_back(); return tmp; }
	void updateCandidateChar(char newChar, int index) {
		pushBaseword(&m_baseword);
		m_baseword[index] = newChar;
	}
	void restorePrevCandidate() { m_baseword = popBaseword(); }
	std::string* getTmpBase() { return &m_baseword; }
	int getCharIndex() { return m_charIndex; }
	void initBaseword(const std::string_view passedCandidate) { m_baseword = passedCandidate; }
	void PassToNextChar(PostGeneratorWorker* gW, LeetWorker* l);
	LeetPermutator* getLeetPermutator() { return m_leetPermutator; }
};

class RequirementPermutator : public Permutator { //not a real permutator
private:
	std::vector<IndexRequirement*> m_requirementList{};
public:
	RequirementPermutator(int type, double weight, int id, std::string displayName) : Permutator(type, weight, id, displayName) {}
	~RequirementPermutator() {
		for (int i = 0; i < m_requirementList.size(); i++) {
			delete m_requirementList[i]; }
	}
	void pushRequirement(IndexRequirement* requirement) { 
			m_requirementList.push_back(requirement);
    }
	std::vector<IndexRequirement*>* getRequirementList() { return &m_requirementList; }
};

class PrefixPermutator : public Permutator { //not a real permutator
private:
	std::vector<PrefixRequirement*> m_prefixList{};
public:
	PrefixPermutator(int type, double weight, int id, std::string displayName) : Permutator(type, weight, id, displayName) {}
	~PrefixPermutator() {}
	void pushPrefix(PrefixRequirement* prefix) {
		m_prefixList.push_back(prefix);
	}
	std::vector<PrefixRequirement*>* const getPrefixList() { return &m_prefixList; }
};

class SuffixPermutator : public Permutator { //not a real permutator
private:
	std::vector<SuffixRequirement*> m_suffixList{};
public:
	SuffixPermutator(int type, double weight, int id, std::string displayName) : Permutator(type, weight, id, displayName) {}
	~SuffixPermutator() {}
	void pushSuffix(SuffixRequirement* suffix) {
		m_suffixList.push_back(suffix);
	}
	std::vector<SuffixRequirement*>* const getSuffixList() { return &m_suffixList; }
};

