#!/bin/bash
rm kbd
gcc -Wall -o kbd kbd.c -framework ApplicationServices -arch x86_64
rm ~/bin/chelu/kbd
cp kbd ~/bin/chelu/
