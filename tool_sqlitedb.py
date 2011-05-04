import os
import sys

import eol_scons
tools = ['doxygen']
env = Environment(tools = ['default'] + tools)
platform = env['PLATFORM']
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
    env.AppendUnique(CPPPATH=[thisdir,])
    env.AppendLibrary('sqlitedb')
    env.AppendLibrary('spatialite')
    env.AppendUnique(CPPDEFINES=['SPATIALITE_AMALGAMATION',])
    env.AppendDoxref('SQLiteDB')
    env.Replace(CCFLAGS=['-g','-O2'])
    env.Require(tools)

Export('sqlitedb')
