for i in CMakeCache.txt CMakeFiles cmake_install.cmake gameplay Makefile samples config.h openal.pc;
do 
    rm -rf $i;
done

rm install_manifest.txt
