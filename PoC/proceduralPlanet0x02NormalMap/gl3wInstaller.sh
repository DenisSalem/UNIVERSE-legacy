#! /bin/bash

echo "Installing gl3w..."
if type "git" &> /dev/null; then
  git clone "http://github.com/skaslev/gl3w"
  cd gl3w/
  ./gl3w_gen.py
  if find "/usr/include/GL/gl3w.h" "/usr/include/GL/glcorearb.h" &> /dev/null; then
    echo "gl3w is already installed :)"
  else
    cp -v include/GL/* /usr/include/GL
  fi
  cp -v src/gl3w.c ..
  chmod +w ../gl3w.c
  cd ..
  rm -rf gl3w/
else
  echo "git not found..."
  echo "Aborting..."
fi

