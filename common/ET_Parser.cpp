//
// ET_Parser.cpp: copyright (c) 2022 Paul Ranson, paul@epicyclism.com
//

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "ET_Operator.h"
#include "ET_Parser.h"

// leading short, red, trailing short, blue, long, green
//
void ColourRows(colour_row_set& vvRows)
{
	if (vvRows.size() < 2)
		return;

	auto it = vvRows.begin();
	auto itp = vvRows.begin();
	if ((*it).row_.size() < 3)
		(*it).colour_ = colour_row::RED;
	++it;
	while (it != vvRows.end())
	{
		if ((*it).row_.size() < 3)
		{
			(*it).colour_ = colour_row::RED;
			if ((*itp).colour_ == colour_row::RED)
				(*itp).colour_ = colour_row::BLUE;
		}
		else
			(*it).colour_ = colour_row::GREEN;
		++it;
		++itp;
	}
	// cannot end on a RED
	if ((*itp).colour_ == colour_row::RED)
		(*itp).colour_ = colour_row::BLUE;
#if 0
#if defined(_DEBUG)
	std::cerr << "ColourRows...\n";
	for (colour_row const& r : vvRows)
	{
		switch (r.colour_)
		{
		case colour_row::RED:
			std::cerr << r.row_.size() << " - RED\n";
			break;
		case colour_row::GREEN:
			std::cerr << r.row_.size() << " - GREEN\n";
			break;
		case colour_row::BLUE:
			std::cerr << r.row_.size() << " - BLUE\n";
			break;
		}
	}
#endif
#endif
}

void ProcessRowsToExams(candidate& cd, colour_row_set const& vvRows)
{
	exam_row exam;
	for (colour_row const& cr : vvRows)
	{
		switch (cr.colour_)
		{
		case colour_row::RED:
			if (!exam.row_.empty())
			{
				cd.exams_.push_back(exam);
			}
			exam.row_ = cr.row_;
			if (cr.row_.size() == 1)
				exam.row_.push_back(""); // seat forgotten or not set yet.
			break;
		case colour_row::GREEN:
			std::copy(cr.row_.begin(), cr.row_.end(), std::back_inserter(exam.row_));
			break;
		case colour_row::BLUE:
			// apply magic to the exam row
			for (auto s : cr.row_)
			{
				int i = 0;
				for (auto& e : exam.row_)
				{
					//					if (i > 2 && !e.empty() && e.back() == ' ')
										// 'DASHWOOD ' and 'BUILDING', for example...
										// but ignore the seat, which often has trailing spaces.
					if (i != 1 && !e.empty() && e.back() == ' ')
					{
						e += s;
						break;
					}
					++i;
				}
			}
			break;
		}
	}
	// don't forget the last...
	if (!exam.row_.empty())
	{
		cd.exams_.push_back(exam);
	}
}

// the hand crafted one
bool Parse(char const** b, char const** e, data_t& results)
{
	LexicalAnalyser<char, CSVSeparator, ET_Classifier> la(*b, *e);
	if (la.Current() == size_t(-1))
		return false;

	enum ET_STATE { WAIT_HDR, HEADER, EXAM_HDR, EXAM };
	size_t state = WAIT_HDR;
	candidate cd;
	size_t cnt = 0; // keep GCC happy, because state is initialised, cnt gets initialised first time into the state machine
	colour_row_set vvRows;
	colour_row      vRow;

	do
	{
		size_t token = la.Current();
		//		std::cout << token << " = " << la.Value() << "\r\n";
		switch (state)
		{
		case WAIT_HDR:
			if (token == ET_Classifier::INDIVIDUALCANDIDATETABLE)
			{
				cd.Reset();
				vvRows.clear();
				cnt = 0;
				state = HEADER;
			}
			break;
		case HEADER:
			if (token == ET_Classifier::ULN)
			{
				if (cnt < 7)
				{
					cd.hdr_.UCI_ = "NO UCI ASSIGNED";
					++cnt;
				}
			}
			else
				if (token == ET_Classifier::VARIABLE)
				{
					switch (cnt)
					{
					case 0:
						cd.hdr_.season_ = la.Value();
						break;
					case 1:
						cd.hdr_.centre_ = la.Value();
						break;
					case 2:
						cd.hdr_.name_ = la.Value();
						break;
					case 3:
						cd.hdr_.year_ = la.Value();
						break;
					case 4:
						cd.hdr_.number_ = la.Value();
						break;
					case 5:
						cd.hdr_.group_ = la.Value();
						break;
					case 6:
						cd.hdr_.UCI_ = la.Value();
						break;
					case 7:
						cd.hdr_.ULN_ = la.Value();
						break;
					}
					++cnt;
					if (cnt == 8)
						state = EXAM;
				}
				else
					if (token == ET_Classifier::START)
					{
						// force 'end of header'
						state = EXAM;
					}
			break;
		case EXAM_HDR:
			break;
		case EXAM:
			if (token == ET_Classifier::VARIABLE)
			{
				vRow.row_.push_back(la.Value());
			}
			else
				if (token == ET_Classifier::NL)
				{
					if (!vRow.row_.empty())
					{
						vvRows.push_back(vRow);
						vRow.row_.clear();
					}
				}
				else
					if (token == ET_Classifier::PRINTEDON)
					{
						ColourRows(vvRows);
						ProcessRowsToExams(cd, vvRows);
						results.push_back(cd);
						state = WAIT_HDR;
					}
			break;
		}

	} while (la.Next());

	*b = la.B();

	return true;
}

bool IsDate(std::string const& s)
{
	static const char* days[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };

	if (s.size() < 3)
		return false;

	char sTst[4];
	::memcpy(sTst, s.data(), 3);
	sTst[3] = 0;
	for (auto day : days)
	{
		if (::strcmp(day, sTst) == 0)
			return true;
	}
	return false;
}

bool TransformRawRowToExam(exam_row const& row, exam& ex)
{
	// does the row contain sufficient fields?
	if (row.row_.size() < 11)
		return false;
	// does first element of input contain a date?
	if (IsDate(row.row_[0]))
	{
		ex.date_ = row.row_[0];
		ex.time_ = row.row_[1];
		ex.board_ = row.row_[2];
		ex.level_ = row.row_[3];
		ex.eltcode_ = row.row_[4];
		ex.elementtitle_ = row.row_[5];
		ex.compcode_ = row.row_[6];
		ex.comptitle_ = row.row_[7];
		ex.duration_ = row.row_[8];
		ex.room_ = row.row_[9];
		ex.seat_ = row.row_[10];
	}
	else
	{
		ex.date_ = row.row_[2];
		ex.time_ = row.row_[3];
		ex.board_ = row.row_[4];
		ex.level_ = row.row_[5];
		ex.eltcode_ = row.row_[6];
		ex.elementtitle_ = row.row_[7];
		ex.compcode_ = row.row_[8];
		ex.comptitle_ = row.row_[9];
		ex.duration_ = row.row_[10];
		ex.room_ = row.row_[0];
		ex.seat_ = row.row_[1];
	}
	return true;
}

void CleanCD(candidate& cd)
{
	if (FindComma(cd.hdr_.centre_))
		FixComma(cd.hdr_.centre_);
	if (FindComma(cd.hdr_.group_))
		FixComma(cd.hdr_.group_);
	if (FindComma(cd.hdr_.name_))
		FixComma(cd.hdr_.name_);
	if (FindComma(cd.hdr_.number_))
		FixComma(cd.hdr_.number_);
	if (FindComma(cd.hdr_.season_))
		FixComma(cd.hdr_.season_);
	if (FindComma(cd.hdr_.UCI_))
		FixComma(cd.hdr_.UCI_);
	if (FindComma(cd.hdr_.ULN_))
		FixComma(cd.hdr_.ULN_);
	if (FindComma(cd.hdr_.year_))
		FixComma(cd.hdr_.year_);
}

void CleanExam(exam& e)
{
	if (FindComma(e.board_))
		FixComma(e.board_);
	if (FindComma(e.compcode_))
		FixComma(e.compcode_);
	if (FindComma(e.comptitle_))
		FixComma(e.comptitle_);
	if (FindComma(e.date_))
		FixComma(e.date_);
	if (FindComma(e.duration_))
		FixComma(e.duration_);
	if (FindComma(e.elementtitle_))
		FixComma(e.elementtitle_);
	if (FindComma(e.eltcode_))
		FixComma(e.eltcode_);
	if (FindComma(e.level_))
		FixComma(e.level_);
	if (FindComma(e.room_))
		FixComma(e.room_);
}

void WriteResults(data_t& data, std::ostream& out)
{
	for (auto& cd : data)
	{
		CleanCD(cd);
		for (auto& ex : cd.exams_)
		{
			exam e;
			if (TransformRawRowToExam(ex, e))
			{
				CleanExam(e);
				out << cd.hdr_.name_ << ", "
					<< cd.hdr_.number_ << ", ";
				if (cd.hdr_.year_.empty())
					out << "E" << ", ";
				else
					out << cd.hdr_.year_ << ", ";
				out << cd.hdr_.group_ << ", ";
				out << e.date_ << ", "
					<< e.time_ << ", "
					<< e.eltcode_ << ", "
					<< e.comptitle_ << ", "
					<< e.duration_ << ", "
					<< e.room_ << ", "
					<< e.seat_ << "\n";

			}
			else
			{
				// barf
				out << cd.hdr_.name_ << ", "
					<< cd.hdr_.number_ << ", ";
				out << "malformed exam record!!!!\n";
			}
		}
	}
}