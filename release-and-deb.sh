#!/usr/bin/bash

OUT_PATH=$1
TARGET_DIR=$2
CONTROL_PATH=$3

cmake -DCMAKE_BUILD_TYPE=Release -B $OUT_PATH -S .
cmake --build $OUT_PATH

cqtdeployer -bin $OUT_PATH/RichTextEditor -qmake /usr/bin/qmake -targetDir $TARGET_DIR -deb $CONTROL_PATH
