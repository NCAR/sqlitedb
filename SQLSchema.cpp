/*
 * SQLSchema.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: martinc
 */

#include "SQLSchema.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <boost/regex.hpp>

//////////////////////////////////////////////////////////////////////////////
SQLSchema::SQLSchema(std::string schemaOrPath, bool isFilePath) {

	if (isFilePath) {
		std::ifstream dbSchemaFile(schemaOrPath.c_str());
		std::string s((std::istreambuf_iterator<char>(dbSchemaFile)),
				       std::istreambuf_iterator<char>());
		_schema = s;
	} else {
		_schema = schemaOrPath;
	}

	int v;
}

//////////////////////////////////////////////////////////////////////////////
SQLSchema::~SQLSchema() {

}

//////////////////////////////////////////////////////////////////////////////
std::string SQLSchema::schema() {
	return _schema;
}

//////////////////////////////////////////////////////////////////////////////
bool SQLSchema::user_version(int& user_version) {

	// Regular expression to find  and decode the user_version pragma.
	// ( Elements enclosed in parenthesis are returned in the results).
	boost::regex pragma_re(".*pragma.*user_version.*=.*([0-9]+).*");

	// Search for the pragma.
	boost::match_results<std::string::const_iterator> results;
	if (boost::regex_match(_schema, results, pragma_re)) {
		std::stringstream(results[1]) >> user_version;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
