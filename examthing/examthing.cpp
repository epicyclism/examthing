﻿//
// examthing.cpp: copyright (c) 2022 Paul Ranson, paul@epicyclism.com
//

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "mm_file.h"
#include "ET_Operator.h"

void usage()
{
	std::cerr << "Usage : ExamThing <xxx.csv>\n";
	std::cerr << "Writes output to console, so typically 'examthing xxx.csv > xxx_et.csv'\n\n";
}

int main(int argc, char* argv[])
{
	if (argc < 2 || argv[1][9] == '-' || argv[1][0] == '/')
	{
		usage();
		return -1;
	}
	mem_map_file<char> file(argv[1]);
	if (!file)
	{
		std::cerr << "File <" << argv[1] << "> couldn't be opened.\n\n";
		usage();
		return -1;
	}

	auto iter = file.begin();
	auto end = file.end();
	data_t res;
	bool r = Parse(&iter, &end, res);

	if (r && iter == end)
	{
		WriteResults(res, std::cout);
	}
	else
	{
		if (end - iter > 512)
			end = iter + 512;
		std::string rest(iter, end);
		std::cerr << "-------------------------\n";
		std::cerr << "Parsing failed\n";
		std::cerr << "stopped at: \": " << rest << "\"\n";
		std::cerr << "-------------------------\n";
	}

	return 0;
}

