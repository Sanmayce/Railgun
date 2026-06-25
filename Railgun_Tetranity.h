//#include <immintrin.h>
//#include <stdint.h>
//#include <string.h>
//#include <stddef.h>

void *Railgun_Tetranity_AVX2(const unsigned char *restrict h, size_t h_len, const unsigned char *restrict n, size_t n_len) {
    // ---------------------------------------------------------
    // FASTPATH 1: Nano-Needles & Edge Cases
    // ---------------------------------------------------------
    if (n_len == 0) return (void *)h;
    if (n_len > h_len) return NULL;
    
    // 1-Vector Sieve (1 byte)
    if (n_len == 1) {
        __m256i ymm_v1 = _mm256_set1_epi8((char)n[0]);
        size_t shift = 0;
        const size_t limit = h_len - 1;
        while (shift <= limit) {
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift));
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(h1, ymm_v1));
            if (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                if (shift + tz <= limit) return (void *)(h + shift + tz);
            }
            shift += 32;
        }
        return NULL;
    }

    // ---------------------------------------------------------
    // FASTPATH 2: 2-Vector Sieve Fallback (Exactly 2 bytes)
    // ---------------------------------------------------------
    if (n_len == 2) {
        __m256i ymm_v1 = _mm256_set1_epi8((char)n[1]);
        __m256i ymm_v2 = _mm256_set1_epi8((char)n[0]);
        size_t shift = 0;
        const size_t limit = h_len - 2;
        while (shift <= limit) {
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + 1));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + 0));
            __m256i mask = _mm256_and_si256(_mm256_cmpeq_epi8(h1, ymm_v1), _mm256_cmpeq_epi8(h2, ymm_v2));
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(mask);
            if (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                if (shift + tz <= limit) return (void *)(h + shift + tz);
            }
            shift += 32;
        }
        return NULL;
    }

    // ---------------------------------------------------------
    // FASTPATH 3: TRINITY (Exactly 3 bytes)
    // ---------------------------------------------------------
    if (n_len == 3) {
        // For exactly 3 bytes, passing a 3-vector sieve is a guaranteed 100% match.
        // No scalar fallback or verification is needed.
        __m256i ymm_v1 = _mm256_set1_epi8((char)n[2]);
        __m256i ymm_v2 = _mm256_set1_epi8((char)n[1]);
        __m256i ymm_v3 = _mm256_set1_epi8((char)n[0]);
        
        size_t shift = 0;
        const size_t limit = h_len - 3;
        
        while (shift <= limit) {
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + 2));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + 1));
            __m256i h3 = _mm256_loadu_si256((const __m256i*)(h + shift + 0));
            
            __m256i m1 = _mm256_cmpeq_epi8(h1, ymm_v1);
            __m256i m2 = _mm256_cmpeq_epi8(h2, ymm_v2);
            __m256i m3 = _mm256_cmpeq_epi8(h3, ymm_v3);
            
            __m256i mask = _mm256_and_si256(_mm256_and_si256(m1, m2), m3);
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(mask);
            
            if (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                if (shift + tz <= limit) return (void *)(h + shift + tz);
            }
            shift += 32;
        }
        return NULL;
    }

    // =========================================================
    // THE TETRANITY SETUP (n_len >= 4)
    // Extracts 4 distinct right-to-left anchor points 
    // =========================================================
    size_t idx1 = n_len - 1; 
    size_t idx2 = n_len - 2;

    while (idx2 > 2 && n[idx2] == n[idx1]) idx2--; 
    
    size_t idx3 = (idx2 > 1) ? idx2 - 1 : 1;
    while (idx3 > 1 && (n[idx3] == n[idx1] || n[idx3] == n[idx2])) idx3--;
    
    size_t idx4 = (idx3 > 0) ? idx3 - 1 : 0;
    while (idx4 > 0 && (n[idx4] == n[idx1] || n[idx4] == n[idx2] || n[idx4] == n[idx3])) idx4--;

    __m256i ymm_v1 = _mm256_set1_epi8((char)n[idx1]);
    __m256i ymm_v2 = _mm256_set1_epi8((char)n[idx2]);
    __m256i ymm_v3 = _mm256_set1_epi8((char)n[idx3]);
    __m256i ymm_v4 = _mm256_set1_epi8((char)n[idx4]);

    size_t shift = 0;
    const size_t limit = h_len - n_len;

    // ---------------------------------------------------------
    // FASTPATH 4: Micro-Tetranity (4 <= n_len <= 32)
    // ---------------------------------------------------------
    if (n_len <= 32) {
        char needle_pad[32] = {0};
        memcpy(needle_pad, n, n_len);
        __m256i ymm_needle = _mm256_loadu_si256((const __m256i*)needle_pad);
        uint32_t verify_bitmask = (n_len == 32) ? 0xFFFFFFFF : (1U << n_len) - 1;

        while (shift <= limit) {
            // Tetranity: 4 loads mapping to L1 Cache/Load Ports
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + idx1));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + idx2));
            __m256i h3 = _mm256_loadu_si256((const __m256i*)(h + shift + idx3));
            __m256i h4 = _mm256_loadu_si256((const __m256i*)(h + shift + idx4));
            
            // Quadruple-Intersection Sieve
            __m256i m1 = _mm256_cmpeq_epi8(h1, ymm_v1);
            __m256i m2 = _mm256_cmpeq_epi8(h2, ymm_v2);
            __m256i m3 = _mm256_cmpeq_epi8(h3, ymm_v3);
            __m256i m4 = _mm256_cmpeq_epi8(h4, ymm_v4);
            
            // Branchless logical AND sequence
            __m256i mask_12 = _mm256_and_si256(m1, m2);
            __m256i mask_34 = _mm256_and_si256(m3, m4);
            __m256i final_mask = _mm256_and_si256(mask_12, mask_34);
            
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(final_mask);

            while (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                size_t cand_shift = shift + tz;

                if (cand_shift > limit) return NULL; 

                // Inline O(1) full vector validation for needles 4 to 32 bytes
                __m256i v_h = _mm256_loadu_si256((const __m256i*)(h + cand_shift));
                __m256i v_cmp = _mm256_cmpeq_epi8(v_h, ymm_needle);
                if (((uint32_t)_mm256_movemask_epi8(v_cmp) & verify_bitmask) == verify_bitmask) {
                    return (void *)(h + cand_shift);
                }
                current_mask &= (current_mask - 1);
            }
            shift += 32;
        }
        return NULL;
    }

    // Scalar (n_len > 32) with BMH Pseudo-Order 4 strengthened by 'Swampwalker' Heuristic with Primalization/Factorization order 4: [[[

    //void *Railgun_Trinity_AVX2(const unsigned char *restrict h, size_t h_len, const unsigned char *restrict n, size_t n_len) {
    //char * Railgun_Trolldom_64 (char * pbTarget, char * pbPattern, uint64_t cbTarget, uint32_t cbPattern) {

    char * pbTarget = h;
    char * pbPattern = n;
    uint64_t cbTarget = h_len;
    uint32_t cbPattern = n_len;

    char * pbTargetMax = pbTarget + cbTarget;
    uint32_t ulHashPattern;
    signed long long count;
    unsigned char bm_Horspool_Order2[256*256];
    //unsigned char bm_Horspool_Order2bitwise[(256*256)>>3];
    uint64_t i, Gulliver;
    uint64_t PRIMALposition, PRIMALpositionCANDIDATE;
    uint64_t PRIMALlength, PRIMALlengthCANDIDATE;
    uint64_t j, FoundAtPosition;
    register uint32_t ulHashTarget;

    // Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) [
    // Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
    // Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=29 PRIMALlength=04  'vvvv'
    // Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=08 PRIMALlength=20  'vvvBOOMSHAKALAKAvvvv'
    // Needle: Trollland                                    PRIMALposition=01 PRIMALlength=09  'Trollland'
    // Needle: Swampwalker                                  PRIMALposition=01 PRIMALlength=11  'Swampwalker'
    // Needle: licenselessness                              PRIMALposition=01 PRIMALlength=15  'licenselessness'
    // Needle: alfalfa                                      PRIMALposition=02 PRIMALlength=06  'lfalfa'
    // Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=08  'Sandokan'
    // Needle: shazamish                                    PRIMALposition=01 PRIMALlength=09  'shazamish'
    // Needle: Simplicius Simplicissimus                    PRIMALposition=06 PRIMALlength=20  'icius Simplicissimus'
    // Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=32  'domilliaquadringenquattuorquinqu'
    // Needle: boom-boom                                    PRIMALposition=02 PRIMALlength=08  'oom-boom'
    // Needle: vvvvv                                        PRIMALposition=01 PRIMALlength=04  'vvvv'
    // Needle: 12345                                        PRIMALposition=01 PRIMALlength=05  '12345'
    // Needle: likey-likey                                  PRIMALposition=03 PRIMALlength=09  'key-likey'
    // Needle: BOOOOOM                                      PRIMALposition=03 PRIMALlength=05  'OOOOM'
    // Needle: aaaaaBOOOOOM                                 PRIMALposition=02 PRIMALlength=09  'aaaaBOOOO'
    // Needle: BOOOOOMaaaaa                                 PRIMALposition=03 PRIMALlength=09  'OOOOMaaaa'
    PRIMALlength=0;
    for (i=0+(1); i < cbPattern-((4)-1)+(1)-(1); i++) { // -(1) because the last BB (Building-Block) order 4 has no counterpart(s)
        FoundAtPosition = cbPattern - ((4)-1) + 1;
        PRIMALpositionCANDIDATE=i;
        while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
            j = PRIMALpositionCANDIDATE + 1;
            while ( j <= (FoundAtPosition-1) ) {
                if ( *(uint32_t *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(uint32_t *)(pbPattern+j-(1)) ) FoundAtPosition = j;
                j++;
            }
            PRIMALpositionCANDIDATE++;
        }
        PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+1 +((4)-1);
        if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
        if (cbPattern-i+1 <= PRIMALlength) break;
        if (PRIMALlength > 31) break;
        //printf("i, PRIMALlength: %d, %d \n", i,PRIMALlength); //dbg
    }
    // Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) ]

    PRIMALlengthCANDIDATE = cbPattern;
    cbPattern = PRIMALlength;
    pbPattern = pbPattern + (PRIMALposition-1);

    ulHashPattern = *(uint32_t *)(pbPattern);
    for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
    for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>(16-1))+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
    i=0;
    while (i <= cbTarget-cbPattern) {
        Gulliver = 1;
        if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) {
            if ( \
            ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
            || ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
            ) Gulliver = cbPattern-(2-1)-2-4-2 +1; else {
                if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
                    count = cbPattern-4+1;
                    while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) ) count = count-4;
                    if (cbPattern != PRIMALlengthCANDIDATE) {
                        if ( count <= 0 ) {
                            if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) {
                                if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) {
                                    count = PRIMALlengthCANDIDATE-4+1;
                                    while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) ) count = count-4;
                                    if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));    
                                }
                            }
                        }
                    } else {
                        if ( count <= 0 ) return(pbTarget+i);
                    }
                }
            }
        } else Gulliver = cbPattern-(2-1)-2;
        i = i + Gulliver;
    }
    return(NULL);
    // Scalar (n_len > 32) with BMH Pseudo-Order 4 strengthened by 'Swampwalker' Heuristic with Primalization/Factorization order 4: ]]]
}
