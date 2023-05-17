#!/bin/bash
g++ -c main.cpp
g++ main.o -o sfml-app.so -lsfml-graphics -lsfml-window -lsfml-system
./sfml-app.so
