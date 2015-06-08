/*
 * SQLSchema.cpp
 *
 *  Created on: Nov 3, 2011
 *      Author: martinc
 */
#include "SQLSchema.h"
#include <QtCore/QtCore>
#include <iostream>
#include <sstream>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
SQLSchema::SQLSchema(std::string schemaOrPath, bool isFilePath) {

	if (isFilePath) {
		QFile dbSchemaFile(schemaOrPath.c_str());
		dbSchemaFile.open(QFile::ReadOnly);
		_schema = QTextStream(&dbSchemaFile).readAll().toStdString();
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
	// (Hint: the parenthesis identifiy the fields that
	// will be returned by QRegExp::cap())
	QRegExp re("pragma\\s+user_version.*([0-9]+).*;", Qt::CaseInsensitive);

	// Use setMinimal to enable non-greedy matching.
	re.setMinimal(true);

	// Search for the pragma.
	if (re.indexIn(QString(_schema.c_str())) != -1) {
		std::stringstream(re.cap(1).toStdString()) >> user_version;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
