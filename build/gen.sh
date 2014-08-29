. env.sh
cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain.cmake . -G 'Unix Makefiles' \
    -DCMAKE_BUILD_TYPE=debug \
    -DCMAKE_INSTALL_PREFIX=$PWD/dist
