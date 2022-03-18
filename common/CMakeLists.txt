cmake_minimum_required (VERSION 3.18)

add_library (common STATIC CSVHelper.h ET_Operator.h ET_Parser.h mm_file.h ET_Parser.cpp etw_version.h http_common.h
				http_common.cpp  )
