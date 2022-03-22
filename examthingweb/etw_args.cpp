#include <args.hxx>
#include "etw_args.h"
#include "etw_version.h"

etw_arg_params etw_get_args(int ac, char const* const* av)
{
	etw_arg_params parms{ false, false, false, 8080, "./resources" };
	args::ArgumentParser ap(app_name " v" version_string ", " date_string ".", "");
	args::HelpFlag help(ap, "help", "Show this help", { 'h', "help" });
	args::Flag verbose(ap, "verbose", "print details of communcations", { 'v', "verbose" });
	args::ValueFlag<unsigned short> port(ap, "port", "accept connections on this port (default 8080)", { 'p', "port" });
	args::ValueFlag<std::string> doc_root(ap, "root", "serve html documents from this directory (default ./resources)", { 'r', "root" });
	try
	{
		ap.ParseCLI(ac, av);
	}
	catch (args::Help&)
	{
		parms.help_ = true;
		parms.wsp_ = ap.Help();
	}
	catch (args::ParseError& e)
	{
		parms.error_ = true;
		parms.wsp_ = e.what();
	}
	if (args::get(help))
	{
		parms.help_ = true;
		parms.wsp_ = ap.Help();
	}
	if (verbose)
		parms.verbose_ = args::get(verbose);
	if(port)
		parms.port_ = args::get(port);
	if (doc_root)
		parms.wsp_ = args::get(doc_root);

	return parms;
}
