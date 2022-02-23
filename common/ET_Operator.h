//
// ET_operator.h: copyright (c) 2022 Paul Ranson, paul@epicyclism.com
//

#pragma once

struct exam
{
	std::string date_;
	std::string time_;
	std::string board_;
	std::string level_;
	std::string eltcode_;
	std::string elementtitle_;
	std::string compcode_;
	std::string comptitle_;
	std::string duration_;
	std::string room_;
	std::string seat_;
};

struct exam_row
{
	std::vector<std::string> row_;
};

struct header
{
	std::string season_;
	std::string name_;
	std::string year_;
	std::string number_;
	std::string group_;
	std::string UCI_;
	std::string centre_;
	std::string ULN_;
	void Reset()
	{
		season_.clear();
		name_.clear();
		year_.clear();
		number_.clear();
		group_.clear();
		UCI_.clear();
		centre_.clear();
		ULN_.clear();
	}
};

struct candidate
{
	header hdr_;
	std::vector<exam_row> exams_;
	void Reset()
	{
		hdr_.Reset();
		exams_.clear();
	}
};

typedef 	std::vector<candidate> data_t;

// this ("(?:[^\\"]|\\.)*")\s*(?:$|,) might be a regexp that will suck csv blobs out
// to try....
#include "CSVHelper.h"
#include "ET_Parser.h"