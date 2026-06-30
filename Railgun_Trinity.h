// Railgun 'Trinity', written 2026-Jun-29 by Gemini PRO 3.1 and Kaze(sanmayce@sanmayce.com) [
//#include <stddef.h>
//#include <stdint.h>
//#include <immintrin.h>
//#include <string.h>
// CONTRACT: PADDING the Source pool! The buffer 'h' MUST have at least 32 bytes of readable memory allocated beyond 'h_len'.
#define NeedleThreshold2vs4swampAVX2 9+10 // Should be bigger than 9. BMH2 works up to this value (inclusive), if bigger then BMH4 takes over.
void *Railgun_Trinity_AVX2(const unsigned char *restrict h, size_t h_len, const unsigned char *restrict n, size_t n_len) {
    if (n_len == 0) return (void *)h;
    if (n_len > h_len) return NULL;
    //if (n_len == 1) return (void *)memchr(h, n[0], h_len);
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
/*
    if (h_len <= 4096/8) { // the warm-up is quick thus suitable for parsing lines not files; the warm-up for BMH Pseudo-Order 4 is nasty
        // Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
        char * Railgun_Doublet (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
        {
            char * pbTargetMax = pbTarget + cbTarget;
            register uint32_t ulHashPattern;
            uint32_t ulHashTarget, count, countSTATIC;
            if (cbPattern > cbTarget) return(NULL);
            countSTATIC = cbPattern-2;
            pbTarget = pbTarget+cbPattern;
            ulHashPattern = (*(uint16_t *)(pbPattern));
            for ( ;; ) {
                if ( ulHashPattern == (*(uint16_t *)(pbTarget-cbPattern)) ) {
                    count = countSTATIC;
                    while ( count && *(char *)(pbPattern+2+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+2+(countSTATIC-count)) ) {
                        count--;
                    }
                    if ( count == 0 ) return((pbTarget-cbPattern));
                }
                pbTarget++;
                if (pbTarget > pbTargetMax) return(NULL);
            }
        }
    } //if (h_len <= 4096/8) { // the warm-up is quick thus suitable for parsing lines not files; the warm-up for BMH Pseudo-Order 4 is nasty
*/
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
    if (n_len <= 32) {
        size_t idx1 = n_len - 1; 
        size_t idx2 = n_len - 2;
        while (idx2 > 1 && n[idx2] == n[idx1]) idx2--; // cannot be the first char (i.e. ==0) - it is reserved for idx3 eventually
        size_t idx3 = (idx2 > 0) ? idx2 - 1 : 0;
        while (idx3 > 0 && (n[idx3] == n[idx1] || n[idx3] == n[idx2])) idx3--;

        __m256i ymm_v1 = _mm256_set1_epi8((char)n[idx1]);
        __m256i ymm_v2 = _mm256_set1_epi8((char)n[idx2]);
        __m256i ymm_v3 = _mm256_set1_epi8((char)n[idx3]);

        size_t shift = 0;
        const size_t limit = h_len - n_len;

        char needle_pad[32] = {0};
        memcpy(needle_pad, n, n_len);
        __m256i ymm_needle = _mm256_loadu_si256((const __m256i*)needle_pad);
        uint32_t verify_bitmask = (n_len == 32) ? 0xFFFFFFFF : (1U << n_len) - 1;

        while (shift <= limit) {
            // Perfect Hardware Symmetry: 3 loads mapping to 3 CPU load ports
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + idx1));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + idx2));
            __m256i h3 = _mm256_loadu_si256((const __m256i*)(h + shift + idx3));
            
            // Triple-Intersection Sieve
            __m256i m1 = _mm256_cmpeq_epi8(h1, ymm_v1);
            __m256i m2 = _mm256_cmpeq_epi8(h2, ymm_v2);
            __m256i m3 = _mm256_cmpeq_epi8(h3, ymm_v3);
            
            __m256i mask = _mm256_and_si256(_mm256_and_si256(m1, m2), m3);
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(mask);

            while (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                size_t cand_shift = shift + tz;

                if (cand_shift > limit) return NULL; 

                // SPECIAL OPTIMIZATION: If the needle is exactly 3 bytes (like "and"),
                // passing a 3-vector sieve is a guaranteed 100% match. Skip validation entirely.
                // TO-DO: this should be removed... and get its own usecase...
                //if (n_len == 3) {
                //    return (void *)(h + cand_shift);
                //}

                // Inline O(1) full vector validation for needles 4 to 32 bytes
                // 100% Branchless Verification
                // If the first n_len bits match, we have an exact hit. No loops.
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

		if ( cbPattern<=NeedleThreshold2vs4swampAVX2 ) { 

			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;

								if (cbPattern != PRIMALlengthCANDIDATE) { // No need of same comparison when Needle and NewNeedle are equal!
							if ( count <= 0 ) {
								if ( ((signed long long)(i-(PRIMALposition-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)]+((PRIMALlengthCANDIDATE-4+1)-1) <= pbTargetMax - 4) ) { // 2020-jan-11
									if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
										count = PRIMALlengthCANDIDATE-4+1; 
										while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
											count = count-4;
										if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
									}
								}
							}
								} else { //if (cbPattern != PRIMALlengthCANDIDATE)
														if ( count <= 0 ) return(pbTarget+i);
								}
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
			}
			return(NULL);

		} else { // if ( cbPattern<=NeedleThreshold2vs4swampAVX2 )

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
		} // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

    // Scalar (n_len > 32) with BMH Pseudo-Order 4 strengthened by 'Swampwalker' Heuristic with Primalization/Factorization order 4: ]]]

}
// Railgun 'Trinity', written 2026-Jun-29 by Gemini PRO 3.1 and Kaze(sanmayce@sanmayce.com) ]

/*
I made a simple C program/test that accepts a file and searches it with several functions (GLIBC 2.41, musl 1.2.5, StringZilla-4.6.2).
The package is at:
https://github.com/user-attachments/files/29234007/StringZilla_vs_GLIBC_vs_musl_vs_Railgun_round9_NO-CORPORA.tar.gz
My function is at:
https://github.com/Sanmayce/Railgun/blob/main/Railgun_Trinity.h

Tested datafile: linux-6.6.1.tar, 1,419,100,160 bytes;
Test machine: Laptop Dell Latitude 7420 8x4 GB (Row Of Chips) LPDDR4 4267 MT/s, Intel i7-1185G7 "Tiger Lake-UP3", Max Turbo: 4800 MHz, L3 cache: 12 MiB, Linux Fedora 42
The values are in MiB/s – the highest of 7 attempts is taken.
The mode is ‘Performance’ as superuser and with highest niceness.

CLANG_20.1.8, (-O3 -mavx2):
+-----+--------------------+-----------------+-------------+---------+-------------+
| Len | Target String      | Trinity (MiB/s) | StringZilla | GLIBC   | Winner      |
+-----+--------------------+-----------------+-------------+---------+-------------+
| 2   | );                 | 11804.6         | 9751.8      | 1850.0  | Trinity     |
| 2   |   (spaces)         | 1388.9          | 1454.3      | 1097.9  | StringZilla |
| 3   | zoo                | 14398.3         | 14324.0     | 3577.6  | Trinity     |
| 3   | int                | 12059.2         | 12562.3     | 2783.3  | StringZilla |
| 4   | fast               | 14296.7         | 14205.5     | 4364.2  | Trinity     |
| 4   | that               | 14244.2         | 14080.5     | 4226.3  | Trinity     |
| 5   | Linux              | 14284.1         | 14367.9     | 4607.7  | StringZilla |
| 5   | btree              | 14283.5         | 14301.8     | 5009.1  | StringZilla |
| 6   | struct             | 11564.2         | 11744.3     | 4889.6  | StringZilla |
| 8   | include            | 13732.4         | 14030.6     | 6170.4  | StringZilla |
| 9   | interrupt          | 14283.0         | 14133.0     | 7434.5  | Trinity     |
| 9   | <stdio.h>          | 14406.9         | 14335.9     | 8063.2  | Trinity     |
| 12  | independent.       | 14434.7         | 14302.0     | 9187.9  | Trinity     |
| 14  | Linus Torvalds     | 14343.8         | 14329.9     | 9198.0  | Trinity     |
| 14  | Linux Torvalds     | 14402.5         | 14296.2     | 9841.0  | Trinity     |
| 14  | consumer->node     | 13701.6         | 14255.7     | 9741.5  | StringZilla |
| 30  | irq_bypass...      | 14271.7         | 14035.1     | 12074.6 | Trinity     |
| 37  | MD5 Message...     | 16328.2         | 12467.5     | 11924.1 | Trinity     |
| 40  | you can redi...    | 16420.4         | 12403.9     | 12322.8 | Trinity     |
| 50  | EXPORT_SYMBOL...   | 17999.3         | 12521.8     | 13074.1 | Trinity     |
| 73  | void irq_by...     | 17016.5         | 12718.9     | 10503.9 | Trinity     |
+-----+--------------------+-----------------+-------------+---------+-------------+
 
GCC 15.2.1, (-O3 -mavx2):
+-----+--------------------+-----------------+-------------+---------+-------------+
| Len | Target String      | Trinity (MiB/s) | StringZilla | GLIBC   | Winner      |
+-----+--------------------+-----------------+-------------+---------+-------------+
| 2   | );                 | 11718.4         | 10177.5     | 1842.8  | Trinity     |
| 2   |   (spaces)         | 1499.1          | 1338.5      | 1120.0  | Trinity     |
| 3   | zoo                | 14506.0         | 14148.7     | 3585.2  | Trinity     |
| 3   | int                | 12335.7         | 11917.6     | 2809.1  | Trinity     |
| 4   | fast               | 14303.9         | 14041.0     | 4417.5  | Trinity     |
| 4   | that               | 14316.8         | 13876.0     | 4156.0  | Trinity     |
| 5   | Linux              | 14306.2         | 14139.5     | 4636.2  | Trinity     |
| 5   | btree              | 14252.6         | 14137.1     | 5065.7  | Trinity     |
| 6   | struct             | 11415.9         | 11205.8     | 4872.8  | Trinity     |
| 8   | include            | 13833.3         | 13790.5     | 5942.6  | Trinity     |
| 9   | interrupt          | 14280.7         | 13935.1     | 7412.1  | Trinity     |
| 9   | <stdio.h>          | 14465.5         | 14248.9     | 7967.8  | Trinity     |
| 12  | independent.       | 14441.4         | 14164.7     | 9219.0  | Trinity     |
| 14  | Linus Torvalds     | 14432.2         | 14093.1     | 9719.6  | Trinity     |
| 14  | Linux Torvalds     | 14406.7         | 14158.2     | 9707.5  | Trinity     |
| 14  | consumer->node     | 13740.1         | 14085.9     | 9422.1  | StringZilla |
| 30  | irq_bypass...      | 14337.4         | 14056.3     | 12063.6 | Trinity     |
| 37  | MD5 Message...     | 15740.6         | 14119.0     | 11880.3 | Trinity     |
| 40  | you can redi...    | 16300.8         | 14072.3     | 12672.8 | Trinity     |
| 50  | EXPORT_SYMBOL...   | 17691.2         | 14120.1     | 12968.9 | Trinity     |
| 73  | void irq_by...     | 16813.8         | 14084.1     | 10580.8 | Trinity     |
+-----+--------------------+-----------------+-------------+---------+-------------+
 
Tested datafile: GCF_000001405.40_GRCh38.p14_genomic.fna, 3,339,739,109 bytes;
Test machine: Laptop Dell Latitude 7420 8x4 GB (Row Of Chips) LPDDR4 4267 MT/s, Intel i7-1185G7 "Tiger Lake-UP3", Max Turbo: 4800 MHz, L3 cache: 12 MiB, Linux Fedora 42
The values are in MiB/s – the highest of 7 attempts is taken.
The mode is ‘Performance’ as superuser and with highest niceness.

CLANG_20.1.8, (-O3 -mavx2):
+-----+--------------------+-----------------+-------------+---------+-------------+
| Len | Target String      | Trinity (MiB/s) | StringZilla | GLIBC   | Winner      |
+-----+--------------------+-----------------+-------------+---------+-------------+
| 2   | TT                 | 2437.1          | 2465.8      | 981.5   | StringZilla |
| 4   | CCGA               | 12835.8         | 8112.0      | 1705.6  | Trinity     |
| 4   | GACC               | 7744.4          | 7971.9      | 1536.8  | StringZilla |
| 8   | CCCCGGGA           | 8668.6          | 8352.7      | 2762.4  | Trinity     |
| 9   | GCGGACCCC          | 8627.2          | 8799.5      | 2618.1  | StringZilla |
| 41  | GTTTCTCAGGTT...    | 11555.9         | 8375.6      | 4383.6  | Trinity     |
| 80  | TTTTAGTGTAGG...    | 11901.7         | 7767.4      | 5109.3  | Trinity     |
+-----+--------------------+-----------------+-------------+---------+-------------+
 
GCC 15.2.1, (-O3 -mavx2):
+-----+--------------------+-----------------+-------------+---------+-------------+
| Len | Target String      | Trinity (MiB/s) | StringZilla | GLIBC   | Winner      |
+-----+--------------------+-----------------+-------------+---------+-------------+
| 2   | TT                 | 2618.4          | 2393.0      | 989.1   | Trinity     |
| 4   | CCGA               | 12837.1         | 7562.0      | 1686.1  | Trinity     |
| 4   | GACC               | 7568.5          | 7540.4      | 1513.4  | Trinity     |
| 8   | CCCCGGGA           | 8549.2          | 7955.2      | 2817.5  | Trinity     |
| 9   | GCGGACCCC          | 8425.8          | 8342.5      | 2572.4  | Trinity     |
| 41  | GTTTCTCAGGTT...    | 11446.3         | 8558.8      | 4409.9  | Trinity     |
| 80  | TTTTAGTGTAGG...    | 11958.8         | 8370.2      | 5127.8  | Trinity     |
+-----+--------------------+-----------------+-------------+---------+-------------+
 
Hope, the search for Fastest memmem() is not over, I am looking forward seeing betterments, who can speed it up even more?
*/

