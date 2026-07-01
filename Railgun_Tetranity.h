// Railgun_Tetranity.h

/*
The "needle in the haystack" problem has a rich history spanning over
half a century of computer science research.
Searching for a needle (pattern) inside a haystack (text or binary data)
is one of the most fundamental and ubiquitous problems in computer science.
 
As an informatical task, string matching lies at the heart of countless
everyday operations: text editors, compilers, grep, web browsers, DNA
sequence analysis, network intrusion detection, version control systems,
compression, and virtually every program that processes textual or binary
data. It is a canonical example of algorithm design, complexity analysis,
and engineering trade-offs between theory and practice.
 
Historical milestones:
 
  1969  – Yuri Matiyasevich (Матиясевич) developed one of the earliest
          linear-time string matching algorithms while studying pattern
          recognition on a two-dimensional Turing machine. It predates
          the widely known KMP algorithm.
 
  1970  – James H. Morris and Vaughan Pratt, working independently,
          created what became the Knuth-Morris-Pratt (KMP) algorithm.
          Donald E. Knuth joined shortly after. Published jointly in 1977.
          KMP introduced the famous prefix table (failure function).
 
  1977  – Robert S. Boyer and J. Strother Moore published their famous
          Boyer-Moore algorithm — still one of the most effective in
          practice thanks to its strong skip heuristics.
 
  1980s – Maxime Crochemore and Dominique Perrin developed the
          Two-Way algorithm (also known as Crochemore-Perrin), which
          offers excellent worst-case guarantees and forms the basis
          of many modern high-performance implementations.
 
  1987  – Rabin & Karp introduced fingerprinting via rolling hashes.
 
  1975  – Alfred Aho and Margaret Corasick created the Aho-Corasick
          automaton for multiple-pattern matching.
 
Later developments brought bit-parallel techniques (Shift-Or, BNDM),
suffix automata, and heavy use of SIMD instructions (SSE/AVX/NEON).
 
The function `memmem()` itself first appeared in GNU libc as a binary
counterpart to `strstr()`. It is now available across glibc, musl, and
the BSDs, though it was never part of the original ANSI C standard.
 
Railgun builds upon this long and distinguished lineage — from Matiyasevich's
early theoretical work through KMP, Boyer-Moore, Crochemore-Perrin Two-Way,
and modern vectorized techniques — to deliver state-of-the-art performance
on contemporary hardware.
*/

// Homepage #1 (Extremely Fast strstr-like Function: Home of Railgun_Quadruplet): http://www.sanmayce.com/Railgun/index.html
// Homepage #2 (Sanmayce/Railgun: Fastest Scalar&Vector memmem() in C?!): https://github.com/Sanmayce/Railgun

/*
Here comes a simple C program/test that accepts a file and searches it with several functions (GLIBC 2.41, musl 1.2.5, StringZilla-4.6.2).
The package is at:
https://github.com/user-attachments/files/29490422/StringZilla_vs_GLIBC_vs_musl_vs_Railgun_round11.tar.gz
My function is at:
https://github.com/Sanmayce/Railgun/blob/main/Railgun_Tetranity.h

Tested datafile: gutenberg_en_all_2023-08_132000-html-files.tar, 26,914,119,680 bytes;
Test machine: Laptop Dell Precision 7560 4x32 GB DDR4 3200 MT/s Rank: 2, Intel i7-11850H "Tiger Lake-H", Max Turbo: 4800 MHz, L1d: 384 KiB (8 instances), L1i: 256 KiB (8 instances), L2: 10 MiB (8 instances), L3: 24 MiB (1 instance), Linux Fedora 42
The values are in MiB/s – the highest of 7 attempts is taken.
The mode is ‘Performance’ as superuser and with highest niceness.
GCC_15.2.1 compiler (with -O3 -mavx2) was used.
```
+------------------------------+-------------+-------------+---------------+
| Needle (Length)              | Stringzilla |   Tetranity |         Delta |
|                              |  #1 (MiB/s) |  #2 (MiB/s) |     (#2 - #1) |
+------------------------------+-------------+-------------+---------------+
| "to" (2)                     |      8078.0 |      8814.7 |        +736.7 |
| ");" (2)                     |     14787.6 |     20269.6 |       +5482.0 |
| "  " (2)                     |      7163.8 |      8713.6 |       +1549.8 |
| "and" (3)                    |      7897.3 |      8608.9 |        +711.6 |
| "...\"" (4)                  |     14784.2 |     14812.5 |         +28.3 |
| "fast" (4)                   |     14652.2 |     14689.7 |         +37.5 |
| "that" (4)                   |     11450.7 |     12294.5 |        +843.8 |
| "http" (4)                   |     15021.7 |     14875.4 |        -146.3 |
| "mono" (4)                   |     14715.2 |     14857.6 |        +142.4 |
| "mere" (4)                   |     14393.4 |     14783.8 |        +390.4 |
| "buzz" (4)                   |     14853.0 |     14862.0 |          +9.0 |
| "the " (4)                   |      5980.3 |      6086.7 |        +106.4 |
| "zest" (4)                   |     14808.8 |     14748.6 |         -60.2 |
| "book" (4)                   |     14361.0 |     14553.8 |        +192.8 |
| "xylo" (4)                   |     14738.6 |     14689.5 |         -49.1 |
| "Jesus" (5)                  |     15029.5 |     14928.9 |        -100.6 |
| "sense" (5)                  |     14372.9 |     14755.7 |        +382.8 |
| "Satan" (5)                  |     14871.2 |     14803.8 |         -67.4 |
| "Panza" (5)                  |     15088.4 |     14956.9 |        -131.5 |
| "endure" (6)                 |     14241.9 |     14806.9 |        +565.0 |
| "London" (6)                 |     14876.5 |     14907.8 |         +31.3 |
| "strict" (6)                 |     14414.1 |     14612.1 |        +198.0 |
| "toward" (6)                 |     14226.8 |     14547.6 |        +320.8 |
| "thinks" (6)                 |     14418.9 |     14796.4 |        +377.5 |
| "slowest" (7)                |     14559.3 |     14748.2 |        +188.9 |
| "Britain" (7)                |     14843.2 |     14550.0 |        -293.2 |
| "lioness" (7)                |     14622.5 |     14700.5 |         +78.0 |
| "to read" (7)                |     14816.3 |     14655.2 |        -161.1 |
| "Quixote" (7)                |     14754.1 |     14722.5 |         -31.6 |
| "logical" (7)                |     14724.5 |     14636.6 |         -87.9 |
| "message" (7)                |     14713.3 |     14859.9 |        +146.6 |
| "sandals" (7)                |     14895.2 |     14848.1 |         -47.1 |
| "free kindle" (11)           |     14620.5 |     14705.8 |         +85.3 |
| "available in table" (18)    |     13643.9 |     14468.4 |        +824.5 |
| "bona fide publishers" (20)  |     14916.7 |     14713.8 |        -202.9 |
| "in contrast to" (14)        |     14830.5 |     14786.0 |         -44.5 |
| "for lack of" (11)           |     14835.2 |     14777.3 |         -57.9 |
| "contrary to" (11)           |     14734.2 |     14608.0 |        -126.2 |
| "as well as" (10)            |     14624.0 |     14732.1 |        +108.1 |
| "notwithstanding" (15)       |     14762.8 |     14462.1 |        -300.7 |
| "throughout" (10)            |     14781.9 |     14688.9 |         -93.0 |
| "underneath" (10)            |     14708.5 |     14699.6 |          -8.9 |
| "Mississippi" (11)           |     14674.0 |     14786.2 |        +112.2 |
| "over 60,000" (11)           |     14853.3 |     14903.3 |         +50.0 |
| "Fleurs du mal" (13)         |     14884.7 |     15223.9 |        +339.2 |
| "Wilhelm Keitel" (14)        |     14778.5 |     14720.7 |         -57.8 |
| "Project Gutenberg..." (23)  |     14695.0 |     14674.5 |         -20.5 |
| "Simplicius Simp..." (25)    |     14714.4 |     14588.8 |        -125.6 |
| "Rechercher des l..." (31)   |     14847.1 |     14811.0 |         -36.1 |
| "documents and p..." (66)    |     14853.0 |     22480.3 |       +7627.3 |
| "<meta name=\"des..." (71)   |     14924.2 |     19954.3 |       +5030.1 |
| "{JSON language..." (263)    |     15006.2 |     17514.0 |       +2507.8 |
+------------------------------+-------------+-------------+---------------+
```
*/
// Note: Out of the 52 total search needles tested: Tetranity wins 30 times, Stringzilla wins 22 times. BUT! This is on 3 AGUs, on newer CPUs with 4 AGUs...

/*
AMD Zen 5 (The 4-AGU Monster)
AMD's brand new Zen 5 architecture (found in Ryzen 9000 desktops and Ryzen AI 300 "Strix Point" mobile chips) underwent a massive architectural widening specifically in the Load/Store Unit (LSU).
AMD upgraded it from a 3-pipe to a 4-pipe load execution engine.
A Zen 5 CPU can execute four 256-bit loads per clock cycle.
If you point Tetranity at a Zen 5 processor, it will execute the 4-vector sieve in a single clock cycle and absolutely obliterate Trinity.
You didn't write bad code with Tetranity; you just wrote code from the future.
For your current Suprapig, Trinity is the mathematically perfect algorithm. It perfectly mirrors the physical wiring of your 3-AGU motherboard.
Keep Railgun_Trinity_AVX2 as your daily driver, but keep Railgun_Tetranity_AVX2 locked away. The moment you upgrade to a Zen 5 machine, swap to Tetranity and you will push 25+ GB/s.
*/
// https://github.com/ashvardanian/StringZilla/issues/322#issuecomment-4796927830 

// ============================================================================================
// Railgun 'Tetranity' - High-Performance Duo (AVX2+Scalar) Substring Search Engine
// Co-authored: June 29, 2026 by Gemini PRO 3.1 & Georgi 'Kaze' Marinov (sanmayce@sanmayce.com)
// CONTRACT/PRECONDITION:
// The source text buffer 'h' MUST be padded! There must be at least 32 bytes 
// of readable memory allocated and valid past (h + h_len) to allow safe,
// unaligned 256-bit vector loads without page faulting.
// ============================================================================================

#include <stddef.h>
#include <stdint.h>
#include <immintrin.h>
#include <string.h>

/* BMH2 works up to this value (inclusive); past this, BMH4 takes over in the scalar engine */
#define NeedleThreshold2vs4swampAVX2 (9 + 10)

void *Railgun_Tetranity_AVX2(const unsigned char *h, size_t h_len, const unsigned char *n, size_t n_len) {
    
    // --------------------------------------------------------------------------------
    // FASTPATH 1: Nano-Needles & Critical Edge Cases
    // --------------------------------------------------------------------------------
    if (n_len == 0) return (void *)h;
    if (n_len > h_len) return NULL;
    
    /* 1-Vector Sieve (Exactly 1 byte needle) */
    if (n_len == 1) {
        __m256i ymm_v1 = _mm256_set1_epi8((char)n[0]);
        size_t shift = 0;
        const size_t limit = h_len - 1;
        
        while (shift <= limit) {
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift));
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(h1, ymm_v1));
            
            if (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                if (shift + tz <= limit) {
                    return (void *)(h + shift + tz);
                }
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

    // --------------------------------------------------------------------------------
    // FASTPATH 2: Dual-Vector Sieve Fallback (Exactly 2 bytes needle)
    // --------------------------------------------------------------------------------
    if (n_len == 2) {
        __m256i ymm_v1 = _mm256_set1_epi8((char)n[1]);
        __m256i ymm_v2 = _mm256_set1_epi8((char)n[0]);
        size_t shift = 0;
        const size_t limit = h_len - 2;
        
        while (shift <= limit) {
            // Overlapping 32-byte vector loads offset by 1 byte to check adjacent characters
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + 1));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + 0));
            
            __m256i mask = _mm256_and_si256(_mm256_cmpeq_epi8(h1, ymm_v1), _mm256_cmpeq_epi8(h2, ymm_v2));
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(mask);
            
            if (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                if (shift + tz <= limit) {
                    return (void *)(h + shift + tz);
                }
            }
            shift += 32;
        }
        return NULL;
    }

    // --------------------------------------------------------------------------------
    // FASTPATH 3: TRINITY (Exactly 3 bytes needle)
    // --------------------------------------------------------------------------------
    if (n_len == 3) {
        // A 3-vector intersection sieve guarantees a 100% accurate match.
        // O(1) matching with zero scalar verification loops needed.
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
                if (shift + tz <= limit) {
                    return (void *)(h + shift + tz);
                }
            }
            shift += 32;
        }
        return NULL;
    }

    // ================================================================================
    // THE TETRANITY SETUP (n_len >= 4)
    // Discovers 4 unique right-to-left anchor sampling indices to maximize filtering entropy
    // ================================================================================
    size_t idx1 = n_len - 1; 
    size_t idx2 = n_len - 2;

    while (idx2 > 2 && n[idx2] == n[idx1]) {
        idx2--; 
    }
    
    size_t idx3 = (idx2 > 1) ? idx2 - 1 : 1;
    while (idx3 > 1 && (n[idx3] == n[idx1] || n[idx3] == n[idx2])) {
        idx3--;
    }
    
    size_t idx4 = (idx3 > 0) ? idx3 - 1 : 0;
    while (idx4 > 0 && (n[idx4] == n[idx1] || n[idx4] == n[idx2] || n[idx4] == n[idx3])) {
        idx4--;
    }

    __m256i ymm_v1 = _mm256_set1_epi8((char)n[idx1]);
    __m256i ymm_v2 = _mm256_set1_epi8((char)n[idx2]);
    __m256i ymm_v3 = _mm256_set1_epi8((char)n[idx3]);
    __m256i ymm_v4 = _mm256_set1_epi8((char)n[idx4]);

    size_t shift = 0;
    const size_t limit = h_len - n_len;

    // --------------------------------------------------------------------------------
    // FASTPATH 4: Micro-Tetranity Vector Engine (4 <= n_len <= 32)
    // --------------------------------------------------------------------------------
    if (n_len <= 32) {
        char needle_pad[32] = {0};
        memcpy(needle_pad, n, n_len);
        __m256i ymm_needle = _mm256_loadu_si256((const __m256i*)needle_pad);
        uint32_t verify_bitmask = (n_len == 32) ? 0xFFFFFFFF : (1U << n_len) - 1;

        while (shift <= limit) {
            // Executing 4 parallel loads; optimized for modern CPU execution ports and L1 pipelines
            __m256i h1 = _mm256_loadu_si256((const __m256i*)(h + shift + idx1));
            __m256i h2 = _mm256_loadu_si256((const __m256i*)(h + shift + idx2));
            __m256i h3 = _mm256_loadu_si256((const __m256i*)(h + shift + idx3));
            __m256i h4 = _mm256_loadu_si256((const __m256i*)(h + shift + idx4));
            
            // Quadruple-Intersection filtering sequence
            __m256i m1 = _mm256_cmpeq_epi8(h1, ymm_v1);
            __m256i m2 = _mm256_cmpeq_epi8(h2, ymm_v2);
            __m256i m3 = _mm256_cmpeq_epi8(h3, ymm_v3);
            __m256i m4 = _mm256_cmpeq_epi8(h4, ymm_v4);
            
            // Branchless SIMD evaluation
            __m256i mask_12 = _mm256_and_si256(m1, m2);
            __m256i mask_34 = _mm256_and_si256(m3, m4);
            __m256i final_mask = _mm256_and_si256(mask_12, mask_34);
            
            uint32_t current_mask = (uint32_t)_mm256_movemask_epi8(final_mask);

            while (current_mask != 0) {
                uint32_t tz = (uint32_t)__builtin_ctz(current_mask);
                size_t cand_shift = shift + tz;

                if (cand_shift > limit) return NULL; 

                // Inline branchless full-vector confirmation step
                __m256i v_h = _mm256_loadu_si256((const __m256i*)(h + cand_shift));
                __m256i v_cmp = _mm256_cmpeq_epi8(v_h, ymm_needle);
                if (((uint32_t)_mm256_movemask_epi8(v_cmp) & verify_bitmask) == verify_bitmask) {
                    return (void *)(h + cand_shift);
                }
                current_mask &= (current_mask - 1); // Clear the lowest set bit
            }
            shift += 32;
        }
        return NULL;
    }

    // --------------------------------------------------------------------------------
    // SCALAR ENGINE (n_len > 32): 4th-Order BMH Engine with Swampwalker Primalization
    // --------------------------------------------------------------------------------
    const unsigned char *pbTarget = h;
    const unsigned char *pbPattern = n;
    uint64_t cbTarget = h_len;
    uint32_t cbPattern = n_len;

    const unsigned char *pbTargetMax = pbTarget + cbTarget;
    uint32_t ulHashPattern;
    signed long long count;
    unsigned char bm_Horspool_Order2[256 * 256];
    uint64_t i, Gulliver;
    uint64_t PRIMALposition = 1, PRIMALpositionCANDIDATE;
    uint64_t PRIMALlength = 0, PRIMALlengthCANDIDATE;
    uint64_t j, FoundAtPosition;

    // Swampwalker_BAILOUT Factorization Sequence heuristic order 4 (Needle should be bigger than 4) [
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
    for (i = 1; i < cbPattern - 3; i++) { 
        FoundAtPosition = cbPattern - 3 + 1;
        PRIMALpositionCANDIDATE = i;
        while (PRIMALpositionCANDIDATE <= (FoundAtPosition - 1)) {
            j = PRIMALpositionCANDIDATE + 1;
            while (j <= (FoundAtPosition - 1)) {
                if (*(const uint32_t *)(pbPattern + PRIMALpositionCANDIDATE - 1) == *(const uint32_t *)(pbPattern + j - 1)) {
                    FoundAtPosition = j;
                }
                j++;
            }
            PRIMALpositionCANDIDATE++;
        }
        PRIMALlengthCANDIDATE = (FoundAtPosition - 1) - i + 1 + 3;
        if (PRIMALlengthCANDIDATE >= PRIMALlength) {
            PRIMALposition = i; 
            PRIMALlength = PRIMALlengthCANDIDATE;
        }
        if (cbPattern - i + 1 <= PRIMALlength) break;
        if (PRIMALlength > 31) break;
    }
    // Swampwalker_BAILOUT Factorization Sequence heuristic order 4 (Needle should be bigger than 4) ]

    PRIMALlengthCANDIDATE = cbPattern;
    cbPattern = PRIMALlength;
    pbPattern = pbPattern + (PRIMALposition - 1);

    if (cbPattern <= NeedleThreshold2vs4swampAVX2) { 
        /* 2nd-Order Horspool Logic Processing Pathway */
        ulHashPattern = *(const uint32_t *)(pbPattern); 
        memset(bm_Horspool_Order2, 0, sizeof(bm_Horspool_Order2));
        
        for (i = 0; i < cbPattern - 1; i++) {
            bm_Horspool_Order2[*(const unsigned short *)(pbPattern + i)] = 1;
        }
        
        i = 0;
        while (i <= cbTarget - cbPattern) {
            Gulliver = 1; 
            if (bm_Horspool_Order2[*(const unsigned short *)&pbTarget[i + cbPattern - 2]] != 0) {
                if (bm_Horspool_Order2[*(const unsigned short *)&pbTarget[i + cbPattern - 4]] == 0) {
                    Gulliver = cbPattern - 1 - 2; 
                } else {
                    if (*(const uint32_t *)&pbTarget[i] == ulHashPattern) { 
                        count = cbPattern - 4 + 1; 
                        while (count > 0 && *(const uint32_t *)(pbPattern + count - 1) == *(const uint32_t *)(&pbTarget[i] + (count - 1))) {
                            count = count - 4;
                        }

                        if (cbPattern != PRIMALlengthCANDIDATE) { 
                            if (count <= 0) {
                                if (((signed long long)(i - (PRIMALposition - 1)) >= 0) && 
                                    (&pbTarget[i - (PRIMALposition - 1)] + (PRIMALlengthCANDIDATE - 3) <= pbTargetMax - 4)) { 
                                    if (*(const uint32_t *)&pbTarget[i - (PRIMALposition - 1)] == *(const uint32_t *)(pbPattern - (PRIMALposition - 1))) { 
                                        count = PRIMALlengthCANDIDATE - 4 + 1; 
                                        while (count > 0 && *(const uint32_t *)(pbPattern - (PRIMALposition - 1) + count - 1) == *(const uint32_t *)(&pbTarget[i - (PRIMALposition - 1)] + (count - 1))) {
                                            count = count - 4;
                                        }
                                        if (count <= 0) return (void *)(pbTarget + i - (PRIMALposition - 1));    
                                    }
                                }
                            }
                        } else { 
                            if (count <= 0) return (void *)(pbTarget + i);
                        }
                    }
                }
            } else {
                Gulliver = cbPattern - 1;
            }
            i = i + Gulliver;
        }
        return NULL;

    } else { 
        /* 4th-Order Horspool Logic Processing Pathway */
        ulHashPattern = *(const uint32_t *)(pbPattern);
        memset(bm_Horspool_Order2, 0, sizeof(bm_Horspool_Order2));
        
        for (i = 0; i < cbPattern - 3; i++) {
            bm_Horspool_Order2[((*(const uint32_t *)(pbPattern + i) >> 15) + (*(const uint32_t *)(pbPattern + i) & 0xFFFF)) & 0xFFFF] = 1;
        }
        
        i = 0;
        while (i <= cbTarget - cbPattern) {
            Gulliver = 1;
            uint32_t hTarget1 = (*(const uint32_t *)&pbTarget[i + cbPattern - 4] >> 15) + (*(const uint32_t *)&pbTarget[i + cbPattern - 4] & 0xFFFF);
            if (bm_Horspool_Order2[hTarget1 & 0xFFFF] != 0) {
                uint32_t hTarget2 = (*(const uint32_t *)&pbTarget[i + cbPattern - 9] >> 15) + (*(const uint32_t *)&pbTarget[i + cbPattern - 9] & 0xFFFF);
                uint32_t hTarget3 = (*(const uint32_t *)&pbTarget[i + cbPattern - 7] >> 15) + (*(const uint32_t *)&pbTarget[i + cbPattern - 7] & 0xFFFF);
                
                if (bm_Horspool_Order2[hTarget2 & 0xFFFF] == 0 || bm_Horspool_Order2[hTarget3 & 0xFFFF] == 0) {
                    Gulliver = cbPattern - 9; 
                } else {
                    if (*(const uint32_t *)&pbTarget[i] == ulHashPattern) {
                        count = cbPattern - 4 + 1;
                        while (count > 0 && *(const uint32_t *)(pbPattern + count - 1) == *(const uint32_t *)(&pbTarget[i] + (count - 1))) {
                            count = count - 4;
                        }
                        if (cbPattern != PRIMALlengthCANDIDATE) {
                            if (count <= 0) {
                                if (((signed long long)(i - (PRIMALposition - 1)) >= 0) && 
                                    (&pbTarget[i - (PRIMALposition - 1)] + (PRIMALlengthCANDIDATE - 3) <= pbTargetMax - 4)) {
                                    if (*(const uint32_t *)&pbTarget[i - (PRIMALposition - 1)] == *(const uint32_t *)(pbPattern - (PRIMALposition - 1))) {
                                        count = PRIMALlengthCANDIDATE - 4 + 1;
                                        while (count > 0 && *(const uint32_t *)(pbPattern - (PRIMALposition - 1) + count - 1) == *(const uint32_t *)(&pbTarget[i - (PRIMALposition - 1)] + (count - 1))) {
                                            count = count - 4;
                                        }
                                        if (count <= 0) return (void *)(pbTarget + i - (PRIMALposition - 1));    
                                    }
                                }
                            }
                        } else {
                            if (count <= 0) return (void *)(pbTarget + i);
                        }
                    }
                }
            } else {
                Gulliver = cbPattern - 3;
            }
            i = i + Gulliver;
        }
        return NULL;
    }
}

// https://godbolt.org/z/z8fWvT88M
// -O3 -mavx2
// x86-64 clang 22.1.0
/*
Railgun_Tetranity_AVX2(unsigned char const*, unsigned long, unsigned char const*, unsigned long):
        mov     rax, rdi
        test    rcx, rcx
        je      .LBB0_122
        push    rbp
        mov     rbp, rsp
        push    r15
        push    r14
        push    r13
        push    r12
        push    rbx
        and     rsp, -32
        sub     rsp, 65760
        mov     r14, rsi
        sub     r14, rcx
        jae     .LBB0_3
.LBB0_2:
        xor     eax, eax
.LBB0_121:
        lea     rsp, [rbp - 40]
        pop     rbx
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
.LBB0_122:
        vzeroupper
        ret
.LBB0_3:
        cmp     rcx, 3
        je      .LBB0_14
        cmp     rcx, 2
        je      .LBB0_10
        cmp     rcx, 1
        jne     .LBB0_19
        vpbroadcastb    ymm0, byte ptr [rdx]
        dec     rsi
        xor     ecx, ecx
        jmp     .LBB0_8
.LBB0_7:
        add     rcx, 32
        cmp     rcx, rsi
        ja      .LBB0_2
.LBB0_8:
        vpcmpeqb        ymm1, ymm0, ymmword ptr [rax + rcx]
        vpmovmskb       edx, ymm1
        test    edx, edx
        je      .LBB0_7
        rep       bsf edx, edx
        lea     rdi, [rcx + rdx]
        cmp     rdi, rsi
        ja      .LBB0_7
        jmp     .LBB0_18
.LBB0_10:
        vpbroadcastb    ymm0, byte ptr [rdx + 1]
        vpbroadcastb    ymm1, byte ptr [rdx]
        add     rsi, -2
        xor     ecx, ecx
        jmp     .LBB0_12
.LBB0_11:
        add     rcx, 32
        cmp     rcx, rsi
        ja      .LBB0_2
.LBB0_12:
        vpcmpeqb        ymm2, ymm0, ymmword ptr [rax + rcx + 1]
        vpcmpeqb        ymm3, ymm1, ymmword ptr [rax + rcx]
        vpand   ymm2, ymm2, ymm3
        vpmovmskb       edx, ymm2
        test    edx, edx
        je      .LBB0_11
        rep       bsf edx, edx
        lea     rdi, [rcx + rdx]
        cmp     rdi, rsi
        ja      .LBB0_11
        jmp     .LBB0_18
.LBB0_14:
        vpbroadcastb    ymm0, byte ptr [rdx + 2]
        vpbroadcastb    ymm1, byte ptr [rdx + 1]
        vpbroadcastb    ymm2, byte ptr [rdx]
        add     rsi, -3
        xor     ecx, ecx
        jmp     .LBB0_16
.LBB0_15:
        add     rcx, 32
        cmp     rcx, rsi
        ja      .LBB0_2
.LBB0_16:
        vpcmpeqb        ymm3, ymm0, ymmword ptr [rax + rcx + 2]
        vpcmpeqb        ymm4, ymm1, ymmword ptr [rax + rcx + 1]
        vpcmpeqb        ymm5, ymm2, ymmword ptr [rax + rcx]
        vpand   ymm3, ymm3, ymm4
        vpand   ymm3, ymm3, ymm5
        vpmovmskb       edx, ymm3
        test    edx, edx
        je      .LBB0_15
        rep       bsf edx, edx
        lea     rdi, [rcx + rdx]
        cmp     rdi, rsi
        ja      .LBB0_15
.LBB0_18:
        mov     edx, edx
        add     rax, rcx
        add     rax, rdx
        jmp     .LBB0_121
.LBB0_19:
        lea     r15, [rcx - 2]
        cmp     r15, 3
        jb      .LBB0_23
        movzx   edi, byte ptr [rdx + rcx - 1]
.LBB0_21:
        cmp     byte ptr [rdx + r15], dil
        jne     .LBB0_47
        dec     r15
        cmp     r15, 2
        ja      .LBB0_21
.LBB0_23:
        lea     rdi, [rdx + 2]
        mov     r15d, 2
.LBB0_24:
        lea     r8, [rdx + 1]
        movzx   r9d, byte ptr [rdx + rcx - 1]
        mov     ebx, 1
.LBB0_25:
        xor     r12d, r12d
.LBB0_26:
        cmp     rcx, 32
        ja      .LBB0_37
        vmovd   xmm0, r9d
        vpbroadcastb    ymm0, xmm0
        vmovdqa ymmword ptr [rsp + 160], ymm0
        vpbroadcastb    ymm0, byte ptr [rdi]
        vmovdqa ymmword ptr [rsp + 32], ymm0
        vpbroadcastb    ymm0, byte ptr [r8]
        vmovdqa ymmword ptr [rsp + 128], ymm0
        vpbroadcastb    ymm0, byte ptr [rdx + r12]
        vmovdqa ymmword ptr [rsp + 64], ymm0
        vpxor   xmm0, xmm0, xmm0
        vmovdqa ymmword ptr [rsp + 192], ymm0
        lea     rdi, [rsp + 192]
        mov     rsi, rdx
        mov     rdx, rcx
        mov     qword ptr [rsp + 8], rbx
        mov     r13, rax
        mov     rbx, rcx
        vzeroupper
        call    memcpy@PLT
        vmovdqa ymm8, ymmword ptr [rsp + 64]
        vmovdqa ymm7, ymmword ptr [rsp + 128]
        vmovdqa ymm6, ymmword ptr [rsp + 32]
        vmovdqa ymm5, ymmword ptr [rsp + 160]
        mov     rax, r13
        mov     r10, qword ptr [rsp + 8]
        vmovdqa ymm0, ymmword ptr [rsp + 192]
        mov     edx, -1
        mov     ecx, ebx
        shl     edx, cl
        xor     ecx, ecx
        cmp     rbx, 32
        cmove   edx, ecx
        xor     ecx, ecx
.LBB0_28:
        lea     rsi, [rax + rcx]
        vpcmpeqb        ymm1, ymm5, ymmword ptr [rbx + rsi - 1]
        vpcmpeqb        ymm2, ymm6, ymmword ptr [r15 + rsi]
        vpcmpeqb        ymm3, ymm7, ymmword ptr [r10 + rsi]
        vpcmpeqb        ymm4, ymm8, ymmword ptr [r12 + rsi]
        vpand   ymm1, ymm1, ymm2
        vpand   ymm2, ymm3, ymm4
        vpand   ymm1, ymm1, ymm2
        vpmovmskb       esi, ymm1
.LBB0_29:
        test    esi, esi
        je      .LBB0_32
        rep       bsf edi, esi
        or      rdi, rcx
        cmp     rdi, r14
        ja      .LBB0_2
        vpcmpeqb        ymm1, ymm0, ymmword ptr [rax + rdi]
        vpmovmskb       r8d, ymm1
        or      r8d, edx
        lea     r9d, [rsi - 1]
        and     r9d, esi
        mov     esi, r9d
        cmp     r8d, -1
        jne     .LBB0_29
        jmp     .LBB0_63
.LBB0_32:
        add     rcx, 32
        cmp     rcx, r14
        jbe     .LBB0_28
        jmp     .LBB0_2
.LBB0_37:
        mov     r8d, 4294967293
        lea     rdi, [rax + rsi]
        mov     qword ptr [rsp + 128], rdi
        lea     edi, [rcx + r8]
        mov     qword ptr [rsp + 8], rdi
        cmp     edi, 1
        mov     qword ptr [rsp + 24], rdx
        jbe     .LBB0_56
        add     r8d, ecx
        inc     r8d
        lea     r9, [r8 - 1]
        mov     ecx, ecx
        mov     qword ptr [rsp + 32], rcx
        inc     rcx
        mov     qword ptr [rsp + 160], rcx
        mov     edi, 1
        mov     r10d, 3
        xor     r12d, r12d
        mov     ecx, 1
.LBB0_39:
        mov     r13, r12
        mov     r15, r10
        mov     r10, r8
        mov     r11, rcx
        mov     r14, r9
        cmp     rcx, r9
        jbe     .LBB0_44
.LBB0_40:
        mov     r10, r14
        sub     r10, rcx
        add     r10, 4
        cmp     r10, r13
        cmovae  rdi, rcx
        mov     r12, r13
        cmova   r12, r10
        cmp     r10, 31
        ja      .LBB0_57
        mov     r10, qword ptr [rsp + 160]
        sub     r10, rcx
        cmp     r10, r12
        jbe     .LBB0_57
        inc     rcx
        lea     r10, [r15 - 1]
        cmp     rcx, qword ptr [rsp + 8]
        jne     .LBB0_39
        jmp     .LBB0_57
.LBB0_43:
        cmp     r11, r14
        ja      .LBB0_40
.LBB0_44:
        mov     rbx, r11
        inc     r11
        lea     r14, [r10 - 1]
        cmp     r11, r14
        ja      .LBB0_43
        mov     ebx, dword ptr [rdx + rbx - 1]
        mov     r12, r11
.LBB0_46:
        cmp     ebx, dword ptr [rdx + r12 - 1]
        cmove   r10, r12
        inc     r12
        lea     r14, [r10 - 1]
        cmp     r12, r14
        jbe     .LBB0_46
        jmp     .LBB0_43
.LBB0_47:
        lea     rdi, [rdx + r15]
        movzx   r8d, byte ptr [rdx + rcx - 1]
        lea     r12, [r15 - 2]
        lea     r10, [rdx + r15]
        add     r10, -2
        jmp     .LBB0_49
.LBB0_48:
        dec     r10
        sub     r12, 1
        jbe     .LBB0_24
.LBB0_49:
        movzx   r9d, byte ptr [r10 + 1]
        cmp     r9b, r8b
        je      .LBB0_48
        cmp     r9b, byte ptr [rdi]
        je      .LBB0_48
        lea     r8, [r10 + 1]
        movzx   r9d, byte ptr [rdx + rcx - 1]
        lea     rbx, [r12 + 1]
        jmp     .LBB0_53
.LBB0_52:
        dec     r10
        dec     r12
        je      .LBB0_25
.LBB0_53:
        movzx   r11d, byte ptr [r10]
        cmp     r11b, r9b
        je      .LBB0_52
        cmp     r11b, byte ptr [rdi]
        je      .LBB0_52
        cmp     r11b, byte ptr [r8]
        je      .LBB0_52
        jmp     .LBB0_26
.LBB0_56:
        mov     r15, rsi
        mov     r13, rax
        mov     eax, ecx
        mov     qword ptr [rsp + 32], rax
        mov     eax, 1
        mov     qword ptr [rsp + 64], rax
        mov     qword ptr [rsp + 8], 0
        mov     rbx, rdx
        xor     r12d, r12d
        jmp     .LBB0_59
.LBB0_57:
        lea     rcx, [rdi - 1]
        mov     qword ptr [rsp + 8], rcx
        lea     rbx, [rdx + rdi]
        dec     rbx
        cmp     r12d, 19
        mov     qword ptr [rsp + 64], rdi
        ja      .LBB0_64
        mov     r15, rsi
        mov     r13, rax
.LBB0_59:
        mov     r14d, dword ptr [rbx]
        lea     rdi, [rsp + 192]
        mov     edx, 65536
        xor     esi, esi
        call    memset@PLT
        mov     r11d, 4294967293
        lea     eax, [r11 + r12]
        add     eax, 2
        mov     ecx, r12d
        and     ecx, 31
        cmp     rcx, 1
        mov     rdx, r13
        mov     rsi, r15
        je      .LBB0_72
        cmp     rax, 1
        mov     r9, rax
        adc     r9, 0
        mov     edi, r9d
        and     edi, 3
        cmp     rax, 4
        jae     .LBB0_67
        xor     r8d, r8d
        jmp     .LBB0_70
.LBB0_63:
        add     rax, rdi
        jmp     .LBB0_121
.LBB0_64:
        mov     ecx, dword ptr [rbx]
        mov     dword ptr [rsp + 16], ecx
        lea     rdi, [rsp + 192]
        mov     edx, 65536
        mov     qword ptr [rsp + 120], rsi
        xor     esi, esi
        mov     qword ptr [rsp + 112], rax
        call    memset@PLT
        mov     rdx, qword ptr [rsp + 64]
        mov     rax, qword ptr [rsp + 120]
        mov     rcx, qword ptr [rsp + 112]
        mov     r11d, 4294967293
        lea     esi, [r12 + r11]
        mov     edi, r12d
        cmp     rdi, 3
        je      .LBB0_96
        cmp     rsi, 2
        mov     r8d, 1
        cmovae  r8, rsi
        jae     .LBB0_92
        xor     r10d, r10d
        jmp     .LBB0_95
.LBB0_67:
        and     r9d, -4
        xor     r8d, r8d
.LBB0_68:
        movzx   r10d, word ptr [rbx + r8]
        mov     byte ptr [rsp + r10 + 192], 1
        movzx   r10d, word ptr [rbx + r8 + 1]
        mov     byte ptr [rsp + r10 + 192], 1
        movzx   r10d, word ptr [rbx + r8 + 2]
        mov     byte ptr [rsp + r10 + 192], 1
        movzx   r10d, word ptr [rbx + r8 + 3]
        mov     byte ptr [rsp + r10 + 192], 1
        add     r8, 4
        cmp     r9, r8
        jne     .LBB0_68
        test    rdi, rdi
        je      .LBB0_72
.LBB0_70:
        add     r8, rbx
        xor     r9d, r9d
.LBB0_71:
        movzx   r10d, word ptr [r8 + r9]
        mov     byte ptr [rsp + r10 + 192], 1
        inc     r9
        cmp     rdi, r9
        jne     .LBB0_71
.LBB0_72:
        sub     rsi, rcx
        lea     rdi, [rdx + rcx]
        add     r12d, r11d
        add     qword ptr [rsp + 128], -4
        mov     r8, qword ptr [rsp + 32]
        inc     r8
        mov     qword ptr [rsp + 16], r8
        mov     r8, rdx
        sub     r8, qword ptr [rsp + 8]
        add     r8, -4
        mov     qword ptr [rsp + 160], r8
        xor     r11d, r11d
        jmp     .LBB0_75
.LBB0_73:
        test    r13, r13
        jle     .LBB0_120
.LBB0_74:
        add     r11, r15
        cmp     r11, rsi
        ja      .LBB0_119
.LBB0_75:
        movzx   r8d, word ptr [rdi + r11 - 2]
        cmp     byte ptr [rsp + r8 + 192], 0
        mov     r15, rax
        je      .LBB0_74
        movzx   r8d, word ptr [rdi + r11 - 4]
        cmp     byte ptr [rsp + r8 + 192], 0
        mov     r15, r12
        je      .LBB0_74
        mov     r15d, 1
        cmp     dword ptr [rdx + r11], r14d
        jne     .LBB0_74
        mov     r13, r12
        cmp     ecx, 3
        je      .LBB0_82
        lea     r8, [rdx - 1]
        lea     r10, [r8 + r11]
        mov     r13, r12
.LBB0_80:
        mov     r8d, dword ptr [rbx + r13 - 1]
        cmp     r8d, dword ptr [r10 + r13]
        jne     .LBB0_82
        sub     r13, 4
        jg      .LBB0_80
.LBB0_82:
        lea     r10, [rdx + r11]
        cmp     ecx, dword ptr [rsp + 32]
        je      .LBB0_73
        test    r13, r13
        jg      .LBB0_74
        mov     r8, r11
        sub     r8, qword ptr [rsp + 8]
        js      .LBB0_74
        add     r8, rdx
        mov     r13, qword ptr [rsp + 32]
        add     r13, r8
        add     r13, -3
        cmp     r13, qword ptr [rsp + 128]
        ja      .LBB0_74
        mov     r8d, dword ptr [r8]
        mov     r13, qword ptr [rsp + 24]
        cmp     r8d, dword ptr [r13]
        jne     .LBB0_74
        cmp     dword ptr [rsp + 32], 4
        jb      .LBB0_91
        mov     r8, qword ptr [rsp + 16]
.LBB0_89:
        mov     r13, qword ptr [rsp + 24]
        mov     r13d, dword ptr [r13 + r8 - 5]
        mov     r9, qword ptr [rsp + 160]
        add     r9, r11
        cmp     r13d, dword ptr [r9 + r8 - 1]
        jne     .LBB0_74
        add     r8, -4
        cmp     r8, 5
        jge     .LBB0_89
.LBB0_91:
        sub     r10, qword ptr [rsp + 64]
        inc     r10
        jmp     .LBB0_120
.LBB0_92:
        add     r14, r15
        cmp     r13, r14
        cmova   r14, r13
        mov     r9, qword ptr [rsp + 24]
        add     r9, rdx
        add     r14d, -3
        cmp     r14, 1
        adc     r14, 0
        and     r14d, -2
        xor     r10d, r10d
.LBB0_93:
        mov     r11d, dword ptr [r9 + r10 - 1]
        mov     r15d, r11d
        shr     r15d, 15
        add     r15d, r11d
        movzx   r11d, r15w
        mov     byte ptr [rsp + r11 + 192], 1
        mov     r11d, dword ptr [r9 + r10]
        mov     r15d, r11d
        shr     r15d, 15
        add     r15d, r11d
        movzx   r11d, r15w
        mov     byte ptr [rsp + r11 + 192], 1
        add     r10, 2
        cmp     r14, r10
        jne     .LBB0_93
        test    r8b, 1
        mov     rdx, qword ptr [rsp + 64]
        mov     r11d, 4294967293
        je      .LBB0_96
.LBB0_95:
        mov     r8d, dword ptr [rbx + r10]
        mov     r9d, r8d
        shr     r9d, 15
        add     r9d, r8d
        movzx   r8d, r9w
        mov     byte ptr [rsp + r8 + 192], 1
.LBB0_96:
        sub     rax, rdi
        lea     r8, [rcx + rdi]
        add     qword ptr [rsp + 128], -4
        lea     r9d, [r11 + r12]
        add     r9d, -6
        mov     r10, qword ptr [rsp + 24]
        lea     r14, [r10 + rdx]
        add     r14, -2
        mov     rbx, rcx
        sub     rbx, rdx
        add     rbx, -3
        xor     r13d, r13d
        mov     edx, dword ptr [rsp + 16]
        jmp     .LBB0_99
.LBB0_97:
        test    r12, r12
        jle     .LBB0_120
.LBB0_98:
        add     r13, r15
        cmp     r13, rax
        ja      .LBB0_119
.LBB0_99:
        mov     r10d, dword ptr [r8 + r13 - 4]
        mov     r11d, r10d
        shr     r11d, 15
        add     r11d, r10d
        movzx   r10d, r11w
        cmp     byte ptr [rsp + r10 + 192], 0
        mov     r15, rsi
        je      .LBB0_98
        mov     r10d, dword ptr [r8 + r13 - 9]
        mov     r11d, r10d
        shr     r11d, 15
        add     r11d, r10d
        movzx   r10d, r11w
        cmp     byte ptr [rsp + r10 + 192], 0
        mov     r15, r9
        je      .LBB0_98
        mov     r10d, dword ptr [r8 + r13 - 7]
        mov     r11d, r10d
        shr     r11d, 15
        add     r11d, r10d
        movzx   r10d, r11w
        cmp     byte ptr [rsp + r10 + 192], 0
        mov     r15, r9
        je      .LBB0_98
        mov     r15d, 1
        cmp     dword ptr [rcx + r13], edx
        jne     .LBB0_98
        mov     r12, rsi
        cmp     edi, 3
        je      .LBB0_107
        lea     r10, [rcx - 1]
        add     r10, r13
        mov     r12, rsi
.LBB0_105:
        mov     r11d, dword ptr [r14 + r12]
        cmp     r11d, dword ptr [r10 + r12]
        jne     .LBB0_107
        sub     r12, 4
        jg      .LBB0_105
.LBB0_107:
        lea     r10, [rcx + r13]
        cmp     edi, dword ptr [rsp + 32]
        je      .LBB0_97
        test    r12, r12
        jg      .LBB0_98
        mov     r11, r13
        sub     r11, qword ptr [rsp + 8]
        js      .LBB0_98
        add     r11, rcx
        mov     rdx, qword ptr [rsp + 32]
        lea     r12, [rdx + r11]
        add     r12, -3
        cmp     r12, qword ptr [rsp + 128]
        ja      .LBB0_111
        mov     r11d, dword ptr [r11]
        mov     rdx, qword ptr [rsp + 24]
        cmp     r11d, dword ptr [rdx]
        jne     .LBB0_111
        cmp     dword ptr [rsp + 32], 4
        jb      .LBB0_91
        lea     r12, [rbx + r13]
        mov     r11, qword ptr [rsp + 160]
.LBB0_115:
        mov     rdx, qword ptr [rsp + 24]
        mov     edx, dword ptr [rdx + r11 - 5]
        cmp     edx, dword ptr [r12 + r11 - 1]
        jne     .LBB0_111
        add     r11, -4
        cmp     r11, 5
        jge     .LBB0_115
        jmp     .LBB0_91
.LBB0_111:
        mov     edx, dword ptr [rsp + 16]
        jmp     .LBB0_98
.LBB0_119:
        xor     r10d, r10d
.LBB0_120:
        mov     rax, r10
        jmp     .LBB0_121
*/

// https://godbolt.org/z/dEbfhW3We
// -O3 -mavx2
// x86-64 gcc 16.1
/*
"Railgun_Tetranity_AVX2(unsigned char const*, unsigned long, unsigned char const*, unsigned long)":
        mov     rax, rdi
        test    rcx, rcx
        je      .L204
        push    rbp
        mov     rbp, rsp
        push    r15
        push    r14
        push    r13
        push    r12
        push    rbx
        mov     rbx, rsi
        and     rsp, -32
        sub     rsp, 65664
        cmp     rsi, rcx
        jb      .L3
        mov     r8, rdx
        cmp     rcx, 1
        je      .L213
        cmp     rcx, 2
        je      .L214
        cmp     rcx, 3
        je      .L215
        movzx   esi, BYTE PTR [rdx-1+rcx]
        vpxor   xmm4, xmm4, xmm4
        lea     rax, [rcx-2]
        vmovd   xmm5, esi
        vpbroadcastb    ymm5, xmm5
        cmp     rcx, 4
        je      .L13
        lea     r12, [rcx-33]
        mov     r9d, 47
        lea     r11, [rdx+r12]
        and     r11d, 31
        lea     rdx, [r11+32]
        cmp     rdx, r9
        cmovb   rdx, r9
        lea     r9, [rcx-5]
        cmp     r9, rdx
        jb      .L22
        mov     r9, rax
        test    r11, r11
        je      .L15
        sub     r9, r11
        jmp     .L17
.L216:
        cmp     r9, rax
        je      .L15
.L17:
        movzx   edx, BYTE PTR [r8+rax]
        mov     r10, rax
        sub     rax, 1
        cmp     dl, sil
        je      .L216
.L16:
        lea     r9, [r8+r10]
.L96:
        movzx   edx, BYTE PTR [r8+rax]
        cmp     dl, sil
        je      .L25
        cmp     dl, BYTE PTR [r9]
        jne     .L217
.L25:
        sub     rax, 1
        cmp     rax, 1
        jne     .L96
        movzx   r13d, BYTE PTR [r9]
        movzx   esi, BYTE PTR [r8+1]
        xor     edx, edx
        movzx   r9d, BYTE PTR [r8]
.L20:
        cmp     rcx, 32
        jbe     .L218
        lea     rax, [rdi+rbx]
        lea     r11d, [rcx-3]
        mov     r9, rdi
        mov     QWORD PTR [rsp+56], rax
        mov     eax, ecx
        mov     QWORD PTR [rsp+88], rax
        cmp     r11d, 1
        jbe     .L219
        lea     r12d, [rcx-2]
        mov     QWORD PTR [rsp+80], rbx
        mov     rsi, rax
        xor     r10d, r10d
        mov     r13d, 1
        mov     ecx, 1
        lea     rbx, [r12-1]
.L46:
        mov     rdi, r12
        cmp     rbx, rcx
        jb      .L37
        mov     r14, rsi
        mov     r15, r9
        mov     rsi, rcx
        mov     r9, rcx
.L41:
        add     rsi, 1
        lea     rax, [rdi-1]
        cmp     rax, rsi
        jb      .L155
        mov     ecx, DWORD PTR [r8-2+rsi]
        mov     rax, rsi
.L40:
        lea     rdx, [rdi-1]
        cmp     ecx, DWORD PTR [r8-1+rax]
        jne     .L39
        lea     rdx, [rax-1]
        mov     rdi, rax
.L39:
        add     rax, 1
        cmp     rdx, rax
        jnb     .L40
        cmp     rdx, rsi
        jnb     .L41
.L155:
        mov     rcx, r9
        mov     rsi, r14
        mov     r9, r15
.L37:
        mov     eax, 3
        sub     rax, rcx
        add     rax, rdi
        cmp     rax, r10
        jnb     .L42
        cmp     r10, rsi
        jnb     .L180
.L44:
        add     rcx, 1
        sub     rsi, 1
        cmp     rcx, r11
        jne     .L46
.L180:
        mov     rbx, QWORD PTR [rsp+80]
        mov     rcx, r10
        jmp     .L43
.L213:
        vpbroadcastb    ymm1, BYTE PTR [rdx]
        lea     rcx, [rsi-1]
        xor     edx, edx
.L6:
        vpcmpeqb        ymm0, ymm1, YMMWORD PTR [rdi+rdx]
        vpmovmskb       eax, ymm0
        test    eax, eax
        je      .L5
        rep bsf eax, eax
        add     rax, rdx
        cmp     rcx, rax
        jnb     .L210
.L5:
        add     rdx, 32
        cmp     rcx, rdx
        jnb     .L6
.L181:
        vzeroupper
.L3:
        xor     eax, eax
.L1:
        lea     rsp, [rbp-40]
        pop     rbx
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
        ret
.L204:
        ret
.L220:
        cmp     rax, 2
        je      .L21
.L22:
        movzx   edx, BYTE PTR [r8+rax]
        mov     r10, rax
        sub     rax, 1
        cmp     dl, sil
        je      .L220
        jmp     .L16
.L215:
        vpbroadcastb    ymm4, BYTE PTR [rdx+2]
        vpbroadcastb    ymm3, BYTE PTR [rdx+1]
        lea     rcx, [rsi-3]
        vpbroadcastb    ymm2, BYTE PTR [rdx]
        xor     edx, edx
.L12:
        vpcmpeqb        ymm1, ymm3, YMMWORD PTR [rdi+1+rdx]
        vpcmpeqb        ymm0, ymm4, YMMWORD PTR [rdi+2+rdx]
        vpand   ymm0, ymm0, ymm1
        vpcmpeqb        ymm1, ymm2, YMMWORD PTR [rdi+rdx]
        vpand   ymm0, ymm0, ymm1
        vpmovmskb       eax, ymm0
        test    eax, eax
        je      .L11
        rep bsf eax, eax
        add     rax, rdx
        cmp     rcx, rax
        jnb     .L210
.L11:
        add     rdx, 32
        cmp     rcx, rdx
        jnb     .L12
        jmp     .L181
.L42:
        cmp     rax, rsi
        jnb     .L207
        cmp     rax, 31
        ja      .L207
        mov     r10, rax
        mov     r13, rcx
        jmp     .L44
.L217:
        movzx   r11d, BYTE PTR [r9]
        movzx   r12d, BYTE PTR [r8+rax]
        lea     rdx, [rax-1]
.L26:
        movzx   r9d, BYTE PTR [r8+rdx]
        cmp     r9b, sil
        je      .L27
        mov     r13d, r11d
        cmp     r9b, r11b
        je      .L27
        cmp     r9b, r12b
        jne     .L221
.L27:
        sub     rdx, 1
        jne     .L26
        movzx   r9d, BYTE PTR [r8]
        mov     r13d, r11d
        mov     esi, r12d
        jmp     .L20
.L15:
        lea     r13, [rcx-4]
        vmovdqa ymm3, ymm5
        vmovdqa ymm1, ymm4
        mov     r10, r12
        sub     r10, r11
        sub     r13, r11
        vmovdqa ymm2, YMMWORD PTR .LC0[rip]
        add     r10, r8
        mov     r11, r13
        and     r11, -32
        mov     rdx, r10
        mov     rax, r10
        sub     rdx, r11
        jmp     .L18
.L23:
        sub     rax, 32
        cmp     rdx, rax
        je      .L222
.L18:
        vmovdqa ymm0, YMMWORD PTR [rax]
        vperm2i128      ymm0, ymm0, ymm0, 1
        vpshufb ymm0, ymm0, ymm2
        vpcmpeqb        ymm0, ymm0, ymm3
        vpcmpeqb        ymm0, ymm0, ymm1
        vptest  ymm0, ymm0
        je      .L23
        sub     r9, r10
        add     rax, r9
        jmp     .L22
.L210:
        add     rax, rdi
        vzeroupper
        lea     rsp, [rbp-40]
        pop     rbx
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
        ret
.L214:
        vpbroadcastb    ymm3, BYTE PTR [rdx+1]
        vpbroadcastb    ymm2, BYTE PTR [rdx]
        lea     rcx, [rsi-2]
        xor     edx, edx
.L9:
        vpcmpeqb        ymm0, ymm2, YMMWORD PTR [rdi+rdx]
        vpcmpeqb        ymm1, ymm3, YMMWORD PTR [rdi+1+rdx]
        vpand   ymm0, ymm0, ymm1
        vpmovmskb       eax, ymm0
        test    eax, eax
        je      .L8
        rep bsf eax, eax
        add     rax, rdx
        cmp     rcx, rax
        jnb     .L210
.L8:
        add     rdx, 32
        cmp     rcx, rdx
        jnb     .L9
        jmp     .L181
.L222:
        cmp     r11, r13
        je      .L21
        mov     rax, r9
        sub     rax, r11
        jmp     .L22
.L207:
        mov     rbx, QWORD PTR [rsp+80]
        mov     r13, rcx
        mov     r10, rax
        mov     ecx, eax
.L43:
        mov     QWORD PTR [rsp+40], r8
        mov     r14d, r10d
        lea     r12, [r8-1+r13]
        sub     rbx, rcx
        mov     QWORD PTR [rsp+48], r9
        lea     r11d, [r10-3]
        cmp     r10d, 19
        jbe     .L223
        mov     eax, DWORD PTR [r12]
        mov     QWORD PTR [rsp+80], rcx
        mov     edx, 65536
        xor     esi, esi
        mov     DWORD PTR [rsp+72], r11d
        lea     rdi, [rsp+128]
        mov     DWORD PTR [rsp+64], eax
        vzeroupper
        call    "memset"
        mov     eax, DWORD PTR [rsp+72]
        mov     rdx, r12
        mov     rcx, QWORD PTR [rsp+80]
        mov     r9, QWORD PTR [rsp+48]
        vpxor   xmm4, xmm4, xmm4
        mov     r8, QWORD PTR [rsp+40]
        mov     r15, rax
        lea     rdi, [r12+rax]
.L72:
        mov     esi, DWORD PTR [rdx]
        add     rdx, 1
        mov     eax, esi
        shr     eax, 15
        add     eax, esi
        movzx   eax, ax
        mov     BYTE PTR [rsp+128+rax], 1
        cmp     rdi, rdx
        jne     .L72
        lea     eax, [r14-9]
        mov     QWORD PTR [rsp+72], r8
        xor     edx, edx
        mov     r10, r15
        mov     QWORD PTR [rsp+80], rax
        jmp     .L94
.L225:
        mov     edi, DWORD PTR [r9-9+rsi]
        mov     eax, edi
        shr     eax, 15
        add     eax, edi
        movzx   eax, ax
        cmp     BYTE PTR [rsp+128+rax], 0
        je      .L74
        mov     esi, DWORD PTR [r9-7+rsi]
        mov     eax, esi
        shr     eax, 15
        add     eax, esi
        movzx   eax, ax
        cmp     BYTE PTR [rsp+128+rax], 0
        je      .L74
        lea     rax, [r9+rdx]
        mov     esi, DWORD PTR [rsp+64]
        cmp     DWORD PTR [rax], esi
        je      .L224
.L76:
        mov     eax, 1
.L73:
        add     rdx, rax
        cmp     rbx, rdx
        jb      .L181
.L94:
        lea     rsi, [rdx+rcx]
        mov     edi, DWORD PTR [r9-4+rsi]
        mov     eax, edi
        shr     eax, 15
        add     eax, edi
        movzx   eax, ax
        cmp     BYTE PTR [rsp+128+rax], 0
        jne     .L225
        mov     rax, r10
        jmp     .L73
.L74:
        mov     rax, QWORD PTR [rsp+80]
        jmp     .L73
.L223:
        mov     eax, DWORD PTR [r12]
        mov     QWORD PTR [rsp+80], rcx
        xor     esi, esi
        mov     edx, 65536
        mov     QWORD PTR [rsp+32], r10
        lea     rdi, [rsp+128]
        mov     DWORD PTR [rsp+64], r11d
        mov     DWORD PTR [rsp+72], eax
        vzeroupper
        call    "memset"
        mov     r10, QWORD PTR [rsp+32]
        mov     rcx, QWORD PTR [rsp+80]
        mov     r9, QWORD PTR [rsp+48]
        mov     r8, QWORD PTR [rsp+40]
        vpxor   xmm4, xmm4, xmm4
        cmp     r10d, 1
        je      .L103
        mov     eax, DWORD PTR [rsp+64]
        lea     edi, [r10-1]
        mov     QWORD PTR [rsp+80], rax
        mov     r11, rax
.L36:
        mov     rax, r12
        lea     rsi, [r12+rdi]
.L49:
        movzx   edx, WORD PTR [rax]
        add     rax, 1
        mov     BYTE PTR [rsp+128+rdx], 1
        cmp     rax, rsi
        jne     .L49
.L48:
        mov     eax, r11d
        mov     QWORD PTR [rsp+16], r8
        xor     edx, edx
        mov     QWORD PTR [rsp+64], rax
        jmp     .L71
.L226:
        movzx   eax, WORD PTR [r9-4+rax]
        cmp     BYTE PTR [rsp+128+rax], 0
        jne     .L51
        mov     rax, QWORD PTR [rsp+64]
.L50:
        add     rdx, rax
        cmp     rbx, rdx
        jb      .L181
.L71:
        lea     rax, [rcx+rdx]
        movzx   esi, WORD PTR [r9-2+rax]
        cmp     BYTE PTR [rsp+128+rsi], 0
        jne     .L226
        mov     rax, rdi
        jmp     .L50
.L51:
        lea     rax, [r9+rdx]
        mov     esi, DWORD PTR [rsp+72]
        cmp     DWORD PTR [rax], esi
        je      .L227
.L52:
        mov     eax, 1
        jmp     .L50
.L227:
        cmp     QWORD PTR [rsp+80], 0
        je      .L56
        mov     r15, QWORD PTR [rsp+80]
        lea     rsi, [r15-1]
        cmp     rsi, 27
        jbe     .L60
        lea     r11, [rdx-29+r15]
        lea     r8, [r12-29+r15]
        add     r11, r9
        mov     r10, r8
        or      r10, r11
        and     r10d, 31
        jne     .L60
        vmovdqa ymm1, YMMWORD PTR .LC1[rip]
        xor     r10d, r10d
        xor     esi, esi
        vmovdqa ymm3, ymm4
        jmp     .L55
.L61:
        add     r10, 8
        sub     rsi, 32
        cmp     r10, 1073741824
        je      .L56
.L55:
        vpermd  ymm0, ymm1, YMMWORD PTR [r11+rsi]
        vpermd  ymm2, ymm1, YMMWORD PTR [r8+rsi]
        vpcmpeqd        ymm0, ymm0, ymm2
        vpcmpeqd        ymm0, ymm0, ymm3
        vptest  ymm0, ymm0
        je      .L61
        mov     rsi, QWORD PTR [rsp+80]
        neg     r10
        lea     rsi, [rsi+r10*4]
        lea     r10, [r9+rsi]
        mov     r14d, DWORD PTR [rdx-1+r10]
        cmp     DWORD PTR [r12-1+rsi], r14d
        jne     .L52
        lea     r10, [r9-1+rdx]
        add     r10, rsi
        mov     r14d, DWORD PTR [r10-4]
        cmp     DWORD PTR [r12-5+rsi], r14d
        jne     .L52
        mov     r14d, DWORD PTR [r10-8]
        cmp     DWORD PTR [r12-9+rsi], r14d
        jne     .L52
        mov     r15d, DWORD PTR [r12-13+rsi]
        cmp     DWORD PTR [r10-12], r15d
        jne     .L52
        mov     r14d, DWORD PTR [r10-16]
        cmp     DWORD PTR [r12-17+rsi], r14d
        jne     .L52
        mov     r15d, DWORD PTR [r12-21+rsi]
        cmp     DWORD PTR [r10-20], r15d
        jne     .L52
        mov     r14d, DWORD PTR [r12-25+rsi]
        cmp     DWORD PTR [r10-24], r14d
        jne     .L52
        mov     esi, DWORD PTR [r12-29+rsi]
        cmp     DWORD PTR [r10-28], esi
        jne     .L52
.L56:
        cmp     rcx, QWORD PTR [rsp+88]
        je      .L202
        mov     r10, rdx
        sub     r10, r13
        mov     rax, r10
        add     rax, 1
        js      .L52
        mov     rsi, QWORD PTR [rsp+88]
        mov     r15, QWORD PTR [rsp+56]
        lea     rsi, [r10-2+rsi]
        lea     r11, [r15-4]
        add     rsi, r9
        cmp     r11, rsi
        jb      .L52
        mov     rsi, QWORD PTR [rsp+16]
        add     rax, r9
        mov     esi, DWORD PTR [rsi]
        cmp     DWORD PTR [rax], esi
        jne     .L52
        mov     r14, QWORD PTR [rsp+88]
        lea     rsi, [r14-3]
        test    rsi, rsi
        jle     .L202
        lea     r11, [r14-4]
        cmp     r11, 27
        jbe     .L64
        lea     r15, [r14-31]
        lea     r14, [r10+r15]
        sub     r15, r13
        add     r14, r9
        add     r15, r12
        mov     QWORD PTR [rsp+24], r14
        or      r14, r15
        and     r14b, 31
        mov     QWORD PTR [rsp+48], r15
        jne     .L64
        shr     r11, 2
        vmovdqa ymm1, YMMWORD PTR .LC1[rip]
        xor     r8d, r8d
        vmovdqa ymm2, ymm4
        lea     r14, [r11+1]
        xor     r11d, r11d
        mov     QWORD PTR [rsp+32], r14
        shr     r14, 3
        lea     r15, [0+r14*8]
        mov     QWORD PTR [rsp+40], r15
        jmp     .L66
.L69:
        sub     r8, 32
        add     r11, 8
        cmp     r11, QWORD PTR [rsp+40]
        je      .L228
.L66:
        mov     r14, QWORD PTR [rsp+24]
        mov     r15, QWORD PTR [rsp+48]
        vpermd  ymm0, ymm1, YMMWORD PTR [r14+r8]
        vpermd  ymm3, ymm1, YMMWORD PTR [r15+r8]
        vpcmpeqd        ymm0, ymm0, ymm3
        vpcmpeqd        ymm0, ymm0, ymm2
        vptest  ymm0, ymm0
        je      .L69
        jmp     .L208
.L221:
        mov     esi, r12d
        jmp     .L20
.L218:
        vmovd   xmm2, esi
        vmovd   xmm1, r13d
        vmovd   xmm0, r9d
        sub     rbx, rcx
        mov     rsi, rbx
        mov     ebx, ecx
        vpbroadcastb    ymm1, xmm1
        vmovdqa YMMWORD PTR [rsp+96], ymm4
        vpbroadcastb    ymm2, xmm2
        vpbroadcastb    ymm0, xmm0
        and     ebx, 127
        je      .L30
        xor     r9d, r9d
.L29:
        mov     r11d, r9d
        add     r9d, 1
        movzx   r12d, BYTE PTR [r8+r11]
        mov     BYTE PTR [rsp+96+r11], r12b
        cmp     r9d, ebx
        jb      .L29
.L30:
        vmovdqa ymm4, YMMWORD PTR [rsp+96]
        mov     r9d, -1
        cmp     rcx, 32
        je      .L31
.L19:
        mov     r8d, 1
        sal     r8d, cl
        lea     r9d, [r8-1]
.L31:
        xor     r8d, r8d
        lea     r11, [rdi+rcx]
        lea     rbx, [rdi+rax]
        add     r10, rdi
        lea     r12, [rdi+rdx]
.L34:
        vpcmpeqb        ymm6, ymm1, YMMWORD PTR [r10+r8]
        vpcmpeqb        ymm3, ymm5, YMMWORD PTR [r8-1+r11]
        vpcmpeqb        ymm7, ymm0, YMMWORD PTR [r12+r8]
        vpand   ymm3, ymm3, ymm6
        vpcmpeqb        ymm6, ymm2, YMMWORD PTR [rbx+r8]
        vpand   ymm6, ymm6, ymm7
        vpand   ymm3, ymm3, ymm6
        vpmovmskb       edx, ymm3
        test    edx, edx
        jne     .L33
        jmp     .L32
.L229:
        add     rax, rdi
        vpcmpeqb        ymm3, ymm4, YMMWORD PTR [rax]
        vpmovmskb       ecx, ymm3
        not     ecx
        test    ecx, r9d
        je      .L202
        lea     eax, [rdx-1]
        and     edx, eax
        je      .L32
.L33:
        xor     eax, eax
        rep bsf eax, edx
        add     rax, r8
        cmp     rsi, rax
        jnb     .L229
        jmp     .L181
.L228:
        cmp     r11, QWORD PTR [rsp+32]
        je      .L202
.L208:
        imul    r11, r11, -4
        add     rsi, r11
        lea     r11, [r9+r10]
        mov     r14, rsi
        sub     r14, r13
        mov     r15d, DWORD PTR [r12+r14]
        cmp     DWORD PTR [r11+rsi], r15d
        jne     .L52
        lea     r15, [rsi-4]
        test    r15, r15
        jle     .L202
        mov     r14, r15
        sub     r14, r13
        mov     r14d, DWORD PTR [r12+r14]
        cmp     DWORD PTR [rsi-4+r11], r14d
        jne     .L52
        lea     r15, [rsi-8]
        test    r15, r15
        jle     .L202
        mov     r14, r15
        lea     r8, [r15+r9]
        sub     r14, r13
        mov     r15, r14
        mov     r14d, DWORD PTR [r8+r10]
        cmp     DWORD PTR [r12+r15], r14d
        jne     .L52
        lea     r14, [rsi-12]
        mov     r15, r14
        test    r14, r14
        jle     .L202
        sub     r14, r13
        mov     r15d, DWORD PTR [r11+r15]
        cmp     DWORD PTR [r12+r14], r15d
        jne     .L52
        lea     r15, [rsi-16]
        test    r15, r15
        jle     .L202
        lea     r14, [r9+r15]
        sub     r15, r13
        mov     r15d, DWORD PTR [r12+r15]
        cmp     DWORD PTR [r14+r10], r15d
        jne     .L52
        lea     r10, [rsi-20]
        test    r10, r10
        jle     .L202
        mov     r14, r10
        sub     r14, r13
        mov     r15d, DWORD PTR [r12+r14]
        cmp     DWORD PTR [rsi-20+r11], r15d
        jne     .L52
        lea     r10, [rsi-24]
        test    r10, r10
        jle     .L202
        mov     r15, r10
        mov     r14d, DWORD PTR [rsi-24+r11]
        sub     r15, r13
        cmp     DWORD PTR [r12+r15], r14d
        jne     .L52
        sub     rsi, 28
        test    rsi, rsi
        jle     .L202
        mov     r10, rsi
        sub     r10, r13
        mov     r15d, DWORD PTR [r12+r10]
        cmp     DWORD PTR [r11+rsi], r15d
        jne     .L52
.L202:
        vzeroupper
        jmp     .L1
.L224:
        lea     rsi, [r10-1]
        cmp     rsi, 27
        jbe     .L106
        mov     r14, QWORD PTR [rsp+72]
        lea     rdi, [r13-30+r10]
        lea     r11, [rdx-29+r10]
        add     r11, r9
        add     rdi, r14
        mov     r14, rdi
        or      rdi, r11
        and     edi, 31
        jne     .L106
        shr     rsi, 2
        vmovdqa ymm1, YMMWORD PTR .LC1[rip]
        xor     edi, edi
        vmovdqa ymm3, ymm4
        add     rsi, 1
        mov     QWORD PTR [rsp+48], rsi
        shr     rsi, 3
        lea     r8, [0+rsi*8]
        xor     esi, esi
.L79:
        vpermd  ymm0, ymm1, YMMWORD PTR [r11+rdi]
        vpermd  ymm2, ymm1, YMMWORD PTR [r14+rdi]
        vpcmpeqd        ymm0, ymm0, ymm2
        vpcmpeqd        ymm0, ymm0, ymm3
        vptest  ymm0, ymm0
        jne     .L230
        add     rsi, 8
        sub     rdi, 32
        cmp     r8, rsi
        jne     .L79
        cmp     QWORD PTR [rsp+48], r8
        je      .L82
        neg     r8
        lea     rsi, [r10+r8*4]
.L85:
        lea     rdi, [r9-1+rdx]
        mov     r15d, DWORD PTR [r12-1+rsi]
        cmp     DWORD PTR [rdi+rsi], r15d
        jne     .L76
        lea     r8, [rsi-4]
        test    r8, r8
        jle     .L82
        mov     r14d, DWORD PTR [r12-5+rsi]
        cmp     DWORD PTR [rsi-4+rdi], r14d
        jne     .L76
        lea     r8, [rsi-8]
        test    r8, r8
        jle     .L82
        mov     r15d, DWORD PTR [r12-9+rsi]
        cmp     DWORD PTR [rsi-8+rdi], r15d
        jne     .L76
        lea     r8, [rsi-12]
        test    r8, r8
        jle     .L82
        mov     r14d, DWORD PTR [r12-13+rsi]
        cmp     DWORD PTR [rsi-12+rdi], r14d
        jne     .L76
        lea     r8, [rsi-16]
        test    r8, r8
        jle     .L82
        mov     r15d, DWORD PTR [r12-17+rsi]
        cmp     DWORD PTR [rsi-16+rdi], r15d
        jne     .L76
        lea     r8, [rsi-20]
        test    r8, r8
        jle     .L82
        mov     r14d, DWORD PTR [r12-21+rsi]
        cmp     DWORD PTR [rsi-20+rdi], r14d
        jne     .L76
        lea     rdi, [rsi-24]
        test    rdi, rdi
        jle     .L82
        add     rdi, r9
        mov     r15d, DWORD PTR [r12-25+rsi]
        cmp     DWORD PTR [rdx-1+rdi], r15d
        jne     .L76
        lea     rdi, [rsi-28]
        test    rdi, rdi
        jle     .L82
        add     rdi, r9
        mov     esi, DWORD PTR [r12-29+rsi]
        cmp     DWORD PTR [rdx-1+rdi], esi
        jne     .L76
.L82:
        cmp     rcx, QWORD PTR [rsp+88]
        je      .L202
        mov     rdi, rdx
        sub     rdi, r13
        mov     rax, rdi
        add     rax, 1
        js      .L76
        mov     rsi, QWORD PTR [rsp+88]
        mov     r14, QWORD PTR [rsp+56]
        lea     rsi, [rdi-2+rsi]
        lea     r8, [r14-4]
        add     rsi, r9
        cmp     r8, rsi
        jb      .L76
        mov     rsi, QWORD PTR [rsp+72]
        add     rax, r9
        mov     esi, DWORD PTR [rsi]
        cmp     DWORD PTR [rax], esi
        jne     .L76
        mov     r14, QWORD PTR [rsp+88]
        lea     r11, [r14-3]
        test    r11, r11
        jle     .L202
        lea     r8, [r14-4]
        cmp     r8, 27
        jbe     .L87
        mov     r15, QWORD PTR [rsp+72]
        lea     rsi, [rdi-31+r14]
        add     rsi, r9
        lea     r15, [r15-32+r14]
        mov     QWORD PTR [rsp+24], rsi
        or      rsi, r15
        mov     QWORD PTR [rsp+32], r15
        and     sil, 31
        jne     .L87
        shr     r8, 2
        vmovdqa ymm1, YMMWORD PTR .LC1[rip]
        vmovdqa ymm2, ymm4
        lea     rsi, [r8+1]
        xor     r8d, r8d
        mov     QWORD PTR [rsp+40], rsi
        shr     rsi, 3
        sal     rsi, 3
        mov     QWORD PTR [rsp+48], rsi
        xor     esi, esi
        jmp     .L89
.L92:
        sub     r8, 32
        add     rsi, 8
        cmp     rsi, QWORD PTR [rsp+48]
        je      .L231
.L89:
        mov     r15, QWORD PTR [rsp+24]
        mov     r14, QWORD PTR [rsp+32]
        vpermd  ymm0, ymm1, YMMWORD PTR [r15+r8]
        vpermd  ymm3, ymm1, YMMWORD PTR [r14+r8]
        vpcmpeqd        ymm0, ymm0, ymm3
        vpcmpeqd        ymm0, ymm0, ymm2
        vptest  ymm0, ymm0
        je      .L92
        jmp     .L209
.L32:
        add     r8, 32
        cmp     rsi, r8
        jnb     .L34
        jmp     .L181
.L230:
        neg     rsi
        lea     rsi, [r10+rsi*4]
        jmp     .L85
.L106:
        mov     rsi, r10
.L83:
        mov     edi, DWORD PTR [r12-1+rsi]
        cmp     DWORD PTR [rax-1+rsi], edi
        jne     .L76
        sub     rsi, 4
        test    rsi, rsi
        jg      .L83
        jmp     .L82
.L13:
        mov     eax, DWORD PTR [rdx]
        vmovdqa YMMWORD PTR [rsp+96], ymm4
        vpbroadcastb    ymm0, BYTE PTR [rdx]
        lea     rsi, [rbx-4]
        vpbroadcastb    ymm1, BYTE PTR [rdx+2]
        vpbroadcastb    ymm2, BYTE PTR [rdx+1]
        xor     edx, edx
        mov     r10d, 2
        mov     DWORD PTR [rsp+96], eax
        mov     eax, 1
        vmovdqa ymm4, YMMWORD PTR [rsp+96]
        jmp     .L19
.L60:
        mov     r15, QWORD PTR [rsp+80]
        and     rsi, -4
        lea     r10, [r15-4]
        sub     r10, rsi
        mov     rsi, r15
        jmp     .L57
.L232:
        sub     rsi, 4
        cmp     rsi, r10
        je      .L56
.L57:
        mov     r15d, DWORD PTR [r12-1+rsi]
        cmp     DWORD PTR [rax-1+rsi], r15d
        je      .L232
        jmp     .L52
.L103:
        mov     eax, 4294967294
        xor     edi, edi
        mov     r11d, -2
        mov     QWORD PTR [rsp+80], rax
        jmp     .L48
.L219:
        mov     eax, DWORD PTR [r8]
        mov     QWORD PTR [rsp+80], r8
        xor     esi, esi
        mov     edx, 65536
        mov     QWORD PTR [rsp+64], rdi
        lea     rdi, [rsp+128]
        mov     r13d, 1
        mov     DWORD PTR [rsp+72], eax
        vzeroupper
        call    "memset"
        mov     r8, QWORD PTR [rsp+80]
        xor     ecx, ecx
        mov     eax, 4294967293
        mov     r9, QWORD PTR [rsp+64]
        mov     edi, 4294967295
        mov     r11d, -3
        mov     QWORD PTR [rsp+80], rax
        mov     r12, r8
        vpxor   xmm4, xmm4, xmm4
        jmp     .L36
.L21:
        movzx   r13d, BYTE PTR [r8+2]
        movzx   r9d, BYTE PTR [r8]
        mov     eax, 1
        xor     edx, edx
        movzx   esi, BYTE PTR [r8+1]
        mov     r10d, 2
        jmp     .L20
.L64:
        mov     r15, QWORD PTR [rsp+88]
        and     r11, -4
        lea     r14, [r15-7]
        sub     r14, r11
        lea     r11, [r9+r10]
        mov     r10, r12
        sub     r10, r13
.L68:
        mov     r15d, DWORD PTR [r10+rsi]
        cmp     DWORD PTR [r11+rsi], r15d
        jne     .L52
        sub     rsi, 4
        cmp     r14, rsi
        jne     .L68
        jmp     .L202
.L231:
        cmp     rsi, QWORD PTR [rsp+40]
        je      .L202
.L209:
        imul    rsi, rsi, -4
        add     rdi, r9
        add     rsi, r11
        mov     r8, rsi
        sub     r8, r13
        mov     r14d, DWORD PTR [r12+r8]
        cmp     DWORD PTR [rdi+rsi], r14d
        jne     .L76
        lea     r8, [rsi-4]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r15d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-4+rdi], r15d
        jne     .L76
        lea     r8, [rsi-8]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r14d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-8+rdi], r14d
        jne     .L76
        lea     r8, [rsi-12]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r15d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-12+rdi], r15d
        jne     .L76
        lea     r8, [rsi-16]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r14d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-16+rdi], r14d
        jne     .L76
        lea     r8, [rsi-20]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r15d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-20+rdi], r15d
        jne     .L76
        lea     r8, [rsi-24]
        test    r8, r8
        jle     .L202
        mov     r11, r8
        sub     r11, r13
        mov     r14d, DWORD PTR [r12+r11]
        cmp     DWORD PTR [rsi-24+rdi], r14d
        jne     .L76
        sub     rsi, 28
        test    rsi, rsi
        jle     .L202
        mov     r8, rsi
        sub     r8, r13
        mov     r15d, DWORD PTR [r12+r8]
        cmp     DWORD PTR [rdi+rsi], r15d
        je      .L202
        jmp     .L76
.L87:
        mov     rsi, QWORD PTR [rsp+72]
        add     rdi, r9
        sub     rsi, 1
.L91:
        mov     r14d, DWORD PTR [rsi+r11]
        cmp     DWORD PTR [rdi+r11], r14d
        jne     .L76
        sub     r11, 4
        test    r11, r11
        jg      .L91
        jmp     .L202
.LC0:
        .byte   15
        .byte   14
        .byte   13
        .byte   12
        .byte   11
        .byte   10
        .byte   9
        .byte   8
        .byte   7
        .byte   6
        .byte   5
        .byte   4
        .byte   3
        .byte   2
        .byte   1
        .byte   0
        .byte   15
        .byte   14
        .byte   13
        .byte   12
        .byte   11
        .byte   10
        .byte   9
        .byte   8
        .byte   7
        .byte   6
        .byte   5
        .byte   4
        .byte   3
        .byte   2
        .byte   1
        .byte   0
.LC1:
        .long   7
        .long   6
        .long   5
        .long   4
        .long   3
        .long   2
        .long   1
        .long   0
*/
