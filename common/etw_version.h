//
// Copyright (c) 2021 Paul Ranson, paul@epicyclism.com
//
//

#pragma once

#include <string_view>

#define app_name "Epicyclism ExamThing"
#define version_string "1.02"
#define date_string "November 17 2022"
constexpr std::string_view announce_string{ app_name " v" version_string " (" date_string ")" };
constexpr std::string_view etw_user_agent { app_name "/" version_string };
