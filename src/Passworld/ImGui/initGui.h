#pragma once
#include "guiUtil.h"
#include "applicationGui.h"
#include <iostream>
#include <vector>
#include <algorithm>
struct gV;

struct imagePointers {
	ImTextureID generateButton{ 0 };
	ImTextureID trashPermutatorButton{ 0 };
	ImTextureID editPermutatorButton{ 0 };
};

struct fontPointers {
	ImFont* robotoSmall{ 0 };
	ImFont* robotoSuperSmall{ 0 };
	ImFont* robotoLarge{ 0 };
};

void loadImages(imagePointers* iP);
void loadFonts(fontPointers* fP, gV* gv);

int inputTextCallback(ImGuiInputTextCallbackData* data);
int inputTextCallback2(ImGuiInputTextCallbackData* data);
int inputParadigmCallback(ImGuiInputTextCallbackData* data);
int inputCharCallback(ImGuiInputTextCallbackData* data);

class IPermutatorWindow { //interface to permutator window(gui) objects
protected:
	IPermutatorWindow() {}
	virtual ~IPermutatorWindow() {}
};

class CombinerPermutatorWindow : public IPermutatorWindow {
public:
	CombinerPermutatorWindow() {
		std::cout << "created\n";
	}
	~CombinerPermutatorWindow() {
		std::for_each(bufferList.begin(), bufferList.end(), [](std::string* const ptr) {delete ptr; std::cout << "destroying: \n"; }); //destroy all allocated buffers
	}
	std::vector<std::string*> bufferList{ }; //list of buffers for GUI input
	std::vector<std::string> labelList{ };
	//here weight list
	bool appendCombination{ true };
	bool prependCombination{ true };

};

class CapitalPermutatorWindow : public IPermutatorWindow {
public:
	CapitalPermutatorWindow() {}
	~CapitalPermutatorWindow() {}
	bool firstCapital{ true };
	bool lastCapital{ true };
	bool firstNlastCapital{ true };
	bool allCapital{ true };
};

class LeetPermutatorWindow : public IPermutatorWindow {
public:
	LeetPermutatorWindow() {}
	~LeetPermutatorWindow() {}
	std::string aBuffer{}; //buffers for input text, comma separated chars 
	std::string eBuffer{};
	std::string oBuffer{};
	std::string iBuffer{};
	std::string bBuffer{};
	std::string sBuffer{};
	bool senseCaseA{};
	bool senseCaseE{};
	bool senseCaseO{};
	bool senseCaseI{};
	bool senseCaseB{};
	bool senseCaseS{};
	bool passDefaultCandidate{true};
};

class BasewordCutterPermutatorWindow : public IPermutatorWindow {
public:
	BasewordCutterPermutatorWindow() {}
	~BasewordCutterPermutatorWindow() {}
	bool threeCut{};
	bool fourCut{};
	bool fiveCut{};

};

class IndexRequirementWindow : public IPermutatorWindow { //not a real permutator but needed to append to the permutatorList, could change 
public:
	IndexRequirementWindow(){}
	~IndexRequirementWindow() {}
	//to implement vector of vectors for 'OR' operations on requirements
	std::vector<std::string*> payloadBufferList{};
	std::vector<std::string*> indexBufferList{};
	std::vector<std::string> labelList{};
	std::vector<std::string> secondlabelList{};
	std::vector<std::string> thirdlabelList{};
};

class PrefixRequirementWindow : public IPermutatorWindow {
public:
	PrefixRequirementWindow(){}
	~PrefixRequirementWindow(){}
	std::vector<std::string*> bufferList{ }; //list of buffers for GUI input
	std::vector<std::string> labelList{};
	std::vector<std::string> secondlabelList{};
};

class SuffixRequirementWindow : public IPermutatorWindow {
public:
	SuffixRequirementWindow() {}
	~SuffixRequirementWindow() {}
	std::vector<std::string*> bufferList{ }; //list of buffers for GUI input
	std::vector<std::string> labelList{};
	std::vector<std::string> secondlabelList{};
};

void showPermutatorWindowIndexRequirement(gV* gv);
void showPermutatorWindowCombiner(gV* gv);
void showPermutatorWindowCapital(gV* gv);
void showPermutatorWindowLeet(gV* gv);
void showPermutatorWindowCutter(gV* gv);
void showPermutatorWindowPrefixRequirement(gV* gv);
void showPermutatorWindowSuffixRequirement(gV* gv);