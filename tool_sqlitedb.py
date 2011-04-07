import os
import sys

import eol_scons
tools = ['doxygen']
env = Environment(tools = ['default'] + tools)
platform = env['PLATFORM']
if platform == 'win32':
    env.AppendUnique(CPPDEFINES=['SPATIALITE_AMALGAMATION',])

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
    env.AppendUnique(CPPPATH=[thisdir,])
    env.AppendLibrary('sqlitedb')
    # on windows, the sqlite library is wrapped in spatialite
    if platform != 'win32':
        env.AppendLibrary('sqlite3')
    else:
        env.AppendLibrary('spatialite')
    if platform == 'win32':
        env.AppendUnique(CPPDEFINES=['SPATIALITE_AMALGAMATION',])

    env.AppendDoxref('SQLiteDB')

    env.Require(tools)

Export('sqlitedb')
