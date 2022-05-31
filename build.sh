#!/bin/bash

if [ "$(whoami)" = "root" ]; then
  echo "  Please don't run as root/using sudo..."
  exit 1
fi

PREVCC="$CC"
PREVCXX="$CXX"

if command -v clang &> /dev/null
then
    echo "-- Clang found on system, great! Long live LLVM! :D"
    export CC=clang
    export CXX=clang++
fi

REPO=$(dirname "$0")
rm -rf "$REPO"/out
mkdir -p "$REPO"/out && cd "$REPO"/out || exit 2

cmake -DBUILD_SHARED_LIBS=Off -DCMAKE_BUILD_TYPE=Release ../src || exit 3

cmake --build . --config Release || exit 4

export CC="$PREVCC"
export CXX="$PREVCXX"

cd "$REPO" || exit 5

echo "  Done. Exported build into $REPO/out"
