#pragma once

#include <string>

struct etw_arg_params
{
	bool error_;
	bool help_;
	unsigned short port_;
	std::string wsp_;
};

etw_arg_params etw_get_args(int ac, char const* const* av);
