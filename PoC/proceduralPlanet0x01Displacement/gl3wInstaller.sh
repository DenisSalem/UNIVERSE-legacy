#! /bin/bash

echo "Installing gl3w..."
if type "git" &> /dev/null; then
  git clone "http://github.com/skaslev/gl3w"
  cd gl3w/
  ./gl3w_gen.py
  cp -v include/GL/* /usr/include/GL
  cp -v src/gl3w.c ..
  chmod +w ../gl3w.c
  cd ..
  rm -rf gl3w/
else
  echo "git not found..."
  echo "Aborting..."
fi

