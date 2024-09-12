#pragma once
#include "imgui_internal.h"
#include "applicationGui.h"
#include "initGui.h"
#include <string>
#include <GLFW/glfw3.h>

struct gV;

void AlignForWidth(float width, float alignment = 0.5f);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool LoadTextureFromFile(const char* filename, ImTextureID* out_texture, int* out_width, int* out_height);
bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);

namespace ImGui {
	// label is used as id
	// <0 frame_padding uses default frame padding settings. 0 for no padding
	IMGUI_API bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
} // namespace ImGui

void addPermutatorWindow(gV* gv, int i);
void editPermutator(gV* gv, int id);
void clearAllFieldsAndPermutators(gV* gv);