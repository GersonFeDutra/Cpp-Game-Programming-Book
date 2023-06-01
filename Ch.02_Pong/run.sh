#!/bin/bash
g++ -c main.cpp
g++ -c Bat.cpp
g++ main.o Bat.o -o sfml-app.so -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
./sfml-app.so
