#!/bin/bash

for file in ./*.png; do
    xxd -i $file > "../include/img/${file%.*}.h"
done
