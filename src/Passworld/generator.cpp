#include "generator.h"

//get the source of the componets (numbers, words and symbols...)
//create iterators of the components
//check priority of the components (words most priority)
//prepend/append components to the tmp password
//once all components are placed, save the iterators (calculate the permutations cap before & advance to the next baseword)
//call appendpassword
//advance the baseword iterator by 1, reset all others iterator

void Generator::ParadigmLoop(ParadigmWorker* pW) {
	//paradigm iterator for each baseword
	GeneratorConfig* g_c{ m_generatorConfig };
	for (size_t i = 0; i < g_c->m_paradigmList.size(); i++) {
		pW->setParadigmPattern(g_c->m_paradigmList[i].getPattern());
		BuildCandidateLoop(pW);
	}
}

void ParadigmWorker::PassToNextElement(ParadigmWorker* pW) {
	m_originalIndex += 1;
	generatorPTR->BuildCandidateLoop(pW);
	m_originalIndex -= 1;
}

void Generator::BuildCandidateLoop(ParadigmWorker* pW) { //TODO bad optimization
	//find the first 'c' in the paradigm, save the index, this will be replaced with the last candidate instead of permutating with the basewordList
	//if more c are found, the latters will be normally permutated TODO other options as all permutated baseword
	PostGeneratorWorker* gW{ pW->getGeneratorWorker() };
	GeneratorConfig* g_c{ m_generatorConfig };
	const std::string& paradigm{ pW->getParadigmPattern() };
	size_t wordIndex{ pW->getWordIndex() };
	size_t originalIndex{ pW->getOriginalIndex() };
	if (originalIndex <= paradigm.length()) {  //iterate through the paradigm string
		char type{ paradigm[originalIndex] };
		size_t it{ 0 };
		switch (type) {
		case 's':
			for (; it < g_c->m_symbolList.size(); it++) {
				if (originalIndex == paradigm.length() - 1) { //this is last
					gW->pushCandidate(*pW->getTmpPass() + g_c->m_symbolList[it].m_symbol); //push candidate to stack state generator //check if std::move improves performance 
					CallNextPermutator(gW);
					continue;
				}
				pW->setTmpPass(g_c->m_symbolList[it].m_symbol.c_str()); //update the password //symbol class with weight todo
				pW->PassToNextElement(pW);
				pW->resetTmpPass();
			}

			break;
		case 'n':
			for (; it < g_c->m_numberList.size(); it++) { //todo fix correct vector size in each section
				if (originalIndex == paradigm.length() - 1) { //this is last
					gW->pushCandidate(*pW->getTmpPass() + g_c->m_numberList[it].m_number.c_str()); //push candidate to stack state generator
					CallNextPermutator(gW);
					continue;
				}
				pW->setTmpPass(g_c->m_numberList[it].m_number.c_str());
				pW->PassToNextElement(pW);
				pW->resetTmpPass();
			}
			break;
		case 'c':
			if (wordIndex == originalIndex) {
				if (originalIndex == paradigm.length() - 1) { //this is last
					gW->pushCandidate(*pW->getTmpPass() + gW->getLastCandidate()); //push candidate to stack generator
					CallNextPermutator(gW);
					break;
				}
				pW->setTmpPass(gW->getLastCandidate()); //get last candidate = permutated baseword
				pW->PassToNextElement(pW);
				pW->resetTmpPass();
			}
			else {
				for (; it < m_permutatedBasewordList.size(); it++) {
					if (originalIndex == paradigm.length() - 1) { //this is last
						gW->pushCandidate(*pW->getTmpPass() + m_permutatedBasewordList[it]); //push candidate to stack state generator
						CallNextPermutator(gW);
						continue;
					}
					pW->setTmpPass(m_permutatedBasewordList[it]);
					pW->PassToNextElement(pW);
					pW->resetTmpPass();
				}
			}
			break;
		}

	}
}

//TODO add checks to control char is a letter
void Generator::capitalPermutation(PreGeneratorWorker* gW, CapitalPermutator* c) {
	std::string lastCandidate{ gW->getLastCandidate() };
	gW->pushCandidate(lastCandidate);
	CallNextBasewordPermutator(gW);
	std::string saveStr{ lastCandidate };
	if (c->getFirstCapitalSwitch()) {
		lastCandidate[0] = std::toupper(lastCandidate[0]);
		gW->pushCandidate(lastCandidate);
		CallNextBasewordPermutator(gW);
	}
	if (c->getLastCapitalSwitch()) {
		saveStr.back() = std::toupper(saveStr.back());
		gW->pushCandidate(saveStr);
		CallNextBasewordPermutator(gW);
	}
	if (c->getFirstNLastCapitalSwitch()) {
		saveStr.front() = std::toupper(saveStr.front());
		gW->pushCandidate(saveStr);
		CallNextBasewordPermutator(gW);
	}
	if (c->getAllCapitalSwitch()) {
		std::transform(saveStr.begin(), saveStr.end(), saveStr.begin(), ::toupper);
		gW->pushCandidate(saveStr);
		CallNextBasewordPermutator(gW);
	}
}

//this function combines 2 words 
void Generator::keywordCombiner(PreGeneratorWorker* gW, CombinerPermutator* c) {//TODO change std::string with Baseword
	std::string lastCandidate{ gW->getLastCandidate() };
	for (size_t i{ 0 }; i < c->getCombineWordList()->size(); ++i) { //custom list of words to combine with basewords possible (todo), raise or lower combinations
		if (lastCandidate == c->getCombineWordList()->at(i)) {
			CallNextBasewordPermutator(gW);
			continue;
		}
		if (c->getAppendSwitch()) {
			gW->pushCandidate(c->getCombineWordList()->at(i) + lastCandidate); //prepend and append combine, todo add switch checks from 'config'
			CallNextBasewordPermutator(gW);
		}
		if (c->getPrependSwitch()) {
			gW->pushCandidate(lastCandidate + c->getCombineWordList()->at(i));
			CallNextBasewordPermutator(gW);
		}
	}
}

//TODO add more customization options
void Generator::basewordCutter(PreGeneratorWorker* gW, BasewordCutterPermutator* s) { //abbreviate the keywords to 3 and 4 chars
	std::string lastCandidate{ gW->getLastCandidate() };
	gW->pushCandidate(lastCandidate);
	CallNextBasewordPermutator(gW);
	if (s->getThreeCutSwitch() && lastCandidate.size() > 3) {
		gW->pushCandidate(lastCandidate.substr(0, 3));
		CallNextBasewordPermutator(gW);
	}
	if (s->getFourCutSwitch() && lastCandidate.size() > 4) {
		gW->pushCandidate(lastCandidate.substr(0, 4));
		CallNextBasewordPermutator(gW);
	}
	if (s->getFiveCutSwitch() && lastCandidate.size() > 5) {
		gW->pushCandidate(lastCandidate.substr(0, 5));
		CallNextBasewordPermutator(gW);
	}
}

//this function replace chars with other chars 7337
//TODO more custom leet replacement
void Generator::leetPermutatorLoop(PostGeneratorWorker* gW, LeetWorker* l) { //possible optimization 
	LeetConfig* l_c{ l->getLeetPermutator()->getConfig() };
	const std::string& saveStr{ gW->getLastCandidate() };
	int charIndex{ l->getCharIndex() };
	if (charIndex == 0) {
		l->initBaseword(gW->getLastCandidate());
	}
	if (charIndex < saveStr.size()) {
		char letter{ std::tolower(saveStr[charIndex], std::locale()) };
		size_t it{ 0 };
		switch (letter) {
		case 'a':
			if (saveStr[charIndex] == 'A' && !l->getLeetPermutator()->getConfig()->m_senseCaseA) { break; } //check if should permutate capital letters
			if (l_c->m_aPermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate){ //before permutations pass unpermutated as-is candidate to next char iteration OR permutator (if last char)
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_aPermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_aPermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_aPermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		case 'e':
			if (saveStr[charIndex] == 'E' && !l->getLeetPermutator()->getConfig()->m_senseCaseE) { break; } //check if should permutate capital letters
			if (l_c->m_ePermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate) {
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_ePermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_ePermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_ePermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		case 'o':
			if (saveStr[charIndex] == 'O' && !l->getLeetPermutator()->getConfig()->m_senseCaseO) { break; } //check if should permutate capital letters
			if (l_c->m_oPermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate) {
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_oPermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_oPermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_oPermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		case 'i':
			if (saveStr[charIndex] == 'I' && !l->getLeetPermutator()->getConfig()->m_senseCaseI) { break; } //check if should permutate capital letters
			if (l_c->m_iPermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate) {
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_iPermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_iPermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_iPermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		case 'b':
			if (saveStr[charIndex] == 'B' && !l->getLeetPermutator()->getConfig()->m_senseCaseB) { break; } //check if should permutate capital letters
			if (l_c->m_bPermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate) {
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_bPermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_bPermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_bPermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		case 's':
			if (saveStr[charIndex] == 'S' && !l->getLeetPermutator()->getConfig()->m_senseCaseS) { break; } //check if should permutate capital letters
			if (l_c->m_sPermutation.size() == 0) { break; }
			if (l->getLeetPermutator()->getConfig()->m_passDefaultCandidate) {
				if (charIndex == saveStr.size() - 1) {
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					return;
				}
				l->PassToNextChar(gW, l);
			}
			for (; it < l_c->m_sPermutation.size(); ++it) {
				if (charIndex == saveStr.size() - 1) {
					l->updateCandidateChar(*l_c->m_sPermutation[it].data(), charIndex);
					gW->pushCandidate(*l->getTmpBase());
					CallNextPermutator(gW);
					l->restorePrevCandidate();
					continue;
				}
				l->updateCandidateChar(*l_c->m_sPermutation[it].data(), charIndex);  //replace letter with new char
				l->PassToNextChar(gW, l);
				l->restorePrevCandidate();
			}
			return;
		}
		//the letter cannot be permutated
		if (charIndex == saveStr.size() - 1) { //but it is last letter
			gW->pushCandidate(*l->getTmpBase());
			CallNextPermutator(gW);
			return;
		}
		else { //not last letter
			l->PassToNextChar(gW, l);
			return;
		}
	}
}
//TODO for prefix and suffix, switch to insert or replace mode 
//enforce custom password rules by principle
//apply on whole password wordlist (last step before finished wordlist)
void Generator::prefixRequirementWorker(PostGeneratorWorker* gW, PrefixPermutator* c_r) {
	std::string lastCandidate{ gW->getLastCandidate() };
	std::string saveCandidate{ lastCandidate };
	for (int i{ 0 }; i < c_r->getPrefixList()->size(); ++i) {
		gW->pushCandidate(lastCandidate.insert(0, (*c_r->getPrefixList())[i]->m_payload));
		CallNextPermutator(gW);
		lastCandidate = saveCandidate;
	}
}

void Generator::suffixRequirementWorker(PostGeneratorWorker* gW, SuffixPermutator* c_r) {
	std::string lastCandidate{ gW->getLastCandidate() };
	std::string saveCandidate{ lastCandidate };
	for (int i{ 0 }; i < c_r->getSuffixList()->size(); ++i) {
		gW->pushCandidate(lastCandidate.append((*c_r->getSuffixList())[i]->m_payload));
		CallNextPermutator(gW);
		lastCandidate = saveCandidate;
	}
}
void Generator::indexRequirementWorker(PostGeneratorWorker* gW, RequirementPermutator* c_r) { //to enforce more characters at the same time, create more instances of this in permutator list. This can enforce a list of char one by one
	std::string lastCandidate{ gW->getLastCandidate() };
	//TODO: insert whole strings instead of just chars
	//TODO: multiple requirements parallel, for each requirement list.... Ex: want to only have candidates whose 2nd char is an 'b' OR 't', both ends with 'm' (maybe global requirements are helpful)
	//for each requirement in requirement list, do the operation on the candidate
	//after all operations send it to CallNextPermutator
	for (int i{ 0 }; i < c_r->getRequirementList()->size(); ++i) { //NOTE: the last requirements override the firsts
		if (c_r->getRequirementList()->at(i)->m_index >= 0 && lastCandidate.size() > c_r->getRequirementList()->at(i)->m_index) { //check if can enforce
			lastCandidate[c_r->getRequirementList()->at(i)->m_index] = c_r->getRequirementList()->at(i)->m_payload;
		}
		if (c_r->getRequirementList()->at(i)->m_index < 0 && c_r->getRequirementList()->at(i)->m_index + (int)lastCandidate.size() >= 0) { //index from back
			lastCandidate[c_r->getRequirementList()->at(i)->m_index + (int)lastCandidate.size()] = c_r->getRequirementList()->at(i)->m_payload;
		}
	}
	gW->pushCandidate(lastCandidate);
	CallNextPermutator(gW);
}

bool paradigmIsValid(Paradigm& p) {
	if (p.getPattern().length() < 2) {
		// paradigm = 1, does it make sense? it is just the wordlist itself
	}
	//TODO additional checks on the paradigm ( check chars are only 'c', 'n', 's' ) OR standardize the input
	return true;
}

Generator::Generator(GeneratorConfig* config) : m_generatorConfig{ config } {
	m_permutatorList.push_back(new Permutator(PARADIGM_PERMUTATOR, 100.0, -1, "Paradigm Permutator")); //place holder for paradigm permutator (it is default)
}
Generator::~Generator() {
	//delete dynamic allocated permutators from permutatorList
	std::for_each(m_permutatorList.begin(), m_permutatorList.end(), [](Permutator* const ptr) {delete ptr; });
}

int Generator::CallNextBasewordPermutator(PreGeneratorWorker* gW) {
	if (stopGenerator)
	{ return 0; }
	if (m_permutatorList[gW->m_permutatorListIndex]->getType() == PARADIGM_PERMUTATOR) {
		gW->getOutList()->insert(gW->popCandidate());
	}
	else {
		gW->m_permutatorListIndex++;
		switch (m_permutatorList[gW->m_permutatorListIndex - 1]->getType()) {
		case UPPERCASE_PERMUTATOR:
			capitalPermutation(gW, static_cast<CapitalPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		case KEYWORDCOMBO_PERMUTATOR:
			keywordCombiner(gW, static_cast<CombinerPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		case BASEWORDCUTTER_PERMUTATOR:
			basewordCutter(gW, static_cast<BasewordCutterPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		}
		gW->popCandidate();
		gW->m_permutatorListIndex--;
	}
	return 0;
}

int Generator::CallNextPermutator(PostGeneratorWorker* gW) {
	//push candidate to stack is responsability of the permutator
	if (stopGenerator)
	{ return 0; }
	if (gW->m_permutatorListIndex == m_permutatorList.size()) {
		//(SEND TO VALIDATOR)
		//(this stream is of the thread, no need to protect)
		std::string candidate{ std::move(gW->popCandidate()) };
		if (validatePassword(&candidate, m_passwordRequirement)) {
			//*gW->getStreamOut() << candidate.c_str() << '\n'/* << ++counter << '\n'*/;
			gW->getOutputSet()->insert(std::move(candidate));
			++gW->localCandidateCount;
		}
	}
	else if (gW->m_permutatorListIndex < m_permutatorList.size()) {
		gW->m_permutatorListIndex++;
		switch (m_permutatorList[gW->m_permutatorListIndex - 1]->getType()) { //should pass the last candidate to the next permutator
		case LEET_PERMUTATOR:
			LeetWorker* lW;
			lW = new LeetWorker(static_cast<LeetPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			leetPermutatorLoop(gW, lW);
			delete lW;
			break;
		case PARADIGM_PERMUTATOR:
			ParadigmWorker* pW;
			pW = new ParadigmWorker(gW, this);
			ParadigmLoop(pW);
			delete pW;
			break;
		case PREFIX_REQUIREMENT:
			prefixRequirementWorker(gW, static_cast<PrefixPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		case SUFFIX_REQUIREMENT:
			suffixRequirementWorker(gW, static_cast<SuffixPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		case INDEX_REQUIREMENT:
			indexRequirementWorker(gW, static_cast<RequirementPermutator*>(m_permutatorList[gW->m_permutatorListIndex - 1]));
			break;
		}
		gW->popCandidate();
		gW->m_permutatorListIndex--;
	}
	return 0;
}


void Generator::pushPermutator(Permutator* p) {

	m_permutatorList.push_back(p);
	std::sort(m_permutatorList.begin(), m_permutatorList.end(), [](Permutator* a, Permutator* b) { //sort the list based on permutator type priority
		if (a->getType() == b->getType()) { //if same type sort by id
			return (a->getId() < b->getId());
		}
		else { //if different type, sort by priority
			int aPriority{ resolvePermutatorPriority(a->getType()) };
			int bPriority{ resolvePermutatorPriority(b->getType()) };
			return (aPriority < bPriority);
		}
		});
}
void Generator::deletePermutator(int id) { //iterate through all permutators, find the right one by id, delete it
	for (int i = 0; i < m_permutatorList.size(); i++) {
		if (m_permutatorList[i]->getId() == id) {
			delete m_permutatorList[i];
			m_permutatorList.erase(m_permutatorList.begin() + i);
			break;
		}
	}
}

int Generator::generatePermutatorId() {
	if (!m_permutatorList.empty()) {
		int max_id{};
		for (int i = 0; i < m_permutatorList.size(); i++) {
			if (m_permutatorList[i]->getId() > max_id) {
				max_id = m_permutatorList[i]->getId();
			}
		}
		return max_id + 1; //get the highest id and add 1
	}
	else {
		return 1;
	}
}
void Generator::startGenerator(gV* gv) {
	//add save of the user input basewords
	//PARSE & add numbers, symbols and paradigms...
	threadFinishedCount = 0;
	resetGeneratorConfigState();
	this->setStage(0);
	if (!gv->saveDirectory.empty()) {
		m_saveHandler = new SaveHandler(gv->saveDirectory);
	}
	else { //if save directory is empty, use default wordlist.txt
		m_saveHandler = new SaveHandler("wordlist.txt");
	}
	std::string tmp{ gv->paradigmsBuffers };
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end()); //erases white spaces from paradigms string
	std::stringstream  ss(tmp);
	std::string str;
	while (std::getline(ss, str, ',')) { //push the paradigms to config from gui buffers
		m_generatorConfig->m_paradigmList.push_back({ std::move(str) , 100 });//weight not yet implemeted
	}
	tmp = gv->basewordsBuffer;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss2(tmp);
	while (std::getline(ss2, str, ',')) { //push the basewords to config from gui buffers
		m_generatorConfig->m_basewordList.push_back({ std::move(str) , 100 });//weight not yet implemeted
	}
	//push also basewords from a wordlist file if present
	if (!gv->wordlistFile.empty()) {
		std::ifstream wordlistFile(gv->wordlistFile);
		std::string line;
		m_generatorConfig->m_basewordList.reserve(1000000);
		if (wordlistFile.is_open())
		{
			while (std::getline(wordlistFile, line))
			{
				m_generatorConfig->m_basewordList.push_back({ std::move(line) , 100 });//weight not yet implemeted
			}
			wordlistFile.close();
		}
	}
	tmp = gv->numbersBuffers;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss3(tmp);
	while (std::getline(ss3, str, ',')) {
		m_generatorConfig->m_numberList.push_back({ std::move(str) , 100 });//weight not yet implemeted
	}
	tmp = gv->symbolsBuffers;
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream  ss4(tmp);
	while (std::getline(ss4, str, ',')) {
		m_generatorConfig->m_symbolList.push_back({ std::move(str), 100 });//weight not yet implemeted
	}
	//performance options:
	m_generatorConfig->threadNumber = gv->threadNumber;
	m_generatorConfig->preChunkSize = m_generatorConfig->m_basewordList.size() > m_generatorConfig->threadNumber ? (m_generatorConfig->m_basewordList.size() - m_generatorConfig->m_basewordList.size() % m_generatorConfig->threadNumber) / m_generatorConfig->threadNumber : 1;
	m_generatorConfig->postChunkSize = 0;
	initPasswordRequirement(gv, &m_passwordRequirement);

	//////////////////// here generates the wordlist, 1st STAGE
	m_permutatedBasewordListIndex = 0;
	m_pureBasewordListIndex = 0;
	m_permutatedBasewordList.clear(); //<---- TODO PUT THIS IN A FUNCTION
	m_paradigmPermutatorIndex = getParadigmPermutatorIndex();
	m_totalCandidateCount = 0;
	this->setStage(1);
	std::vector<std::thread>& t{m_threadList};
	//thread for all chunks
	for (size_t i = 0; i < m_generatorConfig->m_basewordList.size(); i += m_generatorConfig->preChunkSize) {
		t.push_back(std::thread{ &Generator::preGeneratorIntermediate, this, i });
	}
	waitThreadRunning();
	threadFinishedCount = 0;
	t.clear();
	//convert set into vector for element access
	m_permutatedBasewordList.reserve(m_permutatedBasewordSet.size());
	m_permutatedBasewordList.assign(m_permutatedBasewordSet.begin(), m_permutatedBasewordSet.end());
	m_permutatedBasewordSet.clear();
	//here the basewords are all saved in a vector, 2nd STAGE
	this->setStage(2);
	auto t1 = std::chrono::high_resolution_clock::now();
	if (stopGenerator) {
		delete m_saveHandler;
		return;
	}
	m_generatorConfig->postChunkSize = m_permutatedBasewordList.size() > m_generatorConfig->threadNumber ? (m_permutatedBasewordList.size() - m_permutatedBasewordList.size() % m_generatorConfig->threadNumber) / m_generatorConfig->threadNumber : 1;

	for (size_t i = 0; i < m_permutatedBasewordList.size(); i += m_generatorConfig->postChunkSize) {
		t.push_back(std::thread{ &Generator::postGeneratorIntermediate, this, i });
	}

	waitThreadRunning();
	threadFinishedCount = 0;
	t.clear();
	if (stopGenerator) {
		delete m_saveHandler;
		return;
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::cout << "threads of 2nd stage took "
		<< std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count()
		<< " milliseconds\n";
	this->setStage(3);
	//here the permutations are fully done
	delete m_saveHandler;
	//possible 3rd stage for saving sequentially <here>
	hasCompleted = true;
	return;
}

void Generator::resetGeneratorConfigState() {
	m_generatorConfig->m_basewordList.clear();
	m_generatorConfig->m_paradigmList.clear();
	m_generatorConfig->m_numberList.clear();
	m_generatorConfig->m_symbolList.clear();
}

void Generator::preGeneratorIntermediate(size_t wordlistIndex) {
	size_t maxIndex{ wordlistIndex + m_generatorConfig->preChunkSize };
	if (maxIndex > m_generatorConfig->m_basewordList.size()) { //avoid out of bonds for basewordlist vector
		maxIndex = m_generatorConfig->m_basewordList.size();
	}

	std::unordered_set<std::string> baseword_threadList{}; //consider dynamic alloc to shrink stack
	baseword_threadList.reserve(100000);
	PreGeneratorWorker* gW{ new PreGeneratorWorker(&baseword_threadList) }; //create a worker for each baseword, pass the thread outstream //maybe with optimization could reuse the object instead of creating a new one
	for (size_t i = wordlistIndex; i < maxIndex; i++) { //iterate through the words of the chunk until maxIndex
		if (stopGenerator) {
			//return the thread early
			delete gW;
			++threadFinishedCount;
			return;
		}
		this->incrementPureBasewordListIndex();
		gW->resetWorkerState(); //assert better to find bugs??? stack and index should cleanup by themselves
		gW->pushCandidate(m_generatorConfig->m_basewordList[i].m_baseword);
		CallNextBasewordPermutator(gW);
	}
	delete gW;
	mutexBasewordList.lock();
	m_permutatedBasewordSet.merge(baseword_threadList); //merge thread set into general one to avoid duplicate basewords
	mutexBasewordList.unlock();
	++threadFinishedCount;
	return;
}

int Generator::getParadigmPermutatorIndex() {
	for (int i = 0; i < m_permutatorList.size(); i++) {
		if (m_permutatorList[i]->getType() == PARADIGM_PERMUTATOR) { return i; }
	}
	return -1;
}

void Generator::postGeneratorIntermediate(size_t wordlistIndex) {
	size_t maxIndex{ wordlistIndex + m_generatorConfig->postChunkSize };
	if (maxIndex > m_permutatedBasewordList.size()) { //avoid out of bonds for basewordlist vector
		maxIndex = m_permutatedBasewordList.size();
	}
	std::stringstream stream; ////to replace with a vector////
	std::unordered_set<std::string> thread_set{};
	PostGeneratorWorker* gW{ new PostGeneratorWorker(&thread_set) }; //create a worker for each baseword, pass the thread outstream //maybe with optimization could reuse the object instead of creating a new one
	for (size_t i = wordlistIndex; i < maxIndex; i++) { //iterate through the words of the chunk until maxIndex
		if (stopGenerator) {
			//return the thread early
			delete gW;
			++threadFinishedCount;
			return;
		}
		gW->resetWorkerState();
		gW->m_permutatorListIndex = m_paradigmPermutatorIndex;
		gW->pushCandidate(m_permutatedBasewordList[i]);
		CallNextPermutator(gW);
		this->setCurrentBaseword(m_permutatedBasewordList[i]);
		mutexBasewordIndex.lock();
		std::cout << m_permutatedBasewordListIndex << '\n';
		m_totalCandidateCount += gW->localCandidateCount;
		gW->localCandidateCount = 0;
		mutexBasewordIndex.unlock();
	}
	delete gW;
	for (auto& element: thread_set) { //avoid duplicates in thread list
		stream << element.c_str() << '\n';
	}
	m_saveHandler->saveThreadStream(std::move(stream));
	++threadFinishedCount;
	return;
}

std::vector<Permutator*>* const Generator::getPermutatorList() {
	return &m_permutatorList;
}

void Generator::checkNoThreadRunning() {
	while (m_threadList.size() != threadFinishedCount) 
	{ std::cout << "thread terminated\n"; }
	threadJoinLock.lock();
	for (auto& t : m_threadList) {
		if (t.joinable())
			t.join();
	}
	threadJoinLock.unlock();
	return;
}
