#include "validator.h"

//return false = password not valid
//return true = password is valid
bool validatePassword(const std::string* password, PasswordRequirement& r) {
	int passLen{ static_cast<int>(password->length()) };
	if (passLen <= r.getMinLength()) {
		return false; 
	} //check if it is in the length range
	if (passLen > r.getMaxLength() && r.getMaxLength() > 0) { return false; }
	if (r.getRequireNumber()) { //check if it requires numbers
		if (!std::any_of(password->begin(), password->end(), ::isdigit)) {
			return false;
		}
	}
	if (r.getRequireUppercase()) { //check if it requires uppercase
		if (!std::any_of(password->begin(), password->end(), ::isupper)) {
			return false;
		}
	}
	if (r.getRequireSymbol()) { //check if it requires symbols
		bool valid{ false };
		for (int i = 0; i < r.getSymbolList()->size(); i++) {
			if (password->find((r.getSymbolList())->at(i)) == std::string::npos) {
				continue;
			}
			valid = true;
		}
		if (!valid) { return false; }
	}
	//if (!customValidatePassword(password, r)) { return false; }
	//SEND TO SAVE HANDLER

	return true;
}

void initPasswordRequirement(gV* gv, PasswordRequirement* r) {
	r->setMaxLenght(gv->passwordLength[1]);
	r->setMinLength(gv->passwordLength[0]);
	r->setRequireNumber(gv->requireNumber);
	r->setRequireSymbol(gv->requireSpecialCharacter);
	r->setRequireUppercase(gv->requireUppercase);
	std::vector<char>list{};
	std::string str;
	std::string tmp{ gv->allowedSpecialCharsBuffer };
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	std::stringstream ss(tmp);
	while (std::getline(ss, str, ',')) {
		list.push_back(str[0]);
	}
	r->setSymbolList(std::move(list));
}
//TODO 
//other checks

//iterate through a list of user defined rules such as prefix/postfix/fixed_char
/* REWORK FUNCTION
bool customValidatePassword(std::string& password, PasswordRequirement& r) {
	for (auto& i : r.getCustomRequirementList()) {
		const int& type{ i->getType() };
		if (type == PREFIX_TYPE && r.getValidatePrefix()) {
			if (password.starts_with(i->getPayload())) { continue; }
			else { return false; }  
		}
		else if (type == POSTFIX_TYPE && r.getValidatePostfix()) {
			if (password.ends_with(i->getPayload())) { continue; }
			else { return false; }
		}
		else if (type == INDEX_CHAR_TYPE) {  //do not require switch because if there is rule it has to be always validated
			if (i->getPayload().size() == 1 && password[i->getIndex()] == i->getPayload()[0]) { continue; }
			else { return false; }
		}
	}
	return true;
}
*/
//example I have 3 rules, each rule must be satisfied 

//i-char is a number, symbol, letter
//candidate starts with, end with, has x char at y pos (from back or front)
//candidate has maximum/minimum n numbers, symbols...
//respect the requirements: lenght, allowed symbols...
