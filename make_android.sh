CMAKE_VERSION=`ls $android_home/cmake`
CMAKE_DIR=$android_home/cmake/$CMAKE_VERSION
CMAKE_BIN=$CMAKE_DIR/bin/cmake
CMAKE_NINJA=$CMAKE_DIR/bin/ninja
CMAKE_TOOLCHAIN=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake
STL=gnustl_shared
CMAKE_NDK_DEFINES="-DANDROID_NDK=$ANDROID_NDK_HOME"
CMAKE_ANDROID_DEFINES="$CMAKE_NDK_DEFINES -DCMAKE_MAKE_PROGRAM=$CMAKE_NINJA -DANDROID_ABI=armeabi-v7a -DANDROID_NATIVE_API_LEVEL=24 -DANDROID_PLATFORM=android-24 -DANDROID_TOOLCHAIN=clang -DANDROID_STL=$STL -DANDROID_CPP_FEATURES=rtti;exceptions"

function build()
{
	set -e
	$CMAKE_BIN -G"Android Gradle - Ninja" -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN -DCMAKE_BUILD_TYPE=$1 $CMAKE_ANDROID_DEFINES -HSource -BBuildAndroid -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=STL_$1
	$CMAKE_BIN --build BuildAndroid --config $1 --target 3.Triangle
}

build Debug;