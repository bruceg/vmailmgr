#!/bin/sh

newfile() {
  local old=$1
  local new=${old}.new
  cat >$new
  if cmp -s $old $new; then
    echo "$old is unchanged." >&2
    rm -f $new
  else
    echo "$old was changed, REPLACING." >&2
    mv -f $new $old
  fi
}

{
  sed -e '/^%%LIST%%$/,$d' showvconfig.cc.in
  while read type ext name default; do
    filename=`echo $name | sed -e 's/_/-/g'`
    sed -e "s|%NAME%|$name|g" \
	-e "s|%EXT%|$ext|g" \
	-e "s|%TYPE%|$type|g" \
	-e "s|%DEFAULT%|$default|g" \
	-e "s|%FILENAME%|$filename|g" \
	configvar.in | newfile _${name}.cc
    echo "  show_$ext(\"${filename}\", config->${name}());"
  done <configrc.in
  sed -e '1,/^%%LIST%%$/d' showvconfig.cc.in
} | newfile showvconfig.cc

{
  sed -e '/^%%LIST%%$/,$d' configrc.h.in
  while read type ext name default; do
    echo "public: $type $name() const;"
    echo "private: config_cache<$type> ${name}_cache;"
  done <configrc.in
  sed -e '1,/^%%LIST%%$/d' configrc.h.in
} | newfile configrc.h

{
  sed -e '/^%%LIST%%$/,$d' Makefile.am.in
  while read type ext name default; do
    echo "	_${name}.cc \\"
  done <configrc.in
  sed -e '1,/^%%LIST%%$/d' Makefile.am.in
} | newfile Makefile.am
