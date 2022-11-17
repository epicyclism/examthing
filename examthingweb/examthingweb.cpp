//
// Copyright (c) 2021 Paul Ranson, paul@epicyclism.com
//
#if defined (_MSC_VER)
#include "targetver.h"
#endif

#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <tuple>

#include "http_common.h"
#include "etw_args.h"
#include "defaultHTML.h"

#include "ET_Operator.h"

constexpr std::string_view text_html{ "text/html" };
constexpr std::string_view text_csv{ "text/csv" };

using namespace std::string_view_literals;

// the exmnathing worker.
struct ETW
{
	bool can_post() const
	{
		return true;
	}
	bool can_verb(std::string_view verb) const
	{
		return verb == "/transform";
	}
//	auto process(std::string_view task, std::string_view content_type, std::string_view data)
//	{
//		return std::make_pair( sDefaultGET, text_html);
//	}
	bool has_content_disposition(std::string_view body)
	{
		auto pt = body.find("Content-Disposition: form-data; name=\"SIMS_CSV\"; filename=", 0);
		if (pt == body.npos)
		{
			resp_ = sDefaultBugBegin;
			resp_ += "Invalid content type. Content-Disposition marker not found.";
			resp_ += sDefaultEnd;
			return false;
		}
		return true;
	}
	bool set_file_name(std::string_view body)
	{
		auto fns = body.find("filename=\"", 0);
		if (fns == body.npos)
		{
			resp_ = sDefaultBugBegin;
			resp_ += "No filename provided.";
			resp_ += sDefaultEnd;
			return false;
		}
		fns += 10;
		outfn_ = "attachment; filename=\"";
		auto fne = body.find('\"', fns);
		outfn_ += "et_";
		outfn_.append(body.begin() + fns, body.begin() + fne);
		outfn_ += "\"";

		return true;
	}
	std::string_view get_work_area(std::string_view bdry, std::string_view body)
	{
		// get the work area
		auto bp = bdry.find("boundary=");
		if (bp != std::string_view::npos)
			bdry.remove_prefix(bp + 10);
		auto wes = body.find("\r\n\r\n", 0);
		auto wee = body.find(bdry, wes);
		if (wes == body.npos || wee == body.npos)
		{
			resp_ = sDefaultBugBegin;
			resp_ += "Container format incorrect.";
			resp_ += sDefaultEnd;
			return {};
		}
#if 1
		// back the end up to the end of the previous line
		//
		while (wee != wes && body[wee] != '\n')
			--wee;
#endif
		return body.substr(wes, wee - wes);
	}
	auto process(std::string_view task, std::string_view content_type, std::string_view data)
	{
#if 0
		std::cout << data << "\n\n";
#endif
		if (!has_content_disposition(data))
			return std::make_tuple(resp_, text_html, ""sv);

		if (!set_file_name(data))
			return std::make_tuple(resp_, text_html, ""sv);

		auto wa = get_work_area(content_type, data);
		if(wa.empty())
			return std::make_tuple(resp_, text_html, ""sv);
#if 0
		std::cout << std::string_view(wa.begin(), wa.end()) << "\n\n";
#endif
		data_t res;
		// transform
		try
		{
			auto e = ParseS(wa.begin(), wa.end(), res);
			if (e == wa.end())
			{
				if (res.empty())
				{
					resp_ = sDefaultBugBegin;
					resp_ += "There appear to be no valid exam records.";
					return std::make_tuple(resp_, text_html, ""sv);
				}
				else
				{
					std::ostringstream  ostr;
					WriteResults(res, ostr);
					return std::make_tuple(ostr.str(), text_csv, std::string_view{outfn_});
				}
			}
			else
			{
				auto extract = wa.substr(std::distance(wa.begin(), e), std::min(int(std::distance(e, wa.end())), 512));
				resp_ = sDefaultParseFailBegin;
				resp_ += extract;
				resp_ += sDefaultEnd;
			}
		}
		catch (std::exception& ex)
		{
			resp_ = sDefaultParseFailBegin;
			resp_ += ex.what();
			resp_ += sDefaultEnd;
		}
		catch (...)
		{
			resp_ = sDefaultException;
		}
		return std::make_tuple(resp_, text_html, ""sv);
	}
	std::string outfn_;
	std::string resp_;
};

void run_server(bool verbose, unsigned short port, std::string_view doc_root)
{
	std::cout << "Starting the examthingweb server on port " << port << "\n";
	std::cout << "Working directory is " << std::filesystem::current_path() << "\n";
	std::cout << "Serving static resources from " << doc_root << "\n";
	std::cout << "The verbose flag is " << (verbose ? "on\n" : "off\n");
	http_server_impl<ETW> svr{ "0::0", port, 1, doc_root, verbose};
	std::cout << "\n\npress 'q' and enter to exit.\n\n";
	std::string ln;
	while (std::getline(std::cin, ln))
	{
		switch (ln[0])
		{
		case 'q':
		case 'Q':
			goto exit;
		default:
			break;
		}
	}
exit:
	std::cout << "Waiting for server shutdown.... ";
	svr.stop();
	std::cout << "exiting.\n\n";
}

void welcome()
{
	std::cout << app_name << " v" << version_string << ", " << date_string << "\n\n";
	std::cout << "Copyright (c) 2013-2022 paul@epicyclism.com\n\n";
}

int main(int ac, char ** av)
{
	welcome();
	try
	{
		auto args = etw_get_args(ac, av);
		if (args.error_)
		{
			std::cout << "Error parsing argumants.\n";
			std::cout << args.wsp_;
		}
		else
		if (args.help_)
		{
			std::cout << "Usage:\n";
			std::cout << args.wsp_;
		}
		else
			run_server(args.verbose_, args.port_, std::move(args.wsp_));
	}
	catch (std::exception& ex)
	{
		std::cerr << "Caught surprising exception : " << ex.what() << '\n';
	}
}