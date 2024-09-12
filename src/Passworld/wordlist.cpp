#include "wordlist.h"

Wordlist::Wordlist(){}

GeneratorConfig::GeneratorConfig(std::vector<Baseword>& basewordList, std::vector<NumberElement>& numberList, std::vector<SymbolElement>& symbolList, std::vector<Paradigm>& paradigmList) : m_basewordList { basewordList },
m_numberList{ numberList }, m_symbolList{ symbolList }, m_paradigmList{ paradigmList } {}

CombinerPermutator::CombinerPermutator(int type, double weight, int id, bool appendCombination, bool prependCombination, std::string displayName) : m_appendCombination{ appendCombination }, m_prependCombination{ prependCombination }, Permutator(type, weight, id, displayName) {}

void CombinerPermutator::pushCombineWord(std::string* word) {
	if (!word->empty()) {
		m_combineWordList.push_back(*word);
	}
}

CapitalPermutator::CapitalPermutator(int type, double weight, int id, std::string displayName, bool firstCapital, bool lastCapital, bool allCapital, bool firstNlastCapital) : m_firstCapital{ firstCapital }, m_lastCapital{ lastCapital }, m_firstNlastCapital{ firstNlastCapital }, m_allCapital{allCapital}, Permutator(type, weight, id, displayName) {}

BasewordCutterPermutator::BasewordCutterPermutator(int type, double weight, int id, std::string displayName, bool threeCut, bool fourCut, bool fiveCut) : m_threeCut{ threeCut }, m_fourCut{ fourCut }, m_fiveCut{fiveCut}, Permutator(type, weight, id, displayName) {}

LeetPermutator::LeetPermutator(int type, double weight, int id, std::string displayName, LeetConfig* config, Generator* generator) : m_config{config}, Permutator(type, weight, id, displayName), generatorPTR{generator} {}

void LeetPermutator::fillConfig(LeetPermutatorWindow* l) {
	std::string tmp{ l->aBuffer };                                  //parse all buffers, check if the parsed char length is 1
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss(tmp);
	std::string str;
	while (std::getline(ss, str, ',')) {
		if(str.size() == 1) //check it is only a char 
		m_config->m_aPermutation.push_back(str);
	}
	tmp = l->bBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss2(tmp);
	while (std::getline(ss2, str, ',')) {
		if (str.size() == 1) //check it is only a char
		m_config->m_bPermutation.push_back(str);
	}
	tmp = l->iBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss3(tmp);
	while (std::getline(ss3, str, ',')) {
		if (str.size() == 1) //check it is only a char
		m_config->m_iPermutation.push_back(str);
	}
	tmp = l->eBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss4(tmp);
	while (std::getline(ss4, str, ',')) {
		if (str.size() == 1) //check it is only a char
		m_config->m_ePermutation.push_back(str);
	}
	tmp = l->oBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss5(tmp);
	while (std::getline(ss5, str, ',')) {
		if (str.size() == 1) //check it is only a char
		m_config->m_oPermutation.push_back(str);
	}
	tmp = l->sBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss6(tmp);
	while (std::getline(ss6, str, ',')) {
		if (str.size() == 1) //check it is only a char
		m_config->m_sPermutation.push_back(str);
	}
	m_config->m_senseCaseA = l->senseCaseA; //save case sensitive for each letter
	m_config->m_senseCaseB = l->senseCaseB;
	m_config->m_senseCaseI = l->senseCaseI;
	m_config->m_senseCaseE = l->senseCaseE;
	m_config->m_senseCaseO = l->senseCaseO;
	m_config->m_senseCaseS = l->senseCaseS;
	m_config->m_passDefaultCandidate = l->passDefaultCandidate;
}

void LeetWorker::PassToNextChar(PostGeneratorWorker* gW, LeetWorker* l) {
	m_charIndex += 1;
	m_leetPermutator->generatorPTR->leetPermutatorLoop(gW, l);
	m_charIndex -= 1;
}