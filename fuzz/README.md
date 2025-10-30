we fuzz 😎

to build : 
	cd ppsspp
	mkdir build-fuzz 
	cmake -S . -B build-fuzz   -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++   -DCMAKE_BUILD_TYPE=RelWithDebInfo   -DUSE_SDL=OFF -DUSING_VULKAN=OFF -DUSING_GLES2=OFF -DUSING_EGL=OFF
	cmake --build build-fuzz --target fuzz_kernel_load_elf -j$(nproc)
