/*
 * SQLSchema.h
 *
 *  Created on: Nov 3, 2011
 *      Author: martinc
 */

#ifndef SQLSCHEMA_H_
#define SQLSCHEMA_H_

#include <string>

/// @brief A helper class for working with SQL schemas.
///
/// The schema is a series of SQL statements. They may be read from
/// a file, or provided as a string.
class SQLSchema {
public:
	/// Constructor. Read the schema, either from a file or from a string.
	/// @param schemaOrPath The schema, or the path to a schema.
	/// @param isFilePath isFilePath If true, schemaOrPath is a file path.
	/// If false, it is the path.
	SQLSchema(std::string schemaOrPath, bool isFilePath=true);

	/// Destructor
	virtual ~SQLSchema();

	/// @return The schema.
	std::string schema();

	/// Find the user_version pragma value in the scheme. This requires that
	/// there be a pragma statement in the schema such as:
	/// @code
	/// pragma user_version = 1;
	/// pragma user_version(1);
	/// @endcode
	/// @param user_version The value of the user_version pragma will be
	/// returned here, if specified in the schema.
	/// @return True if the pragma exists, false otherwise.
	bool user_version(int& user_version);

protected:
	/// The schema. Line terminators will be retained.
	std::string _schema;
};


#endif /* SQLSCHEMA_H_ */
