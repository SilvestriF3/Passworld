#include "initGui.h"
#include "guiUtil.h"
#include "resource.h"

void loadImages(imagePointers* iP) {
	int my_image_width = 0;
	int my_image_height = 0;
	iP->generateButton = 0;
	bool ret;
	ret = LoadTextureFromMemory(generateButton_png, generateButton_png_len, (GLuint*)&iP->generateButton, &my_image_width, &my_image_height);
	iP->editPermutatorButton = 0;
	ret = LoadTextureFromMemory(editPermutatorButton_png, editPermutatorButton_png_len, (GLuint*)&iP->editPermutatorButton, &my_image_width, &my_image_height);
	iP->trashPermutatorButton = 0;
	ret = LoadTextureFromMemory(trashPermutatorButton_png, trashPermutatorButton_png_len, (GLuint*)&iP->trashPermutatorButton, &my_image_width, &my_image_height);
}

void loadFonts(fontPointers* fP, gV* gv) {
	ImFontConfig dconf;
	dconf.FontDataOwnedByAtlas = false;
	fP->robotoSmall = 0;
	fP->robotoSmall = gv->io.Fonts->AddFontFromMemoryTTF(Roboto_Medium_ttf, Roboto_Medium_ttf_len, 16.0f, &dconf);
	fP->robotoLarge = 0;
	fP->robotoLarge = gv->io.Fonts->AddFontFromMemoryTTF(Roboto_Medium_ttf, Roboto_Medium_ttf_len, 26.0f, &dconf);
	fP->robotoSuperSmall = 0;
	fP->robotoSuperSmall = gv->io.Fonts->AddFontFromMemoryTTF(Roboto_Medium_ttf, Roboto_Medium_ttf_len, 11.0f, &dconf);
	gv->io.Fonts->AddFontDefault();
}

int inputTextCallback(ImGuiInputTextCallbackData* data) {
	if (data->CursorPos >= 1) {
		if (data->BufTextLen > 1) { //buffer max size is 1 (allow only a char)
			data->DeleteChars(data->CursorPos - 1, 1);
		}
		if (data->Buf[data->CursorPos - 1] < 0 || data->Buf[data->CursorPos - 1] > 255) { //allow only ascii
			data->DeleteChars(data->CursorPos - 1, 1);
		}
	}
	return 0;
}

int inputTextCallback2(ImGuiInputTextCallbackData* data) {
	if (!std::isdigit((unsigned char)data->EventChar)) {
		return 1;
	}
	return 0;
}

int inputParadigmCallback(ImGuiInputTextCallbackData* data) {
	unsigned char myChar{ (unsigned char)data->EventChar };
	if (myChar == ' ' || myChar == ',' || myChar == 'c' || myChar == 'n' || myChar == 's') {
		return 0;
	}
	return 1;
}

int inputCharCallback(ImGuiInputTextCallbackData* data) {
	if (data->CursorPos >= 1) {
		if (data->Buf[data->CursorPos - 1] < 0 || data->Buf[data->CursorPos - 1] > 255) { //allow only ascii
			data->DeleteChars(data->CursorPos - 1, 1);
		}
		if (data->Buf[data->CursorPos - 1] < 0 || data->Buf[data->CursorPos - 1] > 255) { //allow only ascii
			data->DeleteChars(data->CursorPos - 1, 1);
		}
	}
	return 0;
}

void showPermutatorWindowCombiner(gV* gv) { //combiner window
	ImGui::OpenPopup("Combiner Permutator");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetMainViewport()->Size.y - 100));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Combiner Permutator", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	CombinerPermutatorWindow* combiner{ static_cast<CombinerPermutatorWindow*>(gv->tmpPermutatorPtr) };
	if (combiner->bufferList.empty()) { //ensure there is always a buffer in the window
		combiner->bufferList.push_back(new std::string); //push a new buffer
		combiner->labelList.push_back("##inputCombinatorDEFAULT");
	}
	ImGui::Checkbox("Append", &combiner->appendCombination);
	ImGui::SameLine();
	ImGui::Checkbox("Prepend", &combiner->prependCombination);
	if (ImGui::GetWindowSize().y > 150) { //check if there is enough space in the window to avoid overlap
		ImGui::Text("Words to combine:");
		ImGui::BeginChild("ChildCombinerWindow", ImVec2(ImGui::GetWindowWidth() - 15, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 35), true, ImGuiConfigFlags_NavEnableKeyboard);
		for (int i = 0; i < combiner->bufferList.size(); i++) { //render all input boxes
			if (ImGui::InputText(combiner->labelList[i].c_str(), combiner->bufferList[i], ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback)) {
				if (i != combiner->bufferList.size() - 1) {
					ImGui::SetKeyboardFocusHere(1);
				}
				else {
					ImGui::SetKeyboardFocusHere(-1);
				}
			}
			ImGui::SameLine();
			std::string buttonLabel{ "##trashButton" + std::to_string(i) };
			if (ImGui::ImageButton(buttonLabel.c_str(), gv->iP->trashPermutatorButton, ImVec2(20, 20)) && combiner->bufferList.size() > 1) { //ensure at least 1 buffer is present
				delete combiner->bufferList[i];
				combiner->bufferList.erase(combiner->bufferList.begin() + i);
			}
		}
		ImGui::EndChild();
	}
	if (!combiner->bufferList.empty() && !(combiner->bufferList.back()->empty())) { //if the last buffer from bufferlist is NOT empty, add a new buffer to the list
		combiner->bufferList.push_back(new std::string); //push a new buffer
		combiner->labelList.push_back("##inputCombinator" + std::to_string(combiner->bufferList.size()));
	}
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->combine_permutator_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<CombinerPermutatorWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save") && combiner->bufferList.size() > 1) { //ensure when saving that there is at least a buffer filled
		gv->combine_permutator_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			CombinerPermutator* p{ new CombinerPermutator(KEYWORDCOMBO_PERMUTATOR, 100, old_permutator_id, combiner->appendCombination, combiner->prependCombination, "Combiner Permutator") };
			for (int i = 0; i < combiner->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				p->pushCombineWord(combiner->bufferList[i]);
			}
			gv->generator->pushPermutator(p);
		}
		else {
			CombinerPermutator* p{ new CombinerPermutator(KEYWORDCOMBO_PERMUTATOR, 100, gv->generator->generatePermutatorId(), combiner->appendCombination, combiner->prependCombination, "Combiner Permutator") };
			for (int i = 0; i < combiner->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				if (!combiner->bufferList[i]->empty())
					p->pushCombineWord(combiner->bufferList[i]); //check if buffer is not empty, then push combine word
			}
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<CombinerPermutatorWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowCapital(gV* gv) {
	ImGui::OpenPopup("Capital");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Capital", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	CapitalPermutatorWindow* capital{ static_cast<CapitalPermutatorWindow*>(gv->tmpPermutatorPtr) };
	ImGui::Text("Options:");
	ImGui::Checkbox("Just first letter", &capital->firstCapital);
	ImGui::Checkbox("Just last letter", &capital->lastCapital);
	ImGui::Checkbox("Both first & last letter", &capital->firstNlastCapital);
	ImGui::Checkbox("All letters", &capital->allCapital);

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->capital_permutator_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<CapitalPermutatorWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		gv->capital_permutator_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			CapitalPermutator* p{ new CapitalPermutator(UPPERCASE_PERMUTATOR, 100, old_permutator_id, "Capital Permutator", capital->firstCapital, capital->lastCapital, capital->allCapital, capital->firstNlastCapital) };
			gv->generator->pushPermutator(p);
		}
		else {
			CapitalPermutator* p{ new CapitalPermutator(UPPERCASE_PERMUTATOR, 100, gv->generator->generatePermutatorId(), "Capital Permutator", capital->firstCapital, capital->lastCapital, capital->allCapital, capital->firstNlastCapital) };
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<CapitalPermutatorWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowLeet(gV* gv) {
	ImGui::OpenPopup("Leet Permutator");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Leet Permutator", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	LeetPermutatorWindow* leet{ static_cast<LeetPermutatorWindow*>(gv->tmpPermutatorPtr) };
	if (ImGui::GetContentRegionAvail().y > 50) {
		ImGui::BeginChild("LeetChild", ImVec2(-1, ImGui::GetContentRegionAvail().y - 40));
		ImGui::BeginColumns("leet columns", 2, ImGuiOldColumnFlags_NoBorder | ImGuiOldColumnFlags_NoResize);
		ImGui::Text("Lists of chars:");
		ImGui::PushFont(gv->fP->robotoSuperSmall);
		ImGui::Text("(comma-separated chars)");
		ImGui::PopFont();
		ImGui::NextColumn();
		ImGui::Text("Permutate capitals:");
		ImGui::EndColumns();
		ImGui::Text("'a':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'a'", &leet->aBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback))  ImGui::SetKeyboardFocusHere(1);//change labels
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##aCase", &leet->senseCaseA);
		ImGui::Text("'e':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'e'", &leet->eBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback) ) ImGui::SetKeyboardFocusHere(1);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##eCase", &leet->senseCaseE);
		//ImGui::SetCursorPosX(25);
		ImGui::Text(" 'i':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'i' ", &leet->iBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback) ) ImGui::SetKeyboardFocusHere(1);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##iCase", &leet->senseCaseI);
		ImGui::Text("'o':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'o'", &leet->oBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback) ) ImGui::SetKeyboardFocusHere(1);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##oCase", &leet->senseCaseO);
		ImGui::Text("'b':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'b'", &leet->bBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback)) ImGui::SetKeyboardFocusHere(1);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##bCase", &leet->senseCaseB);
		ImGui::Text("'s':");
		ImGui::SameLine();
		ImGui::SetCursorPosX(25);
		if (ImGui::InputText("##'s'", &leet->sBuffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback) ) ImGui::SetKeyboardFocusHere(2); //change focus
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
		ImGui::Checkbox("##sCase", &leet->senseCaseS);
		ImGui::Separator();
		ImGui::Text("Default candidate: ");
		ImGui::SameLine();
		ImGui::Checkbox("##defaultCandidate", &leet->passDefaultCandidate);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::SetTooltip("If checked:\nThe various chars are passed along, first non-permutated and then permutated;\nSo no need to add them to the lists.");
		}
		ImGui::EndChild(); //end LeetChild
	}
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->leet_permutator_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<LeetPermutatorWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		gv->leet_permutator_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			LeetConfig* config{ new LeetConfig };
			LeetPermutator* p{ new LeetPermutator(LEET_PERMUTATOR, 100, old_permutator_id, "Leet Permutator", config, gv->generator) };
			p->fillConfig(leet);
			gv->generator->pushPermutator(p);
		}
		else {
			LeetConfig* config{ new LeetConfig };
			LeetPermutator* p{ new LeetPermutator(LEET_PERMUTATOR, 100, gv->generator->generatePermutatorId(), "Leet Permutator", config, gv->generator) };
			//here fill the config
			p->fillConfig(leet);
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<LeetPermutatorWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowCutter(gV* gv) {
	ImGui::OpenPopup("Baseword Cutter Permutator");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(300, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Baseword Cutter Permutator", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	BasewordCutterPermutatorWindow* cutter{ static_cast<BasewordCutterPermutatorWindow*>(gv->tmpPermutatorPtr) };

	ImGui::Checkbox("Cut to 3 char", &cutter->threeCut);
	ImGui::Checkbox("Cut to 4 char", &cutter->fourCut);
	ImGui::Checkbox("Cut to 5 char", &cutter->fiveCut);

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->cutter_permutator_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<BasewordCutterPermutatorWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		gv->cutter_permutator_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			BasewordCutterPermutator* p{ new BasewordCutterPermutator(BASEWORDCUTTER_PERMUTATOR, 100, old_permutator_id, "Baseword Cutter Permutator", cutter->threeCut, cutter->fourCut, cutter->fiveCut) };
			gv->generator->pushPermutator(p);
		}
		else {
			BasewordCutterPermutator* p{ new BasewordCutterPermutator(BASEWORDCUTTER_PERMUTATOR, 100, gv->generator->generatePermutatorId(), "Baseword Cutter Permutator", cutter->threeCut, cutter->fourCut, cutter->fiveCut) };
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<BasewordCutterPermutatorWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowIndexRequirement(gV* gv) {
	ImGui::OpenPopup("Index Rule");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(500, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Index Rule", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	IndexRequirementWindow* req{ static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr) };
	if (req->indexBufferList.empty()) { //ensure there is always a buffer in the window
		req->indexBufferList.push_back(new std::string); //push a new buffer
		req->payloadBufferList.push_back(new std::string); //push a new buffer
		req->labelList.push_back("##inputIndexRequirementIDEFAULT");
		req->secondlabelList.push_back("##inputIndexRequirementPDEFAULT");
		req->thirdlabelList.push_back("##inputIndexRequirementTDEFAULT");
	}
	ImGui::Text("Options:");
	ImGui::BeginChild("IndexRequirementChild", ImVec2(ImGui::GetWindowWidth() - 15, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 35), true, ImGuiConfigFlags_NavEnableKeyboard);
	ImGui::PushFont(gv->fP->robotoSuperSmall);
	ImGui::Text("Char position (index):");
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3 + 20);
	ImGui::Text("Single char to enforce (payload):");
	ImGui::PopFont();
	for (int i = 0; i < req->indexBufferList.size(); i++) {
		ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3);
		if (ImGui::InputText(req->labelList[i].c_str(), req->indexBufferList[i], ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, inputTextCallback2)) {
				ImGui::SetKeyboardFocusHere();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3);
		if (ImGui::InputText(req->secondlabelList[i].c_str(), req->payloadBufferList[i], ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputTextCallback)) { //add a callback for buffer resizing (to cancel the resize, so buffer size is always 1, for 1 char only)
			if (i != req->indexBufferList.size() - 1) {
				ImGui::SetKeyboardFocusHere(1);
			}
			else {
				ImGui::SetKeyboardFocusHere(-1);
			}
		}
		ImGui::PopItemWidth();
		//button to delete here
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() -40 - ImGui::GetStyle().ScrollbarSize);
		if (ImGui::ImageButton(req->thirdlabelList[i].c_str(), gv->iP->trashPermutatorButton, ImVec2(20, 20)) && req->indexBufferList.size() > 1) { //ensure at least 1 buffer is present
			delete req->indexBufferList[i];
			delete req->payloadBufferList[i];
			req->indexBufferList.erase(req->indexBufferList.begin() + i);
			req->payloadBufferList.erase(req->payloadBufferList.begin() + i);
		}
	}
	ImGui::EndChild();

	if (!req->payloadBufferList.empty() && !req->payloadBufferList.back()->empty()) { //if the last buffer from bufferlist is NOT empty, add a new buffer to the list
		req->indexBufferList.push_back(new std::string); //push a new buffer
		req->payloadBufferList.push_back(new std::string); //push a new buffer
		req->labelList.push_back("##inputIndexRequirementI" + std::to_string(req->indexBufferList.size()));
		req->secondlabelList.push_back("##inputIndexRequirementP" + std::to_string(req->indexBufferList.size()));
		req->thirdlabelList.push_back("##inputIndexRequirementT" + std::to_string(req->indexBufferList.size()));
	}

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->index_requirement_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		gv->index_requirement_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			RequirementPermutator* p{ new RequirementPermutator(INDEX_REQUIREMENT, 100, old_permutator_id, "Index Rule") };
			//here save buffers
			for (int i = 0; i < req->indexBufferList.size(); i++) { //save the content of the buffers to actual permutator object
				int n = 0;
				if (!req->indexBufferList[i]->empty() && !req->payloadBufferList[i]->empty() && sscanf(req->indexBufferList[i]->c_str(), "%d", &n) == 1) {
					p->pushRequirement(new IndexRequirement(req->payloadBufferList[i]->c_str()[0], n, 100));
				}
			}
			gv->generator->pushPermutator(p);
		}
		else {
			RequirementPermutator* p{ new RequirementPermutator(INDEX_REQUIREMENT, 100, gv->generator->generatePermutatorId(), "Index Rule") };
			//here save buffers
			for (int i = 0; i < req->indexBufferList.size(); i++) { //save the content of the buffers to actual permutator object
				int n = 0;
				if (!req->indexBufferList[i]->empty() && !req->payloadBufferList[i]->empty() && req->indexBufferList[i]->size() < 4 && sscanf(req->indexBufferList[i]->c_str(), "%d", &n) == 1) {
					p->pushRequirement(new IndexRequirement(req->payloadBufferList[i]->c_str()[0], n, 100));
				}
			}
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowPrefixRequirement(gV* gv) {
	ImGui::OpenPopup("Prefix Rule");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(500, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Prefix Rule", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	PrefixRequirementWindow* pre{ static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr) };
	if (pre->bufferList.empty()) { //ensure there is always a buffer in the window
		pre->bufferList.push_back(new std::string); //push a new buffer
		pre->labelList.push_back("##inputPrefixDEFAULT");
		pre->secondlabelList.push_back("##inputTrashDEFAULT");
	}
	ImGui::Text("Prefixes:");
	ImGui::BeginChild("PrefixRequirementChild", ImVec2(ImGui::GetWindowWidth() - 15, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 35), true, ImGuiConfigFlags_NavEnableKeyboard);
	for (int i = 0; i < pre->bufferList.size(); i++) {
		if (ImGui::InputText(pre->labelList[i].c_str(), pre->bufferList[i], ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback)) { //add a callback for buffer resizing (to cancel the resize, so buffer size is always 1, for 1 char only)
			if (i != pre->bufferList.size() - 1) {
				ImGui::SetKeyboardFocusHere(1);
			}
			else {
				ImGui::SetKeyboardFocusHere(-1);
			}
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40 - ImGui::GetStyle().ScrollbarSize);
		if (ImGui::ImageButton(pre->secondlabelList[i].c_str(), gv->iP->trashPermutatorButton, ImVec2(20, 20)) && pre->bufferList.size() > 1) { //ensure at least 1 buffer is present
			delete pre->bufferList[i];
			pre->bufferList.erase(pre->bufferList.begin() + i);
		}
	}
	ImGui::EndChild();

	if (!pre->bufferList.empty() && !pre->bufferList.back()->empty()) { //if the last buffer from bufferlist is NOT empty, add a new buffer to the list
		pre->bufferList.push_back(new std::string); //push a new buffer
		pre->labelList.push_back("##inputPrefix" + std::to_string(pre->bufferList.size()));
		pre->secondlabelList.push_back("##inputTrash" + std::to_string(pre->bufferList.size()));
	}


	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->prefix_requirement_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save") && pre->bufferList.size() > 1) {
		gv->prefix_requirement_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			PrefixPermutator* p{ new PrefixPermutator(PREFIX_REQUIREMENT, 100, old_permutator_id, "Prefix Rule") };
			//here save buffers
			for (int i = 0; i < pre->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				if (!pre->bufferList[i]->empty())
				p->pushPrefix(new PrefixRequirement(pre->bufferList[i]->c_str(), 100));
			}
			gv->generator->pushPermutator(p);
		}
		else {
			PrefixPermutator* p{ new PrefixPermutator(PREFIX_REQUIREMENT, 100, gv->generator->generatePermutatorId(), "Prefix Rule") };
			//here save buffers
			for (int i = 0; i < pre->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				if (!pre->bufferList[i]->empty())
					p->pushPrefix(new PrefixRequirement(pre->bufferList[i]->c_str(), 100));
			}
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}

void showPermutatorWindowSuffixRequirement(gV* gv) {
	ImGui::OpenPopup("Suffix Rule");
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(-1, -1));
	ImGui::SetNextWindowSize(ImVec2(500, ImGui::GetMainViewport()->Size.y - 300));
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->GetCenter().x, 50), 0, ImVec2(0.5f, 0.0f));
	ImGui::BeginPopupModal("Suffix Rule", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	SuffixRequirementWindow* suf{ static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr) };
	if (suf->bufferList.empty()) { //ensure there is always a buffer in the window
		suf->bufferList.push_back(new std::string); //push a new buffer
		suf->labelList.push_back("##inputSuffixDEFAULT");
		suf->secondlabelList.push_back("##inputTrashDEFAULT");
	}
	ImGui::Text("Suffixes:");
	ImGui::BeginChild("SuffixRequirementChild", ImVec2(ImGui::GetWindowWidth() - 15, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 35), true, ImGuiConfigFlags_NavEnableKeyboard);
	for (int i = 0; i < suf->bufferList.size(); i++) {
		if (ImGui::InputText(suf->labelList[i].c_str(), suf->bufferList[i], ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit, inputCharCallback)) { //add a callback for buffer resizing (to cancel the resize, so buffer size is always 1, for 1 char only)
			if (i != suf->bufferList.size() - 1) {
				ImGui::SetKeyboardFocusHere(1);
			}
			else {
				ImGui::SetKeyboardFocusHere(-1);
			}
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40 - ImGui::GetStyle().ScrollbarSize);
		if (ImGui::ImageButton(suf->secondlabelList[i].c_str(), gv->iP->trashPermutatorButton, ImVec2(20, 20)) && suf->bufferList.size() > 1) { //ensure at least 1 buffer is present
			delete suf->bufferList[i];
			suf->bufferList.erase(suf->bufferList.begin() + i);
		}
	}
	ImGui::EndChild();

	if (!suf->bufferList.empty() && !suf->bufferList.back()->empty()) { //if the last buffer from bufferlist is NOT empty, add a new buffer to the list
		suf->bufferList.push_back(new std::string); //push a new buffer
		suf->labelList.push_back("##inputPrefix" + std::to_string(suf->bufferList.size()));
		suf->secondlabelList.push_back("##inputTrash" + std::to_string(suf->bufferList.size()));
	}


	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 105, ImGui::GetWindowHeight() - 30));
	if (ImGui::Button("Cancel")) {
		gv->suffix_requirement_window = false;
		gv->permutatorBeingEdited = false;
		delete static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		gv->suffix_requirement_window = false; //hide the window
		//here save the object
		if (gv->permutatorBeingEdited) {
			int old_permutator_id{ gv->modifiedPermutatorId };
			gv->generator->deletePermutator(old_permutator_id);
			SuffixPermutator* p{ new SuffixPermutator(SUFFIX_REQUIREMENT, 100, old_permutator_id, "Suffix Rule") };
			//here save buffers
			for (int i = 0; i < suf->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				if (!suf->bufferList[i]->empty()) {
					p->pushSuffix(new SuffixRequirement(suf->bufferList[i]->c_str(), 100));
				}
			}
			gv->generator->pushPermutator(p);
		}
		else {
			SuffixPermutator* p{ new SuffixPermutator(SUFFIX_REQUIREMENT, 100, gv->generator->generatePermutatorId(), "Suffix Rule") };
			//here save buffers
			for (int i = 0; i < suf->bufferList.size(); i++) { //save the content of the buffers to actual permutator object
				if (!suf->bufferList[i]->empty()) {
					p->pushSuffix(new SuffixRequirement(suf->bufferList[i]->c_str(), 100));
				}
			}
			gv->generator->pushPermutator(p);
		}
		gv->permutatorBeingEdited = false;
		delete static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr);//cancel the window object after save
	}
	ImGui::EndPopup(); //end popup modal
}
