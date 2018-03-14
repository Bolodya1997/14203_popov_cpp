Use 'make' on Linux, Mac OS; use 'mingw32-make.exe' on Windows.

		*** BUILD TESTS ***
cmake -DCMAKE_BUILD_TYPE=Optimized . -Bbuild
cd build
make install

		*** RUN TESTS ***
bin/test_lru

		*** MAKE COVERAGE ***
cmake -DCMAKE_BUILD_TYPE=Coverage . -Bbuild
cd build
make
make LRU_coverage

		*** WATCH COVERAGE ***
open build/LRU_coverage/index.html in your browser
