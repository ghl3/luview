#!/bin/bash

lua="."
luabin=$(pwd)/bin

PATH=${PATH/${luabin}:/""} # Remove lua/bin from path if it exists

export PATH=${luabin}:$PATH
export LUA_CPATH=$(pwd)/lib/?.so
export LUA_HOME=$(pwd)

echo "*** run with 'source' to export these environment variables for build: ***"
echo "PATH=${PATH}"
echo "LUA_CPATH=${LUA_CPATH}"
echo "LUA_HOME=${LUA_HOME}"