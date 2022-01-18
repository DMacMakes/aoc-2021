#!/bin/sh
if [ $# -eq 0 ]; then
  echo "Error: New project name wasn't provided."
  echo "Usage: ./renameProject.sh new-name"
  exit 1
fi

old_name=$(ls *.sln | sed -e ''s/\.sln$//'')
new_name=$1
echo "Renaming $old_name to $new_name"

#./rerename ''/$old_name/$new_name/'' *

./rename ''s/$old_name/$new_name/'' *
sed -i ''s/$old_name/$new_name/g'' *.sln *.vcxproj* Makefile .gitignore .replit
