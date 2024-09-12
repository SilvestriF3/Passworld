#include "applicationGui.h"
#include "initGui.h"

gV::~gV() {
	if (backendThread && backendThread->joinable()) { //detach backend thread before closing program
		generator->setGenerationState(true);
		backendThread->detach();
	}
	delete backendThread;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int gui_main(Generator* generator)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(600, 720, "Passworld", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwSetWindowSizeLimits(window, 400, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	gV gv_; //pointer to gui variables
	gV* gv{ &gv_ };
	imagePointers iP{};
	fontPointers fP{};
	gv->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	gv->io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	gv->iP = &iP;
	gv->fP = &fP;
	gv->generator = generator;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	//Load fonts and images
	loadImages(gv->iP);
	loadFonts(gv->fP, gv);

	// Main loop
	glfwSetWindowUserPointer(window, gv);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {  gV* gv = (gV*)glfwGetWindowUserPointer(window); draw(window, gv); });
	while (!glfwWindowShouldClose(window))
	{
		draw(window, gv);

	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void draw(GLFWwindow* window, gV* gv) {
	{
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y));
			ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
			ImGui::Begin("##Main Window", nullptr, ImGuiWindowFlags_NoDecoration);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 12));
			if (ImGui::BeginMainMenuBar())
			{ //TODO
				if (ImGui::BeginMenu("File"))
				{
					//import
					//export configs from file
					//save tmp configurations ?
					if (ImGui::MenuItem("Clear All Fields")) { clearAllFieldsAndPermutators(gv); }
					if (ImGui::MenuItem("Exit")) { exit(0); }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Settings"))
				{
					if (ImGui::MenuItem("Performance")) { gv->performance_window = true; }
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItem("About")) { gv->about_window = true; }
					if (ImGui::MenuItem("Getting Started")) { gv->getting_started_window = true; }
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			ImGui::PopStyleVar();
			ImGui::Text("Save To:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() - 300); //todo replace magical numbers
#ifdef _WIN32
			if (ImGui::InputText("##saveDir", gv->saveDirectoryCSTR, _MAX_PATH)) {
				gv->saveDirectory = gv->saveDirectoryCSTR;
			}
#elif __linux__
			if (ImGui::InputText("##saveDir", gv->saveDirectoryCSTR, PATH_MAX)) {
				gv->saveDirectory = gv->saveDirectoryCSTR;
			}
#endif
			ImGui::PopItemWidth();
			ImGui::SameLine();
			if (ImGui::Button("Browse")) {
				const char* ext[1]{ "*.txt" };
				char* tmpPath{ tinyfd_saveFileDialog("Choose path", "", 1, ext, "") };
				if (tmpPath) {
					gv->saveDirectory = tmpPath;
					strncpy(gv->saveDirectoryCSTR, tmpPath, sizeof gv->saveDirectoryCSTR -1);
				}
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 90); //allign on right
			if (ImGui::ImageButtonWithText(gv->iP->generateButton, "Generate", ImVec2(10, 10))) {
				//function to start generating
				//disable button just after
				if (gv->backendThread) { gv->backendThread->join(); delete gv->backendThread; }//avoid memory leak
				gv->backendThread = new std::thread{ &Generator::startGenerator, gv->generator, gv };
				ImGui::OpenPopup("Generating");
				gv->generating_window = true;

			}
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("General Configs", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Columns(2, 0, false);
				ImGui::SetColumnWidth(-1, ImGui::GetWindowWidth() * 0.75f);
				ImGui::Text("Basewords:");

				ImGui::InputTextMultiline("##input basewords", &gv->basewordsBuffer, ImVec2(ImGui::GetColumnWidth() - 10, 100)); //fix wrapping somehow, comma separeted list of basewords
				if (gv->basewordsBuffer.empty())
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 100);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
					ImGui::PushFont(gv->fP->robotoSuperSmall);
					ImGui::TextColored(ImVec4(66, 66, 66, 100), "List of comma seprated words");
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100 - ImGui::GetFrameHeight());
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
					ImGui::PopFont();
				}
				ImGui::NextColumn();
				ImGui::Text("Wordlist from file:");
				if (ImGui::Button("Browse##wordlist")) {
					char* tmpPath{ tinyfd_openFileDialog("Select a file", "", 0, 0, "", 0) }; //multiple wordlist file selection ( to implement customly with a list ) 
					if (tmpPath) {
						gv->wordlistFile = tmpPath;
					}
				}
				ImGui::TextWrapped("%s", gv->wordlistFile.substr(gv->wordlistFile.find_last_of("/\\") + 1).c_str()); //to save in a variable for performance
				ImGui::EndColumns();
				ImGui::Text("Paradigms:");
				ImGui::InputText("##paradigm buffer", &gv->paradigmsBuffers, ImGuiInputTextFlags_CallbackCharFilter, inputParadigmCallback);
				ImGui::Text("Symbols:");
				ImGui::InputText("##symbol buffer", &gv->symbolsBuffers);
				ImGui::Text("Numbers:");
				ImGui::InputText("##number buffer", &gv->numbersBuffers);
			}
			if (ImGui::CollapsingHeader("Requirement Configs", ImGuiTreeNodeFlags_DefaultOpen)) {
				//validator requirements HERE
				ImGui::Text("Password length Min-Max:");
				ImGui::SameLine();
				ImGui::InputInt2("##Password length Min-Max:", gv->passwordLength);
				ImGui::Checkbox("Require number", &gv->requireNumber);
				ImGui::Checkbox("Require uppercase", &gv->requireUppercase);
				ImGui::Checkbox("Require special character", &gv->requireSpecialCharacter);
				ImGui::Text("Allowed special character");
				ImGui::SameLine();
				ImGui::InputText("##Allowed special character", &gv->allowedSpecialCharsBuffer);
			}
			/////
			AlignForWidth(ImGui::GetMainViewport()->Size.x - 20); // -child window padding
			ImGui::Text("Permutators");
			ImGui::BeginChild("##permutator panel", ImVec2(0, -40), true); //lower space = 40 
			static int selected = 0;
			for (int i = 0; i < gv->generator->getPermutatorList()->size(); i++) //vector of permutators windows
			{
				addPermutatorWindow(gv, i); //draw each permutator
			}
			ImGui::BeginGroup();
			ImGui::PushFont(gv->fP->robotoLarge);
			ImGui::Spacing();
			AlignForWidth(ImGui::CalcTextSize("Add").x);
			if (ImGui::Button("Add")) {
				ImGui::OpenPopup("Add permutator");
				gv->show_add_permutator_window = true;
			}
			//-----------------------------------------------------------------------------------------------//
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Add permutator", &gv->show_add_permutator_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
				ImGui::Text("Choose a permutator/rule:");
				ImGui::SetNextWindowPos(ImVec2(gv->io.DisplaySize.x * 0.5f, gv->io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::BeginChild("choose child", ImVec2(ImGui::GetWindowWidth()-20, ImGui::GetWindowHeight() - 90), false, ImGuiWindowFlags_NoDecoration);
				if (ImGui::Button("Leet Permutator", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->leet_permutator_window = true; //show the window
					gv->tmpPermutatorPtr = new LeetPermutatorWindow;
				}
				ImGui::Separator();
				if (ImGui::Button("Word combiner", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->combine_permutator_window = true;
					gv->tmpPermutatorPtr = new CombinerPermutatorWindow;
					static_cast<CombinerPermutatorWindow*>(gv->tmpPermutatorPtr)->bufferList.push_back(new std::string); //move this to a method of the class
					static_cast<CombinerPermutatorWindow*>(gv->tmpPermutatorPtr)->labelList.push_back("##inputCombinator0");
				}
				ImGui::Separator();
				if (ImGui::Button("Baseword Cutter", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->cutter_permutator_window = true;
					gv->tmpPermutatorPtr = new BasewordCutterPermutatorWindow;
				}
				ImGui::Separator();
				if (ImGui::Button("Capital Permutator", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->capital_permutator_window = true;
					gv->tmpPermutatorPtr = new CapitalPermutatorWindow;
				}
				ImGui::Separator();
				if (ImGui::Button("Prefix rule", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->prefix_requirement_window = true; 
					gv->tmpPermutatorPtr = new PrefixRequirementWindow;
					static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr)->bufferList.push_back(new std::string); //move this to a method of the class
					static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr)->labelList.push_back("##inputPrefix0");
					static_cast<PrefixRequirementWindow*>(gv->tmpPermutatorPtr)->secondlabelList.push_back("##inputTrash0");
				}
				ImGui::Separator();
				if (ImGui::Button("Suffix rule", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->suffix_requirement_window = true;
					gv->tmpPermutatorPtr = new SuffixRequirementWindow;
					static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr)->bufferList.push_back(new std::string); //move this to a method of the class
					static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr)->labelList.push_back("##inputSuffix0");
					static_cast<SuffixRequirementWindow*>(gv->tmpPermutatorPtr)->secondlabelList.push_back("##inputTrash0");
				}
				ImGui::Separator();
				if (ImGui::Button("Index rule", ImVec2(ImGui::GetWindowWidth(), 35))) {
					ImGui::CloseCurrentPopup();
					gv->index_requirement_window = true;
					gv->tmpPermutatorPtr = new IndexRequirementWindow;
					static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr)->indexBufferList.push_back(new std::string);
					static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr)->payloadBufferList.push_back(new std::string);
					static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr)->labelList.push_back("##inputIndexRequirementI");
					static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr)->secondlabelList.push_back("##inputIndexRequirementP");
					static_cast<IndexRequirementWindow*>(gv->tmpPermutatorPtr)->thirdlabelList.push_back("##inputIndexRequirementT");
				}
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 72, ImGui::GetWindowHeight() - 40));
				if (ImGui::Button("Close")) {
					gv->show_add_permutator_window = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 80);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::PopFont();
			ImGui::EndGroup();

			ImGui::EndChild();
			ImGui::End();
		}

		if (gv->combine_permutator_window) {
			showPermutatorWindowCombiner(gv);
		}
		if (gv->capital_permutator_window) {
			showPermutatorWindowCapital(gv);
		}
		if (gv->leet_permutator_window) {
			showPermutatorWindowLeet(gv);
		}
		if (gv->cutter_permutator_window) {
			showPermutatorWindowCutter(gv);
		}
		if (gv->index_requirement_window) {
			showPermutatorWindowIndexRequirement(gv);
		}
		if (gv->prefix_requirement_window) {
			showPermutatorWindowPrefixRequirement(gv);
		}
		if (gv->suffix_requirement_window) {
			showPermutatorWindowSuffixRequirement(gv);
		}
		if (gv->about_window) {
			ImGui::OpenPopup("About");
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("About", &gv->about_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::TextWrapped("Passworld is a fully customizable password wordlist generator which aims to help Red Teamers in unveiling weak passwords composed by user's known information and constructed on typical human password paradigms.\
\nIt is written in C++ to achieve best performances.\
\nThe GUI is built on Dear ImGui.");
				ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 20);
				ImGui::PushFont(gv->fP->robotoSuperSmall);
				AlignForWidth(ImGui::CalcTextSize("This application was created by SilvestriF3.").x);
				ImGui::Text("This application was created by SilvestriF3.");
				ImGui::PopFont();
				ImGui::EndPopup();
			}
		}
		if (gv->getting_started_window) {
			ImGui::OpenPopup("Getting Started");
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Getting Started", &gv->getting_started_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::TextWrapped("The tool's work is divided in 3 stages/steps, in the first stage (PreGeneration) all the input words of the dedicated text box or of the wordlist file are permutated, then in second stage (PostGeneration) the permutated words are used with input numbers and symbols and paradigms to generate a candidate password, on this candidate all the other permutations/substitutions/additions are done, eventually the candidate is validated (third stage) against user's defined rules and appended to a thread stream and eventually to the output file.\
\nThe user can decide what permutators to add and define some configs and rules.\nThe tool is capable of generating tens of thousands of passwords per second with multi-threading, check the performance tab under settings for more information.");
				ImGui::TextLinkOpenURL("Check out some tips on Github.", "https://github.com/SilvestriF3/Passworld/blob/master/Docs/DOCUMENTATION.md");
				ImGui::EndPopup();
			}
		}
		if (gv->performance_window) {
			ImGui::OpenPopup("Performance");
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Performance", &gv->performance_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::Text("Options:");
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
				ImGui::Text("Threads to use:");
				ImGui::SameLine();
				if (ImGui::InputInt("##Threads to use:", &gv->threadNumber, 0, 0)) {
					if (gv->threadNumber < 1) { //enforce at least one thread 
						gv->threadNumber = 1;
					}
				}
				ImGui::Text("By default it uses all cpu threads minus one");
				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}
		}
		if (gv->generating_window) {
			ImGui::OpenPopup("Generating");
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), 0, ImVec2(0.5f, 0.5f));
			ImGui::BeginPopupModal("Generating", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);
			AlignForWidth(ImGui::CalcTextSize("Generating wordlist...").x);
			ImGui::Text("Generating wordlist...");
			ImGui::Text("Progress:");
			if (gv->generator->getStage() != 1 && gv->generator->getCurrentBaseword()) {
				ImGui::TextUnformatted(gv->generator->getCurrentBaseword()->c_str()); //possible null-pointer dereference
				ImGui::ProgressBar(static_cast<float>(gv->generator->getPermutatedBasewordListIndex()) / static_cast<int>(gv->generator->getPermutatedBasewordList()->size()));
				std::stringstream ss;
				ss << gv->generator->getPermutatedBasewordListIndex() << " / " << gv->generator->getPermutatedBasewordList()->size();
				std::string tmp{ ss.str() };
				ImGui::TextUnformatted(tmp.c_str());
				ImGui::Text("Total candidates: %u", gv->generator->getTotalCandidateCount());
			}
			else if (gv->generator->getStage() == 1) {
				ImGui::Text("Permutating the basewords...");
				ImGui::ProgressBar(static_cast<float>(gv->generator->getPureBasewordListIndex()) / static_cast<int>(gv->generator->getGeneratorConfig()->m_basewordList.size()));
			}
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 60, ImGui::GetWindowHeight() - 30));
			if (!gv->generator->getHasCompleted() && ImGui::Button("Cancel##generating_window")) {
				//stop the thread
				gv->generator->setGenerationState(true);
				gv->generator->checkNoThreadRunning();//check all workers stopped
				gv->generator->setHasCompleted(false);
				gv->generator->setGenerationState(false);
				gv->generating_window = false;
				ImGui::CloseCurrentPopup();
				//reset the generator object state but keep the input configs
			}
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 50, ImGui::GetWindowHeight() - 30));
			if (gv->generator->getHasCompleted()) {
				if (ImGui::Button("Done##generating_window")) {
					gv->generator->setHasCompleted(false);
					gv->generating_window = false;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();

		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(gv->clear_color.x * gv->clear_color.w, gv->clear_color.y * gv->clear_color.w, gv->clear_color.z * gv->clear_color.w, gv->clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
}