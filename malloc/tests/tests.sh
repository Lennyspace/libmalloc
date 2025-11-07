#!/bin/sh

###########################################
#   LENNY TAURINES PRIME      TESTSUITE   #
###########################################

#----------------- COLOR -----------------#
# 0 - No style | 1 - Bold
RED="\e[0;31m"
BRED="\e[1;31m"
GRN="\e[0;32m"
BGRN="\e[1;32m"
YEL="\e[0;33m"
BYEL="\e[1;33m"
BLU="\e[0;34m"
BBLU="\e[1;34m"
PUR="\e[0;35m"
BPUR="\e[1;35m"
CYA="\e[0;36m"
BCYA="\e[1;36m"
WHI="\e[0;37m"
BWHI="\e[1;37m"
GRE="\e[2;37m"

#----------------- TESTS -----------------#

tit_wrap() {
  echo -e "$@ $WHI"
}

func_test() {
  tit_wrap - $PUR "$1"

  out1="/tmp/out1.$$"
  eval "$*" >"$out1"
  exit1="$?"

  out2="/tmp/out2.$$"
  eval "LD_PRELOAD=./libmalloc.so $*" >"$out2"
  exit2="$?"

  if [ ${exit1} -ne ${exit2} ]; then
    tit_wrap ${RED} "ton malloc n as pas marcher pour cette commande"
  elif cmp -s "$out1" "$out2"; then
    tit_wrap ${GRN} "ALL GOOD !"
  else
    tit_wrap ${RED} "PAS LA MEME SORTIE AVEC TON MALLOC"
  fi

  rm -f "$out1" "$out2"

}

func_file() {
  cd tests/command_test
  while IFS= read -r line || [ -n "$line" ]; do
    cd ../..
    func_test "$line"
    rm malloc.tar
    cd tests/command_test

  done <"$1"
}

func_file "command.txt"
