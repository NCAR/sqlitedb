import os
import sys

import eol_scons
tools = ['doxygen']
env = Environment(tools = ['default'] + tools)

libsources = Split("""
SQLiteDB.cpp
""")

headers = Split("""
SQLiteDB.h
""")

libsqlitedb = env.Library('sqlitedb', libsources)

html = env.Apidocs(libsources + headers,  DOXYFILE_DICT={'PROJECT_NAME':'SQLiteDB', 'PROJECT_NUMBER':'1.0'})
env.Default(html)

env.Default(libsqlitedb)

thisdir = env.Dir('.').srcnode().abspath

def sqlitedb(env):
    env.AppendUnique(CPPPATH=['/opt/local/include',]) 
    env.AppendUnique(LIBPATH=['/opt/local/lib',])
    env.AppendUnique(LIBS   =['sqlite3',])
    env.AppendUnique(CPPPATH=[thisdir,])
    env.AppendLibrary('sqlitedb')
    env.AppendDoxref('SQLiteDB')
    env.Require(tools)

Export('sqlitedb')
