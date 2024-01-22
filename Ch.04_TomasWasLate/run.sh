#!/bin/bash
for cpp in *.cpp; do
    g++ -c "$cpp"
done
g++ *.o -o sfml-app.so -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
./sfml-app.so
