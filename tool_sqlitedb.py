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

tools = ['doxygen']
env = Environment(tools = ['default'] + tools)
thisdir = env.Dir('.').srcnode().abspath

# define the tool
def sqlitedb(env):
    options = env.GlobalVariables()
    options.AddVariables(PathVariable('SPATIALITEDIR', 'SpatiaLite installation root.', None))
    options.Update(env)
    env.AppendUnique(CPPPATH=[thisdir,])
    env.Append(LIBPATH=[thisdir,])
    if env.has_key('SPATIALITEDIR'):
        env.AppendUnique(CPPPATH=[env['SPATIALITEDIR']+'/include',])
        env.AppendUnique(LIBPATH=[env['SPATIALITEDIR']+'/lib',])
    env.Append(LIBS = ['sqlitedb', 'spatialite', 'sqlite3'] )
    env.Require(tools)

Export('sqlitedb')

# build the SQLiteDB library

# just call sqlitedb() in order to get spatialite details
sqlitedb(env)

libsources = Split("""
SQLiteDB.cpp
""")

headers = Split("""
SQLiteDB.h
""")

libsqlitedb = env.Library('sqlitedb', libsources)
env.Default(libsqlitedb)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'SQLiteDB', 'PROJECT_NUMBER':'1.0'})



