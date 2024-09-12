#pragma once
#include <string>
#include "rule.h"
#include "ImGui/applicationGui.h"
#include "globalConst.h"
#include <algorithm>

class PasswordRequirement;
struct gV;

bool validatePassword(const std::string* password, PasswordRequirement& r);
void initPasswordRequirement(gV* gv, PasswordRequirement* r);
//bool customValidatePassword(std::string& password, PasswordRequirement& r);