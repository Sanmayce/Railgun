// fuzzorama.c
// gcc -O3 -mavx512f -mavx512bw -fopenmp fuzzorama.c -o fuzzorama

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>
#include <omp.h>

#define unlikely(x) __builtin_expect(!!(x), 0)

const char * Railgun_Fuzzy_WuManber64(const char * pbTarget, size_t cbTarget, 
                                      const char * pbPattern, size_t cbPattern, 
                                      uint8_t k_mismatches) {
    
    // Natively supports patterns up to the CPU word size (64 bits).
    if (cbPattern == 0 || cbPattern > 64) return NULL;
    if (k_mismatches >= cbPattern) return pbTarget;

    // [1] ALPHABET PRECOMPUTATION (Shift-Or Logic)
    // We initialize all characters to all 1s (mismatch).
    // We set a 0 bit at the position where the character matches.
    uint64_t peq[256];
    for (int i = 0; i < 256; i++) {
        peq[i] = ~0ULL;
    }
    for (size_t i = 0; i < cbPattern; i++) {
        peq[(unsigned char)pbPattern[i]] &= ~(1ULL << i);
    }

    // [2] INITIALIZE K+1 BITVECTORS
    // R[j] holds the state for 'j' errors.
    // We shift by 'j' to allow leading deletions. For example, R[1] gets bit 0 
    // initialized to '0' because the first character can be theoretically deleted.
    uint64_t R[64]; 
    for (uint8_t j = 0; j <= k_mismatches; j++) {
        R[j] = ~0ULL << j;
    }

    uint64_t match_mask = 1ULL << (cbPattern - 1);
    const char * curr = pbTarget;
    const char * end = pbTarget + cbTarget;

    // [3] THE SCALAR SCAN
    while (curr < end) {
        uint64_t char_mask = peq[(unsigned char)*curr];
        
        uint64_t old_R = R[0];
        
        // Base case: 0 errors (Standard Shift-Or Exact Match)
        // Shifting left brings in a 0. If the char matches, char_mask has a 0. 
        // 0 | 0 = 0 (Match survives!)
        R[0] = (R[0] << 1) | char_mask;
        
        // Compute states for 1 up to K errors
        for (uint8_t j = 1; j <= k_mismatches; j++) {
            uint64_t temp = R[j];
            
            // In Shift-Or, a 0 is a success. Bitwise AND (&) propagates the 0s.
            // If ANY of these 4 conditions are 0, the state survives.
            
            // 1. Match: previous state with j errors + character matches
            uint64_t match = (temp << 1) | char_mask;
            
            // 2. Substitution: previous state with j-1 errors + 1 error penalty
            uint64_t sub = old_R << 1;
            
            // 3. Insertion (in target): previous state with j-1 errors + 1 error penalty
            uint64_t ins = old_R;
            
            // 4. Deletion (from target): CURRENT state with j-1 errors + 1 error penalty
            uint64_t del = R[j-1] << 1;
            
            // Apply all possible paths
            R[j] = match & sub & ins & del;
            
            old_R = temp;
        }

        // Check if the most significant bit of the highest error vector is 0
        if ((R[k_mismatches] & match_mask) == 0) {
            return curr; // Exact end-pointer
        }
        
        curr++;
    }
    
    return NULL;
}

const char * Railgun_Fuzzy_AVX2(const char * pbTarget, size_t cbTarget, 
                                const char * pbPattern, size_t cbPattern, 
                                uint8_t k_mismatches) {
                                    
    if (unlikely(cbTarget < cbPattern)) return NULL;
    if (unlikely(cbPattern == 0)) return pbTarget;

    const char * curr = pbTarget;
    
    // 8-bit counters overflow at 255. Cap pattern size for AVX2 safety.
    if (cbPattern > 255 || cbTarget < cbPattern + 32) {
	} else {

    const char * max_curr_vec = pbTarget + cbTarget - cbPattern - 32;
    
    __m256i v_ones = _mm256_set1_epi8(1);
    __m256i v_K    = _mm256_set1_epi8((char)k_mismatches);

    while (curr <= max_curr_vec) {
        // errs holds 32 independent error counters for 32 sliding offsets
        __m256i errs = _mm256_setzero_si256();
        
        for (size_t i = 0; i < cbPattern; i++) {
            // Load 32 consecutive text bytes starting at offset + i
            __m256i t = _mm256_loadu_si256((const __m256i*)(curr + i));
            
            // Broadcast pattern character [i] into all 32 lanes
            __m256i p = _mm256_set1_epi8(pbPattern[i]);
            
            // Compare: 0xFF if match, 0x00 if mismatch
            __m256i cmp = _mm256_cmpeq_epi8(t, p);
            
            // Convert mismatch to 1 (0x00 -> 0x01)
            __m256i mismatch = _mm256_andnot_si256(cmp, v_ones);
            
            // Vertically accumulate mismatches for all 32 text offsets
            errs = _mm256_add_epi8(errs, mismatch);
            
            // SIMD EARLY EXIT: Every 4 chars, kill the loop if all 32 lanes failed
            if ((i & 3) == 3) {
                // _mm256_max_epu8(errs, K) == K implies errs <= K
                __m256i max_val = _mm256_max_epu8(errs, v_K);
                __m256i match_cmp = _mm256_cmpeq_epi8(max_val, v_K);
                
                // If no lane is <= K, movemask is 0. Abort this chunk!
                if (_mm256_movemask_epi8(match_cmp) == 0) {
                    goto next_chunk; 
                }
            }
        }
        
        // Final evaluation: Check if any of the 32 offsets survived
        {
            __m256i max_val = _mm256_max_epu8(errs, v_K);
            __m256i match_cmp = _mm256_cmpeq_epi8(max_val, v_K);
            uint32_t mask = (uint32_t)_mm256_movemask_epi8(match_cmp);
            
            if (unlikely(mask != 0)) {
                // __builtin_ctz finds the exact text offset that won
                return curr + __builtin_ctz(mask);
            }
        }
        
    next_chunk:
        curr += 32; // MAGICAL SPEEDUP: Slide window by 32 bytes at once!
    }
    
    } //if (cbPattern > 255 || cbTarget < cbPattern + 32) {
// fallback
    {
        const char * max_curr = pbTarget + cbTarget - cbPattern;
        while (curr <= max_curr) {
            uint8_t errs = 0;
            for (size_t i = 0; i < cbPattern; i++) {
                if (curr[i] != pbPattern[i]) {
                    errs++;
                    if (errs > k_mismatches) break;
                }
            }
            if (errs <= k_mismatches) return curr;
            curr++;
        }
    }
    
    return NULL;
}

const char * Schatzhauser_AVX2(const char * pbTarget, size_t cbTarget, 
                         const char * pbPattern, size_t cbPattern, 
                         uint8_t k_mismatches) {
    
    // Trivial case: If errors exceed pattern length, everything matches.
    if (unlikely(k_mismatches >= cbPattern)) return pbTarget;

    const char * curr = pbTarget;
    const char * end = pbTarget + cbTarget;
    
    // YOUR INTUITION IN CODE: The K + 1 Pigeonhole Formula.
    // This mathematically guarantees that the Hamming scout will NEVER 
    // miss a Levenshtein match, no matter how badly it is shifted.
    uint8_t surviving_chars = cbPattern / (k_mismatches + 1);
    if (surviving_chars == 0) surviving_chars = 1; // absolute minimum bound
    uint8_t scout_k = cbPattern - surviving_chars;
    
    while (curr < end) {
        // [1] SCOUT: High-speed AVX-512 block jump
        const char * candidate = Railgun_Fuzzy_AVX2(curr, end - curr, pbPattern, cbPattern, scout_k);
        
        //if (!candidate) return NULL;
        if (!candidate) break;
        
        // [2] ALIGN: Because the "surviving piece" might be at the very end of the word,
        // the true start of the Levenshtein match could be up to K bytes earlier.
        const char * verify_start = candidate - k_mismatches;
        if (verify_start < curr) verify_start = curr; 
        
        // [3] WINDOW: Ensure we span the maximum possible length expansion (L + 2K)
        size_t window_size = cbPattern + (k_mismatches * 2);
        if (verify_start + window_size > end) window_size = end - verify_start;
        
        // [4] VERIFY: Run the scalar DP matrix on the unclipped window
        //const char * res = Railgun_Fuzzy_Levenshtein(verify_start, window_size, pbPattern, cbPattern, k_mismatches);
        const char * res = Railgun_Fuzzy_WuManber64(verify_start, window_size, pbPattern, cbPattern, k_mismatches);
        
        if (res) {
            return res; // Exact match end-pointer
        }
        
        // [5] ADVANCE: Move forward by 1 byte from the rejected candidate
        curr = candidate + 1;
    }

    // =========================================================================
    // THE SHORT-TAIL VERIFIER (Catching end-of-buffer deletions)
    // =========================================================================
    
    // We only need to check the tail if deletions are possible (K > 0)
    if (k_mismatches > 0) {
        // The shortest possible match length is (cbPattern - k_mismatches)
        size_t shortest_possible_match = cbPattern - k_mismatches;
        
        // If the entire text is shorter than the minimum possible match, impossible.
        if (cbTarget < shortest_possible_match) return NULL;

        // Where did the main loops stop checking?
        // They stopped when the remaining text was exactly cbPattern - 1 bytes long.
        const char * tail_start = pbTarget + cbTarget - (cbPattern - 1);
        if (tail_start < pbTarget) tail_start = pbTarget; // Safety clamp

        // Iterate through the unchecked tail positions
        while (tail_start <= pbTarget + cbTarget - shortest_possible_match) {
            
            // The available text from this position to the end of the buffer
            size_t available_text = end - tail_start;

            // Run YOUR specific Verifier directly on this short chunk!
            const char * res = Railgun_Fuzzy_WuManber64(tail_start, available_text, 
                                                        pbPattern, cbPattern, 
                                                        k_mismatches);
            
            if (res) return res; // Found the hidden short-tail match!

            tail_start++;
        }
    }

    return NULL; // Absolutely nothing found.
}

// 'Railgun_Schatzhauser_AVX2' - The Fastest Fuzzy Finder for Levenshtein Distance <=2 
// Testmachine: i7-11850H (16 threads) 4x32GB DDR4, GCC v15.2.1
// Tested in Performance Mode, with highest niceness
/*
MULTI-THREADED SHOWDOWN (16 Cores Engaged):

+-------------+--------------+--------------------+---------------------------+---------------------+
| Needle Size | Distance (K) | WuManber64 (MiB/s) | Schatzhauser_AVX2 (MiB/s) | Speedup vs WuManber |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   7 bytes   |      2       |       2582.61      |          3296.59          |        1.28x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      1       |       3524.21      |         13027.93          |        3.70x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      2       |       2490.84      |          9428.57          |        3.79x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      3       |       1935.29      |          3254.69          |        1.68x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      4       |       1643.46      |           605.01          |        0.37x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      1       |       3752.75      |         14968.65          |        3.99x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      2       |       2604.42      |          6310.41          |        2.42x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      3       |       2036.61      |          1206.50          |        0.59x        |
+-------------+--------------+--------------------+---------------------------+---------------------+

SINGLE-THREADED SHOWDOWN (1 Core Engaged):

+-------------+--------------+--------------------+---------------------------+---------------------+
| Needle Size | Distance (K) | WuManber64 (MiB/s) | Schatzhauser_AVX2 (MiB/s) | Speedup vs WuManber |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   7 bytes   |      2       |        429.00      |           731.31          |        1.70x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      1       |        673.42      |          2767.35          |        4.11x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      2       |        430.24      |          2214.01          |        5.15x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      3       |        386.31      |           676.07          |        1.75x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|  12 bytes   |      4       |        290.01      |           112.99          |        0.39x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      1       |        687.96      |          3421.57          |        4.97x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      2       |        424.89      |          1392.85          |        3.28x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
|   9 bytes   |      3       |        357.32      |           234.93          |        0.66x        |
+-------------+--------------+--------------------+---------------------------+---------------------+
*/
// For K=2:
// Note1: Needle (7 bytes) "Quixote", yields 93,752 hits;
// Note2: Needle (12 bytes) "Schatzhauser", yields 46 hits;
// Note3: Needle (9 bytes) "Gutenberg", yields 3,322,534 hits;


// https://godbolt.org/z/oKxGGv7zW
// CLANG 22.1.0 -O3 -mavx2
/*
.LCPI0_0:
        .quad   0
        .quad   1
        .quad   2
        .quad   3
.LCPI0_1:
        .quad   4
.LCPI0_2:
        .quad   8
.LCPI0_3:
        .quad   12
.LCPI0_4:
        .quad   16
Railgun_Fuzzy_WuManber64(char const*, unsigned long, char const*, unsigned long, unsigned char):
        push    rbp
        push    r15
        push    r14
        push    r13
        push    r12
        push    rbx
        sub     rsp, 2568
        mov     rbx, rcx
        add     rcx, -65
        cmp     rcx, -64
        jae     .LBB0_2
        xor     eax, eax
        jmp     .LBB0_35
.LBB0_2:
        mov     rax, rdi
        movzx   r14d, r8b
        cmp     rbx, r14
        jbe     .LBB0_35
        mov     rbp, rdx
        mov     r15, r8
        mov     r12, rax
        mov     r13, rsi
        lea     rdi, [rsp + 512]
        mov     edx, 2048
        mov     esi, 255
        call    memset@PLT
        cmp     rbx, 1
        jne     .LBB0_24
        xor     ecx, ecx
        mov     rax, r13
        mov     rsi, r12
        mov     rdi, r15
        mov     rdx, rbp
        jmp     .LBB0_6
.LBB0_24:
        mov     eax, ebx
        and     eax, 126
        xor     ecx, ecx
        mov     rdx, rbp
.LBB0_25:
        mov     rsi, -2
        rol     rsi, cl
        movzx   edi, byte ptr [rdx + rcx]
        and     qword ptr [rsp + 8*rdi + 512], rsi
        mov     esi, 2
        shl     rsi, cl
        not     rsi
        movzx   edi, byte ptr [rdx + rcx + 1]
        and     qword ptr [rsp + 8*rdi + 512], rsi
        add     rcx, 2
        cmp     rax, rcx
        jne     .LBB0_25
        test    bl, 1
        mov     rax, r13
        mov     rsi, r12
        mov     rdi, r15
        je      .LBB0_7
.LBB0_6:
        mov     r8, -2
        rol     r8, cl
        movzx   ecx, byte ptr [rdx + rcx]
        and     qword ptr [rsp + 8*rcx + 512], r8
.LBB0_7:
        lea     edx, [rdi + 1]
        movzx   r9d, dl
        cmp     r9b, 2
        mov     ecx, 1
        cmovae  ecx, r9d
        movzx   r8d, cl
        cmp     r9b, 4
        jae     .LBB0_10
        xor     ecx, ecx
        jmp     .LBB0_9
.LBB0_10:
        cmp     dl, 16
        jae     .LBB0_12
        xor     ecx, ecx
        jmp     .LBB0_16
.LBB0_12:
        mov     ecx, r8d
        and     ecx, -16
        lea     r10d, [8*r8]
        and     r10d, -128
        vmovdqa ymm0, ymmword ptr [rip + .LCPI0_0]
        vpbroadcastq    ymm1, qword ptr [rip + .LCPI0_1]
        vpbroadcastq    ymm2, qword ptr [rip + .LCPI0_2]
        vpbroadcastq    ymm3, qword ptr [rip + .LCPI0_3]
        xor     r11d, r11d
        vpcmpeqd        ymm4, ymm4, ymm4
        vpbroadcastq    ymm5, qword ptr [rip + .LCPI0_4]
.LBB0_13:
        vpaddq  ymm6, ymm0, ymm1
        vpaddq  ymm7, ymm0, ymm2
        vpaddq  ymm8, ymm0, ymm3
        vpsllvq ymm9, ymm4, ymm0
        vpsllvq ymm6, ymm4, ymm6
        vpsllvq ymm7, ymm4, ymm7
        vpsllvq ymm8, ymm4, ymm8
        vmovdqu ymmword ptr [rsp + r11], ymm9
        vmovdqu ymmword ptr [rsp + r11 + 32], ymm6
        vmovdqu ymmword ptr [rsp + r11 + 64], ymm7
        vmovdqu ymmword ptr [rsp + r11 + 96], ymm8
        vpaddq  ymm0, ymm0, ymm5
        sub     r11, -128
        cmp     r10, r11
        jne     .LBB0_13
        cmp     ecx, r8d
        je      .LBB0_19
        test    r8b, 12
        je      .LBB0_9
.LBB0_16:
        mov     r10, rcx
        mov     ecx, r8d
        vmovq   xmm0, r10
        vpbroadcastq    ymm0, xmm0
        vpor    ymm0, ymm0, ymmword ptr [rip + .LCPI0_0]
        and     ecx, -4
        vpcmpeqd        ymm1, ymm1, ymm1
        vpbroadcastq    ymm2, qword ptr [rip + .LCPI0_1]
.LBB0_17:
        vpsllvq ymm3, ymm1, ymm0
        vmovdqu ymmword ptr [rsp + 8*r10], ymm3
        add     r10, 4
        vpaddq  ymm0, ymm0, ymm2
        cmp     rcx, r10
        jne     .LBB0_17
        cmp     ecx, r8d
        je      .LBB0_19
.LBB0_9:
        mov     r10, -1
        shl     r10, cl
        mov     qword ptr [rsp + 8*rcx], r10
        inc     rcx
        cmp     r8, rcx
        jne     .LBB0_9
.LBB0_19:
        dec     bl
        mov     r8d, 1
        mov     ecx, ebx
        shl     r8, cl
        add     rax, rsi
        test    dil, dil
        je      .LBB0_20
        cmp     dl, 3
        mov     ecx, 2
        cmovae  ecx, r9d
        movzx   edi, cl
        dec     rdi
        mov     r9, rdi
        and     r9, -2
        neg     r9
        jmp     .LBB0_27
.LBB0_32:
        mov     r15, qword ptr [rsp + 8*r10]
        add     r15, r15
        or      r15, r11
        and     rsi, rbx
        add     rsi, rsi
        and     r15, rbx
        and     r15, rsi
        mov     qword ptr [rsp + 8*r10], r15
.LBB0_33:
        lea     rsi, [rcx + 1]
        test    qword ptr [rsp + 8*r14], r8
        je      .LBB0_34
.LBB0_27:
        cmp     rsi, rax
        jae     .LBB0_28
        mov     rcx, rsi
        movzx   esi, byte ptr [rsi]
        mov     r11, qword ptr [rsp + 8*rsi + 512]
        mov     rbx, qword ptr [rsp]
        lea     rsi, [rbx + rbx]
        or      rsi, r11
        mov     qword ptr [rsp], rsi
        mov     r10d, 1
        cmp     dl, 3
        jb      .LBB0_32
.LBB0_30:
        mov     r15, qword ptr [rsp + 8*r10]
        and     rsi, rbx
        add     rsi, rsi
        and     rsi, rbx
        mov     rbx, qword ptr [rsp + 8*r10 + 8]
        lea     r12, [r15 + r15]
        or      r12, r11
        and     rsi, r12
        mov     qword ptr [rsp + 8*r10], rsi
        lea     r12, [rbx + rbx]
        or      r12, r11
        and     rsi, r15
        mov     r13, rsi
        add     r13, rsi
        and     r12, r15
        mov     rsi, r12
        and     rsi, r13
        mov     qword ptr [rsp + 8*r10 + 8], rsi
        lea     r15, [r9 + r10]
        add     r15, 2
        add     r10, 2
        cmp     r15, 1
        jne     .LBB0_30
        test    dil, 1
        jne     .LBB0_32
        jmp     .LBB0_33
.LBB0_20:
        xor     ecx, ecx
.LBB0_21:
        cmp     rsi, rax
        jae     .LBB0_34
        movzx   edx, byte ptr [rsi]
        mov     rdi, qword ptr [rsp]
        add     rdi, rdi
        or      rdi, qword ptr [rsp + 8*rdx + 512]
        mov     qword ptr [rsp], rdi
        inc     rsi
        test    qword ptr [rsp + 8*r14], r8
        jne     .LBB0_21
        dec     rsi
        mov     rcx, rsi
        jmp     .LBB0_34
.LBB0_28:
        xor     ecx, ecx
.LBB0_34:
        mov     rax, rcx
.LBB0_35:
        add     rsp, 2568
        pop     rbx
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
        vzeroupper
        ret

Railgun_Fuzzy_AVX2(char const*, unsigned long, char const*, unsigned long, unsigned char):
        mov     r9, rsi
        sub     r9, rcx
        jb      .LBB1_1
        mov     rax, rdi
        test    rcx, rcx
        je      .LBB1_2
        push    rbx
        cmp     rcx, 256
        setae   dil
        lea     r10, [rcx + 32]
        cmp     rsi, r10
        setb    r10b
        or      r10b, dil
        mov     rdi, rax
        jne     .LBB1_12
        mov     rdi, rax
        cmp     r9, 32
        jge     .LBB1_6
.LBB1_12:
        add     rax, rsi
        sub     rax, rcx
        jmp     .LBB1_13
.LBB1_18:
        cmp     r9b, r8b
        jbe     .LBB1_19
.LBB1_22:
        inc     rdi
.LBB1_13:
        cmp     rdi, rax
        ja      .LBB1_23
        xor     esi, esi
        xor     r9d, r9d
        jmp     .LBB1_15
.LBB1_17:
        inc     rsi
        cmp     rcx, rsi
        je      .LBB1_18
.LBB1_15:
        movzx   r10d, byte ptr [rdi + rsi]
        cmp     r10b, byte ptr [rdx + rsi]
        je      .LBB1_17
        inc     r9b
        cmp     r9b, r8b
        jbe     .LBB1_17
        jmp     .LBB1_22
.LBB1_23:
        xor     eax, eax
        pop     rbx
        vzeroupper
        ret
.LBB1_6:
        add     r9, rax
        add     r9, -32
        vmovd   xmm0, r8d
        vpbroadcastb    ymm0, xmm0
        vpcmpeqd        ymm1, ymm1, ymm1
        mov     rdi, rax
        jmp     .LBB1_7
.LBB1_20:
        test    r11d, r11d
        jne     .LBB1_21
.LBB1_11:
        add     rdi, 32
        cmp     rdi, r9
        ja      .LBB1_12
.LBB1_7:
        vpxor   xmm2, xmm2, xmm2
        xor     r10d, r10d
        jmp     .LBB1_9
.LBB1_8:
        inc     r10
        cmp     rcx, r10
        je      .LBB1_20
.LBB1_9:
        vpbroadcastb    ymm3, byte ptr [rdx + r10]
        vpcmpeqb        ymm3, ymm3, ymmword ptr [rdi + r10]
        vpaddb  ymm2, ymm2, ymm3
        vpsubb  ymm2, ymm2, ymm1
        mov     ebx, r10d
        not     ebx
        vpmaxub ymm3, ymm0, ymm2
        vpcmpeqb        ymm3, ymm0, ymm3
        vpmovmskb       r11d, ymm3
        test    bl, 3
        jne     .LBB1_8
        test    r11d, r11d
        jne     .LBB1_8
        jmp     .LBB1_11
.LBB1_19:
        mov     rax, rdi
        pop     rbx
        vzeroupper
        ret
.LBB1_21:
        rep       bsf eax, r11d
        add     rdi, rax
        mov     rax, rdi
        pop     rbx
        vzeroupper
        ret
.LBB1_1:
        xor     eax, eax
.LBB1_2:
        ret

Schatzhauser_AVX2(char const*, unsigned long, char const*, unsigned long, unsigned char):
        push    rbp
        push    r15
        push    r14
        push    r13
        push    r12
        push    rbx
        sub     rsp, 104
        mov     qword ptr [rsp + 16], rdi
        mov     edi, r8d
        mov     r10, rcx
        sub     r10, rdi
        jbe     .LBB2_49
        mov     r13d, r8d
        mov     rbx, rcx
        mov     r14, rdx
        mov     r15, rsi
        movzx   esi, r13b
        lea     ecx, [rsi + 1]
        mov     rax, rbx
        shr     rax, 32
        je      .LBB2_3
        mov     rax, rbx
        xor     edx, edx
        div     rcx
        jmp     .LBB2_4
.LBB2_3:
        mov     eax, ebx
        xor     edx, edx
        div     ecx
.LBB2_4:
        mov     rcx, qword ptr [rsp + 16]
        lea     r11, [rcx + r15]
        test    al, al
        mov     ecx, 1
        cmovne  ecx, eax
        mov     ebp, ebx
        sub     ebp, ecx
        neg     rdi
        mov     qword ptr [rsp + 56], rdi
        mov     qword ptr [rsp + 32], rsi
        mov     eax, esi
        lea     rax, [rbx + 2*rax]
        mov     qword ptr [rsp + 48], rax
        cmp     rbx, 255
        jbe     .LBB2_17
        mov     r8, qword ptr [rsp + 16]
.LBB2_6:
        cmp     r8, r11
        jae     .LBB2_40
        mov     rcx, r11
        sub     rcx, r8
        sub     rcx, rbx
        jb      .LBB2_40
        js      .LBB2_40
        add     rcx, r8
        mov     r12, r8
        jmp     .LBB2_12
.LBB2_10:
        cmp     sil, bpl
        jbe     .LBB2_16
.LBB2_11:
        inc     r12
        cmp     r12, rcx
        ja      .LBB2_40
.LBB2_12:
        xor     edx, edx
        xor     esi, esi
        jmp     .LBB2_14
.LBB2_13:
        inc     rdx
        cmp     rbx, rdx
        je      .LBB2_10
.LBB2_14:
        movzx   edi, byte ptr [r12 + rdx]
        cmp     dil, byte ptr [r14 + rdx]
        je      .LBB2_13
        inc     sil
        cmp     sil, bpl
        jbe     .LBB2_13
        jmp     .LBB2_11
.LBB2_16:
        mov     rcx, qword ptr [rsp + 56]
        lea     rdi, [r12 + rcx]
        cmp     rdi, r8
        cmovb   rdi, r8
        mov     rcx, qword ptr [rsp + 48]
        lea     rax, [rdi + rcx]
        mov     rsi, r11
        sub     rsi, rdi
        cmp     rax, r11
        cmovbe  rsi, rcx
        mov     rdx, r14
        mov     rcx, rbx
        mov     r8, qword ptr [rsp + 32]
        mov     dword ptr [rsp + 12], r13d
        mov     r13, r10
        mov     qword ptr [rsp + 24], r11
        call    Railgun_Fuzzy_WuManber64(char const*, unsigned long, char const*, unsigned long, unsigned char)
        mov     r11, qword ptr [rsp + 24]
        mov     r10, r13
        mov     r13d, dword ptr [rsp + 12]
        inc     r12
        mov     r8, r12
        test    rax, rax
        je      .LBB2_6
        jmp     .LBB2_48
.LBB2_17:
        mov     rax, rbx
        neg     rax
        mov     qword ptr [rsp + 24], rax
        vmovd   xmm0, ebp
        vpbroadcastb    ymm2, xmm0
        vpcmpeqd        ymm3, ymm3, ymm3
        mov     r9, qword ptr [rsp + 16]
.LBB2_18:
        cmp     r9, r11
        jae     .LBB2_40
        mov     rcx, r11
        sub     rcx, r9
        mov     rdx, rcx
        sub     rdx, rbx
        jb      .LBB2_40
        mov     r12, r9
        lea     rsi, [rbx + 32]
        cmp     rcx, rsi
        jb      .LBB2_22
        mov     r12, r9
        cmp     rdx, 32
        jge     .LBB2_32
.LBB2_22:
        add     rcx, r9
        add     rcx, qword ptr [rsp + 24]
        jmp     .LBB2_25
.LBB2_23:
        cmp     sil, bpl
        jbe     .LBB2_30
.LBB2_24:
        inc     r12
.LBB2_25:
        cmp     r12, rcx
        ja      .LBB2_40
        xor     edx, edx
        xor     esi, esi
        jmp     .LBB2_28
.LBB2_27:
        inc     rdx
        cmp     rbx, rdx
        je      .LBB2_23
.LBB2_28:
        movzx   edi, byte ptr [r12 + rdx]
        cmp     dil, byte ptr [r14 + rdx]
        je      .LBB2_27
        inc     sil
        cmp     sil, bpl
        jbe     .LBB2_27
        jmp     .LBB2_24
.LBB2_30:
        vmovdqu ymmword ptr [rsp + 64], ymm2
        mov     dword ptr [rsp + 12], r13d
        mov     qword ptr [rsp + 40], r10
.LBB2_31:
        mov     rcx, qword ptr [rsp + 56]
        lea     rdi, [r12 + rcx]
        cmp     rdi, r9
        cmovb   rdi, r9
        mov     rcx, qword ptr [rsp + 48]
        lea     rax, [rdi + rcx]
        mov     rsi, r11
        sub     rsi, rdi
        cmp     rax, r11
        cmovbe  rsi, rcx
        mov     rdx, r14
        mov     rcx, rbx
        mov     r8, qword ptr [rsp + 32]
        mov     r13, r11
        vzeroupper
        call    Railgun_Fuzzy_WuManber64(char const*, unsigned long, char const*, unsigned long, unsigned char)
        mov     r11, r13
        inc     r12
        mov     r9, r12
        test    rax, rax
        mov     r10, qword ptr [rsp + 40]
        mov     r13d, dword ptr [rsp + 12]
        vmovdqu ymm2, ymmword ptr [rsp + 64]
        vpcmpeqd        ymm3, ymm3, ymm3
        je      .LBB2_18
        jmp     .LBB2_48
.LBB2_32:
        add     rdx, r9
        add     rdx, -32
        mov     r12, r9
        jmp     .LBB2_35
.LBB2_33:
        test    edi, edi
        jne     .LBB2_39
.LBB2_34:
        add     r12, 32
        cmp     r12, rdx
        ja      .LBB2_22
.LBB2_35:
        vpxor   xmm0, xmm0, xmm0
        xor     esi, esi
        jmp     .LBB2_37
.LBB2_36:
        inc     rsi
        cmp     rbx, rsi
        je      .LBB2_33
.LBB2_37:
        vpbroadcastb    ymm1, byte ptr [r14 + rsi]
        vpcmpeqb        ymm1, ymm1, ymmword ptr [r12 + rsi]
        vpaddb  ymm0, ymm0, ymm1
        vpsubb  ymm0, ymm0, ymm3
        mov     r8d, esi
        not     r8d
        vpmaxub ymm1, ymm2, ymm0
        vpcmpeqb        ymm1, ymm2, ymm1
        vpmovmskb       edi, ymm1
        test    r8b, 3
        jne     .LBB2_36
        test    edi, edi
        jne     .LBB2_36
        jmp     .LBB2_34
.LBB2_39:
        vmovdqu ymmword ptr [rsp + 64], ymm2
        mov     dword ptr [rsp + 12], r13d
        mov     qword ptr [rsp + 40], r10
        rep       bsf ecx, edi
        add     r12, rcx
        jmp     .LBB2_31
.LBB2_40:
        test    r13b, r13b
        je      .LBB2_47
        cmp     r15, r10
        jae     .LBB2_44
.LBB2_47:
        xor     eax, eax
.LBB2_48:
        add     rsp, 104
        pop     rbx
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
        vzeroupper
        ret
.LBB2_44:
        mov     eax, 1
        sub     rax, rbx
        lea     rbp, [r11 + rax]
        add     rax, r15
        sub     r11, r10
        xor     ecx, ecx
        test    rax, rax
        cmovs   rbp, qword ptr [rsp + 16]
        cmovle  rax, rcx
        sub     r15, rax
        mov     r12, qword ptr [rsp + 32]
.LBB2_45:
        cmp     rbp, r11
        ja      .LBB2_47
        mov     rdi, rbp
        mov     rsi, r15
        mov     rdx, r14
        mov     rcx, rbx
        mov     r8d, r12d
        mov     r13, r11
        vzeroupper
        call    Railgun_Fuzzy_WuManber64(char const*, unsigned long, char const*, unsigned long, unsigned char)
        mov     r11, r13
        inc     rbp
        dec     r15
        test    rax, rax
        je      .LBB2_45
        jmp     .LBB2_48
.LBB2_49:
        mov     rax, qword ptr [rsp + 16]
        jmp     .LBB2_48
*/
