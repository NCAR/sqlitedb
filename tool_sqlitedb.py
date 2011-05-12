import os
import sys

import eol_scons
tools = ['doxygen']
env = Environment(tools = ['default'] + tools)
thisdir = env.Dir('.').srcnode().abspath

def sqlitedb(env):
    options = env.GlobalVariables()
    options.AddVariables(PathVariable('SPATIALITEDIR', 'SpatiaLite installation root.', None))
    options.Update(env)
    env.AppendUnique(CPPPATH=[thisdir,])
    if env.has_key('SPATIALITEDIR'):
        env.AppendUnique(CPPPATH=[env['SPATIALITEDIR']+'/include',])
        env.AppendUnique(LIBPATH=[env['SPATIALITEDIR']+'/lib',])
    env.AppendLibrary('sqlitedb')
    env.AppendLibrary('spatialite')
    env.AppendUnique(CPPDEFINES=['SPATIALITE_AMALGAMATION',])
    env.Replace(CCFLAGS=['-g','-O2'])
    env.Require(tools)

Export('sqlitedb')

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
env.AppendDoxref('SQLiteDB')
env.Default(html)



