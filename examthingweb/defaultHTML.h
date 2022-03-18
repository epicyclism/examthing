#pragma once

#include <string_view>

constexpr std::string_view sDefaultParseFailBegin { "<!DOCTYPE html>\n"
									"<html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
									"<head>\n"
									"<meta charset=\"utf-8\"/>\n"
									"<title>Examthing Parse Fail</title>\n"
									"</head>\n"
									"<body>\n"
									"<h1>The data you supplied failed to parse.</h1>\n"
									"<hr/>\n"
									"<br/>\n" };

constexpr std::string_view  sDefaultEnd	= "<br/>\n"
									"<hr/>\n"
									"Copyright epicyclism.com &copy; 2022 -- paul@epicyclism.com"
									"<hr/>\n"
									"</body>\n"
									"</html>";

constexpr std::string_view  sDefaultException{ "<!DOCTYPE html>\n"
									"<html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
									"<head>\n"
									"<meta charset=\"utf-8\"/>\n"
									"<title>Examthing Exception</title>\n"
									"</head>\n"
									"<body>\n"
									"<h1>An exception occurred during processing. One day this page will have more information.</h1>\n"
									"</body>\n"
									"</html>" };

constexpr std::string_view  sDefaultBugBegin{ "<!DOCTYPE html>\n"
									"<html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
									"<head>\n"
									"<meta charset=\"utf-8\"/>\n"
									"<title>Examthing Bug</title>\n"
									"</head>\n"
									"<body>\n"
									"<h1>Something was wrong with the data you supplied.</h1>\n"
									"<hr/>\n"
									"<br/>\n" };

constexpr std::string_view  sDefaultGET{ "<!DOCTYPE html>\n"
								"<html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
								"<head>\n"
								"<meta charset=\"utf-8\"/>\n"
								"<title>Examthing About</title>\n"
								"</head>\n"
								"<body>\n"
								"<h1>ExamthingWorker WebApp</h1>\n"
								"<hr/>\n"
								"<br/>\n"
								"version 0.03. March 16 2022."
								"<hr/>\n"
								"Copyright epicyclism.com &copy; 2022 -- paul@epicyclism.com"
								"<hr/>\n"
								"</body>\n"
								"</html>" };

