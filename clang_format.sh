#!/bin/bash
case "$OSTYPE" in
  solaris*) echo "SOLARIS" ;;
  darwin*)  echo "OSX" ;; 
  linux*)   echo "LINUX" ;;
  bsd*)     echo "BSD" ;;
  msys*)    echo "WINDOWS" ;;
  cygwin*)  echo "ALSO WINDOWS" ;;
  *)        echo "unknown: $OSTYPE" ;;
esac

# Regexp for grep to only choose some file extensions for formatting
exts="\.\(cpp\|hpp\|cc\|hh\|c\|h\)$"
 
# The formatter to use
formatter=`clang-format`
 
# Check availability of the formatter
if [ -z "$formatter" ]
then
1>&2 echo "$formatter not found. Pre-commit formatting will not be done."
exit 0
fi
 
# Format staged files
for file in `git diff --cached --name-only --diff-filter=ACMR | grep $exts`
do
echo "Formatting $file"
# Get the file from index
git show ":$file" > "$file.tmp"
# Format it
"$formatter" -i "$file.tmp"
# Create a blob object from the formatted file
hash=`git hash-object -w "$file.tmp"`
# Add it back to index
git update-index --add --cacheinfo 100644 "$hash" "$file"
 
# Formatting original file
# echo "Formatting original file "."$file"
"$formatter" -i "$file"
 
# Remove the tmp file
rm "$file.tmp"
done
 
# If no files left in index after formatting - fail
ret=0
if [ ! "`git diff --cached --name-only`" ]; then
1>&2 echo "No files left after formatting"
exit 1
fi
