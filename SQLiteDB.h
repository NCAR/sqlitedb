#ifndef SQLITEDB_INC
#define SQLITEDB_INC

#ifdef SPATIALITE_AMALGAMATION
#include <spatialite/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <sstream>

class SQLiteDB {

public:
	SQLiteDB(std::string dbPath, bool delayInit=false) throw (std::string);
	virtual ~SQLiteDB();
	void init();
	/// @return A string with version information
	std::string version();
	/// Get the table names.
	/// @return The table names.
	std::vector<std::string> table_names();
	/// Get the column names for a table.
	/// @param table_name The table of interest
	/// @return The names of columns in this table
	std::vector<std::string> column_names(std::string table_name);
	/// Execute an sql statement.
	/// @param sql The SQL statement
	/// @throws A string with an error message if the exec() failes.
	void exec(std::string sql) throw (std::string);
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
	/// finish using (finalize) the sqlite prepared statement.
	void finalize();

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

};

#endif
