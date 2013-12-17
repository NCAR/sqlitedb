#include "SQLiteDB.h"
#include <iostream>
////////////////////////////////////////////////////////////////////
SQLiteDB::SQLiteDB(std::string dbPath, bool delayInit, int mode) throw (std::string) :
	_dbPath(dbPath),
	_dbHandle(0),
	_sqliteStmt(0),
	_nColumns(0),
	_mode(mode),
	_trace(false)
{

	// initialize the sqlite type names vector
	initTypeNames();

	if (delayInit) {
		return;
	}

	init();
}

////////////////////////////////////////////////////////////////////
SQLiteDB::~SQLiteDB() {

	// finalize the previously prepared statement
	finalize();

	// close the database
	sqlite3_close(_dbHandle);
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::init() {
	int ret;

	ret = sqlite3_open_v2(_dbPath.c_str(), &_dbHandle, _mode, NULL);

	if (ret != SQLITE_OK) {
		std::stringstream s;
		s << "SQLiteDB: cannot open " << _dbPath << ", "
			<< sqlite3_errmsg(_dbHandle);
		throw(s.str());
	}

	// enable foreign keys
	exec("pragma foreign_keys=on;");
}

////////////////////////////////////////////////////////////////////
std::string SQLiteDB::dbPath() {
	return _dbPath;
}

////////////////////////////////////////////////////////////////////
std::string SQLiteDB::version() {
	std::stringstream s;
	s << "SQLite version: " << sqlite3_libversion();

	return s.str();
}

////////////////////////////////////////////////////////////////////
int SQLiteDB::user_version() {

	int version;

	// prepare the query
	prepare("pragma user_version;");

	if (step()) {
		version = Integer(0);
	}

	// finish the query
	finalize();

	return version;
}

////////////////////////////////////////////////////////////////////
std::vector<std::string> SQLiteDB::table_names() {

	std::vector<std::string> result;

	// prepare the query
	prepare("SELECT name FROM sqlite_master WHERE type='table';");

	int n = sqlite3_column_count(_sqliteStmt);

	while (step()) {
		for (int i = 0; i < n; i++) {
			result.push_back(Text(i));
		}
	}

	// finish the query
	finalize();

	return result;
}

////////////////////////////////////////////////////////////////////
std::vector<std::string> SQLiteDB::column_names(std::string table_name) {

	std::vector<std::string> result;

	// prepare the query
	prepare("Pragma table_info(" + table_name + ")");
	int n = sqlite3_column_count(_sqliteStmt);

	while (step()) {
			result.push_back(Text(1));
	}

	return result;
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::exec(std::string sql)throw (std::string) {

	if (_trace) {
		std::cout << "exec: " << sql << std::endl;
	}

    char *err_msg = NULL;
    int ret = sqlite3_exec(_dbHandle, sql.c_str(), NULL, NULL, &err_msg);

	if (ret != SQLITE_OK) {
		std::stringstream s;
		s << "SQLiteDB: prepare failed for sql statement \"" << sql << "\", "
			<< err_msg;
		sqlite3_free (err_msg);
		throw(s.str());
	}
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::exec(std::stringstream& sql)throw (std::string) {
	exec(sql.str());
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::prepare(std::string sql) throw (std::string) {

	if (_trace) {
		std::cout << "prepare: " << sql << std::endl;
	}

	// finalize the previously prepared statement
	finalize();

	int ret = sqlite3_prepare_v2(_dbHandle, sql.c_str(), sql.length(),
			&_sqliteStmt, NULL);
	if (ret != SQLITE_OK) {
		/* some error occurred */
		std::stringstream s;
		s << "SQLiteDB: prepare failed for sql statement:" << std::endl << sql << std::endl
				<< sqlite3_errmsg(_dbHandle);
		throw(s.str());
	}

	// find out how many columns our query will return
	_nColumns = sqlite3_column_count(_sqliteStmt);
}

////////////////////////////////////////////////////////////////////
bool SQLiteDB::step() {

	int ret = sqlite3_step(_sqliteStmt);

	if (ret == SQLITE_ROW) {
		// determine the column types, to be used later in
		// verifying data requests. This must be done after the first step()
		determineColumnTypes();

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////
std::string SQLiteDB::Text(int col) throw (std::string) {

	// throw an exception if the requested column doesn't exist
	checkColumn(SQLITE_TEXT, col);

	/// @todo Fix this to do the proper character set translation
	const unsigned char* s = sqlite3_column_text(_sqliteStmt, col);

	/// @todo fix this cast when I can access documentation
	std::string t = std::string((const char*) s);
	return (t);
}

////////////////////////////////////////////////////////////////////
int SQLiteDB::Integer(int col) throw (std::string) {

	// throw an exception if the requested column doesn't exist
	checkColumn(SQLITE_INTEGER, col);

	return (sqlite3_column_int(_sqliteStmt, col));
}

////////////////////////////////////////////////////////////////////
double SQLiteDB::Float(int col) throw (std::string) {

	// throw an exception if the requested column doesn't exist
	checkColumn(SQLITE_FLOAT, col);

	return (sqlite3_column_double(_sqliteStmt, col));
}

////////////////////////////////////////////////////////////////////
const void* SQLiteDB::Blob(int col, int& blobSize) throw (std::string) {

	// throw an exception if the requested column doesn't exist
	checkColumn(SQLITE_BLOB, col);

	const void* blob;

	blob = sqlite3_column_blob(_sqliteStmt, col);
	blobSize = sqlite3_column_bytes(_sqliteStmt, col);

	return blob;
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::finalize() {
	if (_sqliteStmt) {
		sqlite3_finalize(_sqliteStmt);
		_sqliteStmt = 0;
	}
}

////////////////////////////////////////////////////////////////////
int SQLiteDB::colType(int col) {
	if (col < 0 || col > _nColumns) {
		std::stringstream s;
		s << "SQLiteDB: requested column " << col << " is larger than the "
				<< _nColumns << " available columns";
		throw(s.str());
	}

	return _colTypes[col];
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::checkColumn(int colType, int col) throw (std::string) {

	if (col < 0 || col > _nColumns) {
		std::stringstream s;
		s << "SQLiteDB: requested column " << col << " is larger than the "
				<< _nColumns << " available columns";
		throw(s.str());
	}

	if (colType != _colTypes[col]) {
		std::stringstream s;
		/// @todo add the query to the error message
		s << "SQLiteDB: Requested data type (" <<
				_sqliteTypeNames[colType] <<
				") does not match actual column data type (" <<
				_sqliteTypeNames[_colTypes[col]] <<
				") for column " << col << " in query: " << std::endl
				<< sqlite3_sql(_sqliteStmt);
		throw(s.str());
	}

	return;
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::determineColumnTypes() {
	int n = sqlite3_column_count(_sqliteStmt);
	_colTypes.resize(n);
	for (int i = 0; i < n; i++) {
		_colTypes[i] = sqlite3_column_type(_sqliteStmt, i);
	}
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::initTypeNames() {
	_sqliteTypeNames[SQLITE_TEXT]    = std::string("SQLITE_TEXT");
	_sqliteTypeNames[SQLITE_FLOAT]   = std::string("SQLITE_FLOAT");
	_sqliteTypeNames[SQLITE_BLOB]    = std::string("SQLITE_BLOB");
	_sqliteTypeNames[SQLITE_INTEGER] = std::string("SQLITE_INTEGER");
	_sqliteTypeNames[SQLITE_NULL]    = std::string("SQLITE_NULL");
}

////////////////////////////////////////////////////////////////////
sqlite3* SQLiteDB::handle() {
	return _dbHandle;
}

////////////////////////////////////////////////////////////////////
void SQLiteDB::trace(bool on) {
	_trace = on;
}
