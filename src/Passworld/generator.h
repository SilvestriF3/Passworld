#pragma once
#include "saveHandler.h"
#include "validator.h"
#include "globalConst.h"
#include "wordlist.h"
#include "rule.h"
#include "ImGui/applicationGui.h"
#include <array>
#include <deque>
#include <vector>
#include <chrono>
#include <string_view>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>

class SaveHandler;
class LeetPermutator;
class LeetWorker;
class GeneratorWorker;
class GeneratorConfig;
class Permutator;
class CombinerPermutator;
class CapitalPermutator;
class BasewordCutterPermutator;
class RequirementPermutator;
class PrefixPermutator;
class SuffixPermutator;
struct gV;
class Generator;
class PostGeneratorWorker;
class PreGeneratorWorker;

class ParadigmWorker {
private:
	PostGeneratorWorker* generatorWorkerPTR;
	Generator* generatorPTR;
	std::string m_paradigmPattern; //current paradigm of the worker
	std::string m_tmpPass; //tmp password being assembled
	char m_type{}; //letter of paradigm for switch
	size_t m_originalIndex{}; //index of the current m_type in paradigm
	std::vector<std::string> m_passwordStack{};  //stack of tmp passwords for the paradigm permutator
public:
	ParadigmWorker(PostGeneratorWorker* pointer, Generator* generator) : generatorWorkerPTR{ pointer }, generatorPTR{generator} {}
	~ParadigmWorker() {}
	PostGeneratorWorker* getGeneratorWorker() { return generatorWorkerPTR; }
	std::string& getParadigmPattern() { return m_paradigmPattern; }
	void setParadigmPattern(const std::string& newValue) { m_paradigmPattern = newValue; }
	std::string* getTmpPass() { return &m_tmpPass; }
	char const getType() { return m_type; }
	size_t const getOriginalIndex() { return m_originalIndex; }
	void pushPassword(std::string* password) { m_passwordStack.push_back(*password); }
	std::string popPassword() { std::string tmp{ std::move(m_passwordStack.back()) }; m_passwordStack.pop_back(); return tmp; }
	void setTmpPass(const std::string& newPass) {
		pushPassword(&m_tmpPass);
		m_tmpPass = m_tmpPass + newPass;
	}
	void setTmpPass(const char& newPass) {
		pushPassword(&m_tmpPass);
		m_tmpPass += newPass;
	}
	void resetTmpPass() { m_tmpPass = popPassword(); } //pop a password from tmp password stack
	void PassToNextElement(ParadigmWorker* pW);
	void setOriginalIndex(size_t newIndex) { m_originalIndex = newIndex; } //update index of m_type in paradigm
	size_t getWordIndex() {
		return m_paradigmPattern.find('c'); //return index of first occurence of 'c'     //implement a second basewordlist for other 'c' occurrencies?
	}
};

class GeneratorWorker { //this class is the worker of a thread
protected:
	size_t m_permutatorListIndex{ 0 }; //index of the permutator
	std::vector<std::string> m_candidateStack; //stack of password candidates 
	void const pushCandidate(const std::string& candidate) { 
		m_candidateStack.push_back(candidate); }
	std::string popCandidate() { std::string tmp{ std::move(m_candidateStack.back()) }; m_candidateStack.pop_back(); return tmp; }
	const std::string getLastCandidate() { return m_candidateStack.back(); }
	void resetWorkerState() { m_permutatorListIndex = 0; m_candidateStack.clear(); }
public:
	GeneratorWorker() {}
	~GeneratorWorker() {}
	
};
 
class PreGeneratorWorker : public GeneratorWorker {
private:
	std::unordered_set<std::string>* m_outThreadVector{}; //not the best for performance
	size_t m_permutatorListIndex{ 0 };
public:
	PreGeneratorWorker(std::unordered_set<std::string>* list) : m_outThreadVector{list} {}
	~PreGeneratorWorker() {}
	std::unordered_set<std::string>* getOutList() { return m_outThreadVector; }
	friend class Generator;
};

class PostGeneratorWorker : public GeneratorWorker {
private:
	std::unordered_set<std::string>* m_outputSet; //stream of the thread (permutations of all the basewords of a chunk assigned to thread)
	int localCandidateCount;
public:
	std::unordered_set<std::string>* getOutputSet() { return m_outputSet; }
	PostGeneratorWorker(std::unordered_set<std::string>* stream) : m_outputSet{ stream } {}
	~PostGeneratorWorker() {}
	friend class Generator;
};

class Generator {
private:
	bool stopGenerator; // variable to check (and set) to stop generator
	bool hasCompleted{false};
	int m_stage{ 0 };
	std::string* m_currentBaseword{ };
	int m_permutatedBasewordListIndex{}; //for gui info/status
	int m_pureBasewordListIndex{};
	unsigned int m_totalCandidateCount;
	GeneratorConfig* m_generatorConfig;
	SaveHandler* m_saveHandler{};
	std::vector<std::thread> m_threadList;
	std::vector<Permutator*> m_permutatorList;
	std::mutex mutexBasewordList;
	std::mutex mutexBasewordIndex;
	std::mutex threadJoinLock;
	int threadFinishedCount{0};
	std::unordered_set<std::string> m_permutatedBasewordSet{}; //set to avoid duplicates
	std::vector<std::string> m_permutatedBasewordList{};
	PasswordRequirement m_passwordRequirement{};
	int m_paradigmPermutatorIndex{};
	int CallNextBasewordPermutator(PreGeneratorWorker* gW);
	int CallNextPermutator(PostGeneratorWorker* gW); // function with all the permutators, recursive function

	void ParadigmLoop(ParadigmWorker* pW);
	void BuildCandidateLoop(ParadigmWorker* pW); //test function to implement
	void leetPermutatorLoop(PostGeneratorWorker* gW, LeetWorker* l);
	void capitalPermutation(PreGeneratorWorker* gW, CapitalPermutator* c);
	void keywordCombiner(PreGeneratorWorker* gW, CombinerPermutator* c);
	void basewordCutter(PreGeneratorWorker* gW, BasewordCutterPermutator* s);

	void prefixRequirementWorker(PostGeneratorWorker* gW, PrefixPermutator* c_r);
	void suffixRequirementWorker(PostGeneratorWorker* gW, SuffixPermutator* c_r);
	void indexRequirementWorker(PostGeneratorWorker* gW, RequirementPermutator* c_r);
	
	int getParadigmPermutatorIndex();

public:
	Generator(GeneratorConfig* config);
	~Generator();

	void pushPermutator(Permutator* p);
	int generatePermutatorId();
	void startGenerator(gV* gv);
	void resetGeneratorConfigState();
	void preGeneratorIntermediate(size_t wordlistIndex);
	void postGeneratorIntermediate(size_t wordlistIndex);
	std::vector<Permutator*>* const getPermutatorList();
	void deletePermutator(int id); //delete a permutator from list with ID
	GeneratorConfig* getGeneratorConfig() { return m_generatorConfig; }
	int getStage() { return m_stage; }
	void setStage(int value) { m_stage = value; }
	void setGenerationState(bool value) { stopGenerator = value; } //stop generating wordlist (is a signal to all threads)
	bool getHasCompleted() { return hasCompleted; }
	void setHasCompleted(bool value) { hasCompleted = value; }
	void waitThreadRunning() {
		threadJoinLock.lock();
		for (auto& t : m_threadList) {
			if(t.joinable())
			t.join();
		}
		threadJoinLock.unlock();
	}
	void checkNoThreadRunning();
	int getPermutatedBasewordListIndex() { return m_permutatedBasewordListIndex; }
	std::vector<std::string>* getPermutatedBasewordList() { return &m_permutatedBasewordList; } //where threads save their baseword lists

	void setCurrentBaseword(std::string& baseword) { m_currentBaseword = &baseword;
	mutexBasewordIndex.lock();
	++m_permutatedBasewordListIndex;
	mutexBasewordIndex.unlock();
	}
	std::string* getCurrentBaseword() { return m_currentBaseword; } //for gui view
	void incrementPureBasewordListIndex() { ++m_pureBasewordListIndex; }
	int getPureBasewordListIndex() { return m_pureBasewordListIndex; } //for gui view
	unsigned int getTotalCandidateCount() { return m_totalCandidateCount; }
	friend class GeneratorWorker;
	friend class LeetWorker;
	friend class ParadigmWorker;
};


