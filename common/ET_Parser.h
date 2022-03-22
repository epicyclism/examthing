//
// ET_Parser.h: copyright (c) 2022 Paul Ranson, paul@epicyclism.com
//
#pragma once

#include <map>

// Examthing specific outline
struct ET_Classifier
{
	enum TOKEN {
		VARIABLE, NAME, YEAR, EXAMNUMBER, SEASON, REGGROUP, CENTRENUMBER, UCI, ULN,
		SPECIALARRANGEMENTSREQUIRED, START, ELEMENT, COMPONENT, DATE, BOARD, LEVEL,
		ELEMENTTITLE, COMPONENTTITLE, DURATION, ROOM, SEAT, TIME, CODE, INDIVIDUALCANDIDATETABLE,
		PRINTEDON, NL
	};
#if defined (_DEBUG)
	static char const* TokenToDesc( size_t tok)
	{
		switch (tok)
		{
		case VARIABLE :
			return "VARIABLE";
		case NAME:
			return "NAME";
		case YEAR:
			return "YEAR";
		case EXAMNUMBER:
			return "EXAMNUMBER";
		case SEASON:
			return "SEASON";
		case REGGROUP:
			return "REGGROUP";
		case CENTRENUMBER:
			return "CENTRENUMBER";
		case UCI:
			return "UCI";
		case ULN:
			return "ULN";
		case SPECIALARRANGEMENTSREQUIRED :
			return "SPECIALARRANGEMENTSREQUIRED";
		case START:
			return "START";
		case ELEMENT:
			return "ELEMENT";
		case COMPONENT:
			return "COMPONENT";
		case DATE:
			return "DATE";
		case BOARD:
			return "BOARD";
		case LEVEL:
			return "LEVEL";
		case ELEMENTTITLE:
			return "ELEMENTTITLE";
		case COMPONENTTITLE:
			return "COMPONENTTITLE";
		case DURATION:
			return "DURATION";
		case ROOM:
			return "ROOM";
		case SEAT:
			return "SEAT";
		case TIME:
			return "TIME";
		case CODE:
			return "CODE";
		case INDIVIDUALCANDIDATETABLE:
			return "INDIVIDUALCANDIDATETABLE";
		case PRINTEDON:
			return "PRINTEDON";
		case NL :
			return "NL";
		default:
			return "Bug - invalid token";
		}
		// NOT REACHED
	}
#endif
	template<typename S> static size_t Classify(S const& s)
	{
		static std::map<std::string, size_t> m { { "Name :", NAME }, { "Year :", YEAR },
		{ "Exam Number :", EXAMNUMBER }, { "Code", CODE }, { "Season :", SEASON },
		{ "Reg Group :", REGGROUP }, { "Centre Number :", CENTRENUMBER }, { "UCI :", UCI }, { "ULN :", ULN },
		{ "Special Arrangements required", SPECIALARRANGEMENTSREQUIRED }, { "Start ", START }, { "Element ", ELEMENT }, { "Component", COMPONENT }, { "Date", DATE }, { "Board", BOARD },
		{ "Level", LEVEL }, { "Element Title", ELEMENTTITLE }, { "Component Title", COMPONENTTITLE }, { "Duration", DURATION },
		{ "Room", ROOM }, { "Seat", SEAT }, { "Time", TIME }, { "Code", CODE },
		{ "Individual Candidate Timetable", INDIVIDUALCANDIDATETABLE }, { "Printed On", PRINTEDON },
		{ "\r", NL }, { "\n", NL }, { "\r\n", NL }
		};

		auto k = m.find(s);
		if (k == m.end())
		{
#if defined (_DEBUG)
			std::cerr << "VARIABLE - " << s << std::endl;
#endif
			return VARIABLE;
		}
#if defined (_DEBUG)
		if ( (*k).second == NL)
			std::cerr << "NL" << std::endl;
		else
			std::cerr << TokenToDesc ( (*k).second) << " - " << s << std::endl;
#endif
		return (*k).second;
	}
};

struct colour_row
{
	enum CR_COLOURS { RED, GREEN, BLUE};
	int colour_;
	std::vector<std::string> row_;
	colour_row() : colour_(RED)
	{
	}
};

using colour_row_set = std::vector<colour_row>;

// leading short, red, trailing short, blue, long, green
//
void ColourRows(colour_row_set& vvRows);
void ProcessRowsToExams(candidate& cd, colour_row_set const& vvRows);

// the hand crafted one
bool Parse(char const** b, char const** e, data_t& results);

template <typename I> const I ParseS(const I b, const I e, data_t& results)
{
	LexicalAnalyserI<I, CSVSeparator, ET_Classifier> la(b, e);
	if (la.Current() == -1)
		return b;

	enum ET_STATE { WAIT_HDR, HEADER, EXAM_HDR, EXAM };
	size_t state = WAIT_HDR;
	candidate cd;
	size_t cnt;
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

	return la.E();
}


bool IsDate(std::string const& s);
bool TransformRawRowToExam(exam_row const& row, exam& ex);

template<typename S> bool FindComma(S const& cs)
{
	return cs.find(typename S::value_type(',')) != S::npos;
}

template<typename S> void FixComma(S& cs)
{
	cs.insert(0, 1, typename S::value_type('\"'));
	cs.append(1, typename S::value_type('\"'));
}

void CleanCD(candidate& cd);
void CleanExam(exam& e);
void WriteResults(data_t& data, std::ostream& out);