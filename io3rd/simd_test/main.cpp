#include <cstdio>
#include <cstdint>
#include <type_traits>
#include <iostream>

#ifdef __OPTIMIZE__
    #include <immintrin.h>
    #define KEWB_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define __OPTIMIZE__
    #include <immintrin.h>
    #undef __OPTIMIZE__
    #define KEWB_FORCE_INLINE inline
#endif

using rf_512 = __m512;
using ri_512 = __m512i;
using mask_512 = uint32_t;

KEWB_FORCE_INLINE rf_512 fill_value(float value) {
    return _mm512_set1_ps(value);
}

int main() {
    rf_512 value = fill_value(2.5);
    std::cout << value[0] << std::endl;
    return 0;
}
