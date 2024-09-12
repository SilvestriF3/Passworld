#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <stdio.h>
#include "initGui.h"
#include "guiUtil.h"
#include "../generator.h"
#include "tinyfiledialogs.h"
#include <iostream>
#include <vector>
#include <string>

#ifdef __linux__
#include <limits.h>
#endif

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

struct imagePointers;
struct fontPointers;
class IPermutatorWindow;
class Generator;
struct gV { //gui variables struct (used for the dynamic resize thing, pass parameters to function )
    imagePointers* iP{};
    fontPointers* fP{};
    Generator* generator{};
    ImGuiIO& io{ ImGui::GetIO() };
    std::thread* backendThread{};
    ~gV();
    bool show_demo_window = true;
    bool show_another_window = false;
    std::string saveDirectory{};
    std::string wordlistFile;
    #ifdef _WIN32
    char saveDirectoryCSTR[_MAX_PATH] {}; //buffer for path where to save
    #elif __linux__
    char saveDirectoryCSTR[PATH_MAX]{}; //buffer for path where to save
    #endif
    IPermutatorWindow* tmpPermutatorPtr{};
    bool show_add_permutator_window{false};
    bool combine_permutator_window{false};
    bool capital_permutator_window{ false };
    bool leet_permutator_window{ false };
    bool cutter_permutator_window{ false };
    bool index_requirement_window{ false };
    bool prefix_requirement_window{ false };
    bool suffix_requirement_window{ false };
    bool generating_window{false};
    bool about_window{ false };
    bool performance_window{ false };
    bool getting_started_window{ false };
    std::string basewordsBuffer{"Hello, This, Is, Passworld"};  //default values
    std::string paradigmsBuffers{"cnn, csn, ncs"};
    std::string symbolsBuffers{ "*, /, !, $, ?, @" };
    std::string numbersBuffers{ "1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 123, 1234, 888, 777, 00" };
    //requirements
    int passwordLength[2]{-1, -1};//min-max
    bool requireNumber{true};
    bool requireSpecialCharacter{false};
    bool requireUppercase{true};
    std::string allowedSpecialCharsBuffer{"*, !, @, +, #"};
    int threadNumber{ static_cast<int>(std::thread::hardware_concurrency()) - 1 };
    int preChunkSize{ 20 };
    int postChunkSize{ 20 };
    //
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int permutatorButtonPressedId{};
    bool permutatorBeingEdited{ false };
    int modifiedPermutatorId{};
};

int gui_main(Generator* generator);
void draw(GLFWwindow* window, gV* gv);