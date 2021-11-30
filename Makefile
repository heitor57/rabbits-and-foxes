all: release
clean:
	cmake --build ./build --target clean
	rm -rf build
release:
	mkdir -p build &&	cd build &&	cmake -DCMAKE_BUILD_TYPE=Release .. &&	cmake --build .
release_time_debug:
	mkdir -p build &&	cd build &&	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-DTIME_DEBUG" .. &&	cmake --build .
debug:
	mkdir -p build &&	cd build &&	cmake -DCMAKE_BUILD_TYPE=Debug .. &&	cmake --build .
