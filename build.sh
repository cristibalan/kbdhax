#!/bin/bash
rm kbd
gcc -Wall -o kbd kbd.c -framework ApplicationServices -arch i386 -arch ppc
rm ~/bin/chelu/kbd
cp kbd ~/bin/chelu/
