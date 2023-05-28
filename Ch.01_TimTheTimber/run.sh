#!/bin/bash
g++ -c main.cpp
g++ main.o -o sfml-app.so -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
./sfml-app.so
