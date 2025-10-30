add_executable(fuzz_kernel_load_elf EXCLUDE_FROM_ALL
  fuzz_sceKernelModule.cpp
  ppsspp_stubs.cpp
)

# IMPORTANT: we need the bridge compiled into Core (where __KernelLoadELFFromPtr lives)
# So add FUZZING_BUILD so the bridge symbol is emitted.
target_compile_definitions(Core PRIVATE FUZZING_BUILD)

# Link the minimum. Start with Core only.
target_link_libraries(fuzz_kernel_load_elf PRIVATE Core)

# Sanitizers + libFuzzer; as-needed stops dragging unused libs like native/SDL
target_compile_options(fuzz_kernel_load_elf PRIVATE
  -fsanitize=fuzzer,address,undefined -g -O1 -fno-omit-frame-pointer)
target_link_options(fuzz_kernel_load_elf PRIVATE
  -fsanitize=fuzzer,address,undefined
  -Wl,--as-needed
  -Wl,--allow-multiple-definition   # neutralize any stray duplicate symbols (e.g., main in static libs)
)
