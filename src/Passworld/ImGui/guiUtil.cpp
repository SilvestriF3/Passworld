#include "applicationGui.h"
#include "guiUtil.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void AlignForWidth(float width, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

// Simple helper function to load an image into a OpenGL texture with common settings //function from dear imgui guide
bool LoadTextureFromFile(const char* filename, ImTextureID* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = (void *)image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Simple helper function to load an image into a OpenGL texture with common settings //function from dear imgui guide
bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

namespace ImGui {
    bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImVec2 size = imageSize; //image size
        if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); }
        else {
            if (size.x <= 0)          size.x = size.y;
            else if (size.y <= 0)     size.y = size.x;
            size *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
        }

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true); // text size
        const bool hasText = textSize.x > 0;

        const float innerSpacing = hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
        const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
        const ImVec2 totalSizeWithoutPadding(size.x + innerSpacing + textSize.x , size.y > textSize.y ? size.y : textSize.y);
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2); //button behaviour
        ImVec2 start(0, 0);
        start = window->DC.CursorPos + padding;
        if (size.y < textSize.y)
            start.y += (textSize.y - size.y) * .5f; //average
        ImVec2 offset{ textSize.x + innerSpacing, 0 }; //offset to place the image after text
        const ImRect image_bb(start + offset, start  + size + offset ); //image button behaviour
        //const ImRect image_bb(start , start + size);
        start = window->DC.CursorPos + padding;
        //start.x += size.x + innerSpacing;
        //start.x += innerSpacing;
        if (size.y > textSize.y)
            start.y += (size.y - textSize.y) * .5f;
        ItemSize(bb);
        if (!ItemAdd(bb, id))
            return false;

        bool hovered = false, held = false;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held);

        // Render
        const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
        if (bg_col.w > 0.0f)
            window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col)); //background

        if (textSize.x > 0) ImGui::RenderText(start, label); //write text
        window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col)); //write image

        
        return pressed;
    }
} // namespace ImGui

void addPermutatorWindow(gV* gv, int i) {
    //this function draws a permutator window
    //parameters: permutatorID, name
    //permutatorID used for buttons, name just for display
    if (gv->generator->getPermutatorList()->at(i)->getType() == PARADIGM_PERMUTATOR) { return; } //do not display paradigm permutator because it is a place holder (it is default)
    ImGui::BeginGroup();
    ImGui::PushFont(gv->fP->robotoLarge);
    ImGui::Spacing();
    ImGui::TextUnformatted(gv->generator->getPermutatorList()->at(i)->getDisplayName().c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 80);
    std::string editLabel{ "editButton" + std::to_string(gv->generator->getPermutatorList()->at(i)->getId())};
    //std::cout << editLabel << '\n';
    if (ImGui::ImageButton(editLabel.c_str(), gv->iP->editPermutatorButton, ImVec2(20, 20))) {
        gv->permutatorButtonPressedId = gv->generator->getPermutatorList()->at(i)->getId();
        std::cout << gv->permutatorButtonPressedId;
        //edit object function here
        std::cout << gv->generator->getPermutatorList()->at(i)->getId() << '\n';
        editPermutator(gv, i);
    }
    ImGui::SameLine(ImGui::GetWindowWidth() - 40);
    std::string trashLabel{ "trashButton" + std::to_string(gv->generator->getPermutatorList()->at(i)->getId()) };
    if (ImGui::ImageButton(trashLabel.c_str(), gv->iP->trashPermutatorButton, ImVec2(20, 20))) {
        gv->permutatorButtonPressedId = gv->generator->getPermutatorList()->at(i)->getId();
        std::cout << gv->permutatorButtonPressedId;
        gv->generator->deletePermutator(gv->generator->getPermutatorList()->at(i)->getId());
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::PopFont();
    ImGui::EndGroup();
}

void editPermutator(gV* gv, int id) {
    //gv->tmpPermutatorPtr = static_cast<IPermutatorWindow>(gv->generator->getPermutatorList()->at(i));
    gv->permutatorBeingEdited = true;
    switch (gv->generator->getPermutatorList()->at(id)->getType()) {
    case KEYWORDCOMBO_PERMUTATOR:
        CombinerPermutator* currentCombiner;
        CombinerPermutatorWindow* copiedCombiner;
        currentCombiner = static_cast<CombinerPermutator*>(gv->generator->getPermutatorList()->at(id));
        std::cout << "Called edit on permutator with id:" << gv->generator->getPermutatorList()->at(id)->getId() << '\n';
        copiedCombiner = new CombinerPermutatorWindow;
        for (int i = 0; i < currentCombiner->getCombineWordList()->size(); i++) { //move this to a method of the class //copy the words from combiner permutator to the newly created buffers of the permutator window
            copiedCombiner->bufferList.push_back(new std::string{ currentCombiner->getCombineWordList()->at(i) });
            copiedCombiner->labelList.push_back("##inputCombinator" + std::to_string(i));
        }
        copiedCombiner->appendCombination = currentCombiner->getAppendSwitch();
        copiedCombiner->prependCombination = currentCombiner->getPrependSwitch();

        gv->tmpPermutatorPtr = copiedCombiner;
        gv->combine_permutator_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        showPermutatorWindowCombiner(gv);
        break;
    case UPPERCASE_PERMUTATOR:
        CapitalPermutator* currentCapital;
        CapitalPermutatorWindow* copiedCapital;
        currentCapital = static_cast<CapitalPermutator*>(gv->generator->getPermutatorList()->at(id));
        std::cout << "Called edit on permutator with id:" << gv->generator->getPermutatorList()->at(id)->getId() << '\n';
        copiedCapital = new CapitalPermutatorWindow;
        copiedCapital->firstCapital = currentCapital->getFirstCapitalSwitch();
        copiedCapital->lastCapital = currentCapital->getLastCapitalSwitch();
        copiedCapital->allCapital = currentCapital->getAllCapitalSwitch();
        copiedCapital->firstNlastCapital = currentCapital->getFirstNLastCapitalSwitch();

        gv->tmpPermutatorPtr = copiedCapital;
        gv->capital_permutator_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    case LEET_PERMUTATOR:
        LeetPermutator* currentLeet;
        LeetPermutatorWindow* copiedLeet;
        currentLeet = static_cast<LeetPermutator*>(gv->generator->getPermutatorList()->at(id));
        copiedLeet = new LeetPermutatorWindow;
        //better copy and data handling is possible...
        for (int i = 0; i < currentLeet->getConfig()->m_aPermutation.size(); i++) {
            copiedLeet->aBuffer += currentLeet->getConfig()->m_aPermutation[i] + ", ";
        }
        for (int i = 0; i < currentLeet->getConfig()->m_bPermutation.size(); i++) {
            copiedLeet->bBuffer += currentLeet->getConfig()->m_bPermutation[i] + ", ";
        }
        for (int i = 0; i < currentLeet->getConfig()->m_ePermutation.size(); i++) {
            copiedLeet->eBuffer += currentLeet->getConfig()->m_ePermutation[i] + ", ";
        }
        for (int i = 0; i < currentLeet->getConfig()->m_oPermutation.size(); i++) {
            copiedLeet->oBuffer += currentLeet->getConfig()->m_oPermutation[i] + ", ";
        }
        for (int i = 0; i < currentLeet->getConfig()->m_iPermutation.size(); i++) {
            copiedLeet->iBuffer += currentLeet->getConfig()->m_iPermutation[i] + ", ";
        }
        for (int i = 0; i < currentLeet->getConfig()->m_sPermutation.size(); i++) {
            copiedLeet->sBuffer += currentLeet->getConfig()->m_sPermutation[i] + ", ";
        }
        copiedLeet->senseCaseA = currentLeet->getConfig()->m_senseCaseA;
        copiedLeet->senseCaseB = currentLeet->getConfig()->m_senseCaseB;
        copiedLeet->senseCaseE = currentLeet->getConfig()->m_senseCaseE;
        copiedLeet->senseCaseO = currentLeet->getConfig()->m_senseCaseO;
        copiedLeet->senseCaseI = currentLeet->getConfig()->m_senseCaseI;
        copiedLeet->senseCaseS = currentLeet->getConfig()->m_senseCaseS;
        gv->tmpPermutatorPtr = copiedLeet;
        gv->leet_permutator_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    case BASEWORDCUTTER_PERMUTATOR:
        BasewordCutterPermutator* currentCutter;
        BasewordCutterPermutatorWindow* copiedCutter;
        currentCutter = static_cast<BasewordCutterPermutator*>(gv->generator->getPermutatorList()->at(id));
        copiedCutter = new BasewordCutterPermutatorWindow;
        copiedCutter->threeCut = currentCutter->getThreeCutSwitch();
        copiedCutter->fourCut = currentCutter->getFourCutSwitch();
        copiedCutter->fiveCut = currentCutter->getFiveCutSwitch();
        gv->tmpPermutatorPtr = copiedCutter;
        gv->cutter_permutator_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    case INDEX_REQUIREMENT:
        RequirementPermutator* currentReq;
        IndexRequirementWindow* copiedReq;
        currentReq = static_cast<RequirementPermutator*>(gv->generator->getPermutatorList()->at(id));
        copiedReq = new IndexRequirementWindow;
        for (int i = 0; i < currentReq->getRequirementList()->size(); i++) { //move this to a method of the class //copy the words from combiner permutator to the newly created buffers of the permutator window
            copiedReq->indexBufferList.push_back(new std::string{ std::to_string(currentReq->getRequirementList()->at(i)->m_index) });
            copiedReq->payloadBufferList.push_back(new std::string{ currentReq->getRequirementList()->at(i)->m_payload });
            copiedReq->labelList.push_back("##inputIndexRequirementI" + std::to_string(i));
            copiedReq->secondlabelList.push_back("##inputIndexRequirementP" + std::to_string(i));
            copiedReq->thirdlabelList.push_back("##inputIndexRequirementT" + std::to_string(i));
        }

        gv->tmpPermutatorPtr = copiedReq;
        gv->index_requirement_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    case PREFIX_REQUIREMENT:
        PrefixPermutator* currentPre;
        PrefixRequirementWindow* copiedPre;
        currentPre = static_cast<PrefixPermutator*>(gv->generator->getPermutatorList()->at(id));
        copiedPre = new PrefixRequirementWindow;
        for (int i = 0; i < currentPre->getPrefixList()->size(); i++) { //move this to a method of the class //copy the words from combiner permutator to the newly created buffers of the permutator window
            copiedPre->bufferList.push_back(new std::string{ currentPre->getPrefixList()->at(i)->m_payload });
            copiedPre->labelList.push_back("##inputPrefix" + std::to_string(i));
            copiedPre->secondlabelList.push_back("##inputTrash" + std::to_string(i));
        }

        gv->tmpPermutatorPtr = copiedPre;
        gv->prefix_requirement_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    case SUFFIX_REQUIREMENT:
        SuffixPermutator* currentSuf;
        SuffixRequirementWindow* copiedSuf;
        currentSuf = static_cast<SuffixPermutator*>(gv->generator->getPermutatorList()->at(id));
        copiedSuf = new SuffixRequirementWindow;
        for (int i = 0; i < currentSuf->getSuffixList()->size(); i++) { //move this to a method of the class //copy the words from combiner permutator to the newly created buffers of the permutator window
            copiedSuf->bufferList.push_back(new std::string{ currentSuf->getSuffixList()->at(i)->m_payload });
            copiedSuf->labelList.push_back("##inputSuffix" + std::to_string(i));
            copiedSuf->secondlabelList.push_back("##inputTrash" + std::to_string(i));
        }

        gv->tmpPermutatorPtr = copiedSuf;
        gv->suffix_requirement_window = true;
        gv->modifiedPermutatorId = gv->generator->getPermutatorList()->at(id)->getId();
        break;
    }

}

void clearAllFieldsAndPermutators(gV* gv) {
    gv->basewordsBuffer.clear();
    gv->numbersBuffers.clear();
    gv->symbolsBuffers.clear();
    gv->paradigmsBuffers.clear();
    gv->passwordLength[0] = -1;
    gv->passwordLength[1] = -1;
    gv->requireNumber = 0;
    gv->requireSpecialCharacter = 0;
    gv->requireUppercase = 0;
    gv->allowedSpecialCharsBuffer.clear();
    gv->threadNumber = static_cast<int>(std::thread::hardware_concurrency()) - 1;
    gv->saveDirectory.clear();
    strncpy(gv->saveDirectoryCSTR, "", sizeof gv->saveDirectoryCSTR - 1);
    gv->wordlistFile.clear();
    //destroying generator object is probably easier
    std::for_each(gv->generator->getPermutatorList()->begin(), gv->generator->getPermutatorList()->end(), [](Permutator* const ptr) {delete ptr; });
    gv->generator->getPermutatorList()->clear();
    gv->generator->getPermutatorList()->push_back(new Permutator(PARADIGM_PERMUTATOR, 100.0, -1, "Paradigm Permutator")); //place holder for paradigm permutator (it is default)
}
