# SQLiteDB scons tool
#
# It builds the library for the SQLiteDB C++ class library,
# and provides CPP and linker specifications for the header 
# and libraries..
#
# Note that it uses the version of sqlite3 which is packaged
# in the SpatiaLite amalgamation. For that reason, the 
# SpatiaLite headers and library must be available.
# Set the SPATIALITEDIR variable in config.py to specify
# the top level directory (above include/ and lib/)
# for SpatiaLite, if these are not installed in standard 
# locations such as /usr/lib or /usr/local/lib.

import os
import sys

tools = ['doxygen', 'prefixoptions']
env = Environment(tools = ['default'] + tools)
thisdir = env.Dir('.').srcnode().abspath

# define the tool
def sqlitedb(env):
    env.AppendUnique(CPPPATH=[thisdir,])
    env.Append(LIBPATH=[thisdir,])
    env.Append(LIBS = ['sqlitedb', 'sqlite3'] )
    if sys.platform == 'darwin':
        # use the homebrew supplied sqlite on osx.
        env.AppendUnique(LIBPATH=['/usr/local/opt/sqlite/lib',])
        env.AppendUnique(CPPPATH=['/usr/local/opt/sqlite/include',])
    env.Require(tools)

Export('sqlitedb')

# build the SQLiteDB library

# just call sqlitedb() in order to get spatialite details
sqlitedb(env)

libsources = Split("""
SQLiteDB.cpp
SQLSchema.cpp
""")

headers = Split("""
SQLiteDB.h
SQLSchema.h
""")

libsqlitedb = env.Library('sqlitedb', libsources)
env.Default(libsqlitedb)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'SQLiteDB', 'PROJECT_NUMBER':'1.0'})



