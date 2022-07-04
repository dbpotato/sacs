#!/bin/bash

if [[ -z "${ANDROID_SDK_ROOT-}" ]]; then
    export ANDROID_SDK_ROOT="" #provide path to sdk directory
fi

if [[ -z "${ANDROID_NDK_ROOT-}" ]]; then
    export ANDROID_NDK_ROOT="" #provide path to ndk directory
fi


SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)
JNI_OUTPUT="$SCRIPT_DIR/jni/build/libsacs_jni.so"
AAR_OUTPUT="$SCRIPT_DIR/aar_lib/sacs/build/outputs/aar/sacs-release.aar"

if [[ -z "${ANDROID_SDK_ROOT-}" ]]; then
    echo "ANDROID_SDK_ROOT is not set. Edit this script and try again.";
    exit 1;
fi

if [[ -z "${ANDROID_NDK_ROOT-}" ]]; then
    echo "ANDROID_NDK_ROOT is not set. Edit this script and try again.";
    exit 1;
fi
    

cd $SCRIPT_DIR/jni
cmake ./
make -j 99

if [ $? -eq 0 ]; then
   echo "Compiled Sacs JNI library at : $JNI_OUTPUT"
else
   echo "Failed to create JNI native library"
   exit 1;
fi

mkdir -p $SCRIPT_DIR/aar_lib/sacs/src/main/jniLibs/armeabi-v7a
cp $JNI_OUTPUT $SCRIPT_DIR/aar_lib/sacs/src/main/jniLibs/armeabi-v7a

cd $SCRIPT_DIR/aar_lib
./gradlew assembleRelease

if [ $? -eq 0 ]; then
   echo "Compiled Sacs AAR library at : $AAR_OUTPUT";
else
   echo "Failed to create Sacs AAR library"
   exit 1;
fi
