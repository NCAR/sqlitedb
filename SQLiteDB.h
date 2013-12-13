#ifndef SQLITEDB_INC
#define SQLITEDB_INC

#include <sqlite3.h>
#include "spatialite.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

/// Note that the "pragma foreign_keys=on;" sql statment is executed
/// when the database is opened, so that foreign key cascade actions
/// are honored.
class SQLiteDB {

public:
	/// Constructor
	/// @param dbPath The file path name of the database.
	/// @param delayInit Delay initializing the database. The user must then
	/// call the init() function.
	/// @param mode The database open mode, as defined for the sqlite3_open_v2() call.
	/// Use SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE to create a new database.
	SQLiteDB(std::string dbPath, bool delayInit = false, int mode = SQLITE_OPEN_READONLY) throw (std::string);
	/// Destructor
	/// The database will be closed
	virtual ~SQLiteDB();
	/// Open the database.
	void init();
	/// @return The pragma user_version
	int user_version();
	/// @return A string with SQLite version information
	std::string version();
	/// Get the table names.
	/// @return The table names.
	std::vector<std::string> table_names();
	/// Get the column names for a table.
	/// @param table_name The table of interest
	/// @return The names of columns in this table
	std::vector<std::string> column_names(std::string table_name);
	/// @param col The column number (0 based)
	/// @return The column type: SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_BLOB, SQLITE_TEXT,
	/// SQLITE_TEXT3, or SQLITE_NULL
	int colType(int col);
	/// Execute an sql statement.
	/// @param sql The SQL statement
	/// @throws A string with an error message if the exec() fails.
	void exec(std::string sql) throw (std::string);
	/// Execute an sql statement.
	/// @param sql The SQL statement
	/// @throws A string with an error message if the exec() fails.
	void exec(std::stringstream& sql) throw (std::string);
	/// Use sqlite3_prepare_v2() to prepare the sql statement 
	/// for later evaluation via the sqlite3_step() function.
	/// @param sql The sql statement
	/// @throws A string with an error message if the prepare()
	/// fails.
	void prepare(std::string sql) throw (std::string);
	/// Step to the next row of the previously prepared statement
	/// @return True if there is another row, false otherwise
	bool step();
	/// @param col The column to be extracted.
	/// @return A string from the selected column of the current row
	std::string Text(int col) throw (std::string);
	/// @return An integer from the selected column of the current row
	int Integer(int col) throw (std::string);
	/// @param col The column to be extracted.
	/// @return A float from the selected column of the current row.
	/// SQL calls them floats, but they are really doubles.
	double Float(int col) throw (std::string);
	/// @param col The column to be extracted.
	/// @param blobSize The blob size is returned here.
	/// @return A pointer to a blob.
	const void* Blob(int col, int& blobSize) throw (std::string);
	/// Enable and disable query tracing, where the sql queries are printed to stdout.
	void trace(bool on);
	/// @return The path to the database file
	std::string dbPath();

protected:
	/// Make sure that the requested column exists.
	/// @throws An error message if it doesn't.
	/// @param colType The expected sqlite column type (SQLITE_INTEGER, 
	/// SQLITE_FLOAT, SQLITE_BLOB, SQLITE_TEXT, SQLITE_TEXT3, or SQLITE_NULL)
	/// @param col The column of interest.
	void checkColumn(int colType, int col) throw (std::string);
	/// Determine the column types of _sqliteStmt and save in _colTypes.
	/// These are defined in sqlite3.h, and
	/// may be SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_BLOB, SQLITE_TEXT, 
	/// SQLITE_TEXT3, or SQLITE_NULL. The last one occurs when a column is not
	/// actually an actual table column.
	void determineColumnTypes();
	/// Initialize the map containing the sqlite type names.
	void initTypeNames();
	/// This function is called to finalize the current prepared
	/// sqlite statement, identified by _sqliteStmt. If statements
	/// are not finalized, resource leaks may occur. _sqliteStmt
	/// will be set to null.
	void finalize();
	
	/// Path to the spatialite database.
	std::string _dbPath;
	/// Handle for the spatialite database.
	sqlite3 *_dbHandle;
	/// The sql statement is built here.
	sqlite3_stmt* _sqliteStmt;
	/// The number of columns returned from the query.
	int _nColumns;
	/// The column type for each column returned from prepare(). These
	/// may be SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_BLOB, SQLITE_TEXT, 
	/// SQLITE_TEXT3, or SQLITE_NULL. The last one occurs when a column is not
	/// an actual table column.
	std::vector<int> _colTypes; 
	/// The names of the sqlite types.
	std::map<int,std::string> _sqliteTypeNames;
	/// The sqlite3_open_v2() open mode.
	int _mode;
	/// Set true to enable query tracing.
	bool _trace;

};

#endif
