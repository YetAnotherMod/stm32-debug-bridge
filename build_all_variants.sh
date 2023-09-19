if [[ "$1" == "" ]]
then
    BUILD=build
else
    BUILD=$1
fi
if [[ "$2" == "" ]]
then
    JOBS=1
else
    JOBS=$2
fi
rm -rf ${BUILD}
for platform in $(ls platform)
do
    for type in "none" "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    do
        cmake -B ${BUILD}/${platform}/${type} -DPLATFORM=${platform} -DCMAKE_BUILD_TYPE=${type} || exit 1
        cmake --build ${BUILD}/${platform}/${type} --target firmware.elf -j ${JOBS} || exit 1
    done
done
