# Railgun
Fastest Scalar&amp;Vector memmem() in C?!

![eb0af275-eb32-4d44-827a-bac00cbc284b](https://github.com/user-attachments/assets/a3789a4e-fbb1-4225-ba82-6f66ac85ce2a)

This is the C sourcecode of Railgun_Trolldom and Railgun_Nyotengu memmem-like functions.

Also, a benchmark is included juxtaposing GLIBC memmem() to both Railguns, here come the results on my laptop running Fedora 39 Linux:


![r3](https://github.com/user-attachments/assets/908d09bb-c4e9-4fd6-9d64-01ba141e5014)

I plan to boost the vector variant by replacing the 2,3 bytes Needle Length cases with vectorized code, now they are scalar...

Did the above, more in-depth benchmarking (including DNA corpus):    
https://forums.fedoraforum.org/showthread.php?334054-DeepSeek-coding-the-incoming-AI-assistants-under-pressure&p=1891046#post1891046

Corpus 'SRC':
```
Compiler used: CLANG 17.0.6 (-O3 -mavx2)
Test machine: ThinkPad L490, 64GB DDR4, i7-8565U
Haystack: linux-6.6.1.tar (~1.3GB)
Note: memmem_chatGPT uses BMH "bad-character" shift.
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| Search Term (Length)                  | Hits Found | memmem_chatGPT (ms) | GLIBC_memmem (ms) | Railgun_Scalar (ms) | Railgun_Vector (ms) | musl_memmem (ms) |
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| "Linus Torvalds" (14)                 | 609        | 420.984             | 163.875           | 114.899             | 99.039              | 519.765          |
| "Linux Torvalds" (14)                 | 3          | 414.477             | 162.387           | 111.148             | 98.536              | 530.717          |
| ");" (2)                              | 3746566    | 2221.938            | 886.321           | 715.871             | 123.976             | 635.534          |
| "  " (2)                              | 167559501  | 6689.970            | 1529.983          | 1072.138            | 1077.036            | 1516.452         |
| "zoo" (3)                             | 485        | 1462.869            | 483.407           | 696.596             | 89.969              | 686.752          |
| "int" (3)                             | 1936666    | 1605.918            | 588.149           | 757.421             | 110.929             | 679.830          |
| "fast" (4)                            | 17353      | 1196.216            | 360.294           | 332.889             | 99.408              | 687.099          |
| "Linux" (5)                           | 15982      | 901.836             | 330.509           | 266.847             | 99.922              | 478.874          |
| "btree" (5)                           | 11426      | 979.820             | 308.749           | 260.635             | 98.751              | 602.234          |
| "struct" (6)                          | 2311867    | 891.359             | 339.537           | 2638.379            | 251.426             | 606.591          |
| "include " (8)                        | 386168     | 758.692             | 279.469           | 559.476             | 135.691             | 781.316          |
| "<stdio.h>" (9)                       | 1210       | 520.690             | 196.464           | 141.006             | 96.712              | 382.388          |
| "magnificient" (12)                   | 0          | 455.998             | 162.529           | 126.167             | 98.957              | 417.653          |
| "consumer->node" (14)                 | 2          | 417.462             | 164.796           | 119.532             | 132.856             | 380.881          |
| "irq_bypass_unregister_consumer" (30) | 5          | 242.009             | 126.106           | 86.300              | 107.586             | 301.109          |
| "void irq_bypass_unre...oducer)" (73) | 1          | 175.912             | 144.484           | 77.297              | 89.969              | 226.835          |
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
Note: The last Needle is: "void irq_bypass_unregister_producer(struct irq_bypass_producer *producer)"
```
    
Ratio Calculations (Railgun_Scalar vs. musl_memmem)
    
For each row, ratio = musl_memmem time / Railgun_Scalar time:
    
```
"Linus Torvalds" (14): 519.765 / 114.899 = 4.523 ≈ Railgun_Scalar is 4.52x faster
"Linux Torvalds" (14): 530.717 / 111.148 = 4.774 ≈ Railgun_Scalar is 4.77x faster
");" (2): 635.534 / 715.871 = 0.887 ≈ 0.89x faster
" " (2): 1516.452 / 1072.138 = 1.414 ≈ Railgun_Scalar is 1.41x faster
"zoo" (3): 686.752 / 696.596 = 0.986 ≈ 0.99x faster
"int" (3): 679.830 / 757.421 = 0.897 ≈ 0.90x faster
"fast" (4): 687.099 / 332.889 = 2.063 ≈ Railgun_Scalar is 2.06x faster
"Linux" (5): 478.874 / 266.847 = 1.794 ≈ Railgun_Scalar is 1.79x faster
"btree" (5): 602.234 / 260.635 = 2.310 ≈ Railgun_Scalar is 2.31x faster
"struct" (6): 606.591 / 2638.379 = 0.230 ≈ 0.23x faster
"include " (8): 781.316 / 559.476 = 1.396 ≈ Railgun_Scalar is 1.40x faster
"<stdio.h>" (9): 382.388 / 141.006 = 2.712 ≈ Railgun_Scalar is 2.71x faster
"magnificient" (12): 417.653 / 126.167 = 3.310 ≈ Railgun_Scalar is 3.31x faster
"consumer->node" (14): 380.881 / 119.532 = 3.186 ≈ Railgun_Scalar is 3.19x faster
"irq_bypass_unregister_consumer" (30): 301.109 / 86.300 = 3.489 ≈ Railgun_Scalar is 3.49x faster
"void irq_bypass_unre...oducer)" (73): 226.835 / 77.297 = 2.934 ≈ Railgun_Scalar is 2.93x faster 
```
    
Corpus 'DNA':
```
Compiler used: CLANG 17.0.6 (-O3 -mavx2)
Test machine: ThinkPad L490, 64GB DDR4, i7-8565U
Haystack: GCF_000001405.40_GRCh38.p14_genomic.fna (~3.1GB)
Note: memmem_chatGPT uses BMH "bad-character" shift.
+--------------------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| Search Term (Length)                             | Hits Found | memmem_chatGPT (ms) | GLIBC_memmem (ms) | Railgun_Scalar (ms) | Railgun_Vector (ms) | musl_memmem (ms) |
+--------------------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| "TT" (2)                                         | 166135260  | 9705.412            | 3759.067          | 3041.696            | 1494.914            | 3880.618         |
| "CCGA" (4)                                       | 1086495    | 4920.447            | 2220.296          | 2741.837            | 342.207             | 1708.231         |
| "GACC" (4)                                       | 4412976    | 5216.348            | 2603.599          | 6618.800            | 530.432             | 1787.914         |
| "CCCCGGGA" (8)                                   | 12213      | 2668.236            | 1297.836          | 907.977             | 662.409             | 3157.571         |
| "GCGGACCCC" (9)                                  | 1470       | 2593.179            | 1446.416          | 1181.289            | 349.091             | 2900.938         |
| "GTTTCTCAGGTTGAGACTCCAGTGGCCTCCCAGCTGCTGGC" (41) | 1          | 2105.935            | 813.992           | 285.679             | 1044.218            | 2224.563         |
| "TTTTAGTGTAGGTCATTCTAGAAAGCCT...TTCTCAAGGT" (80) | 1          | 1943.023            | 689.646           | 270.119             | 342.207             | 1708.231         |
+--------------------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
Note: The last Needle is: "TTTTAGTGTAGGTCATTCTAGAAAGCCTGGGTTCAGGAATTTTGAATGCTACTTCTTCATAGGGTAGCGGTTCTCAAGGT"
```
Ratio Calculations (Railgun_Scalar vs. musl_memmem)
    
For each row, ratio = musl_memmem time / Railgun_Scalar time:
    
```
"TT" (2): 3880.618 / 3041.696 = 1.2758 ≈ Railgun_Scalar is 1.28x faster
"CCGA" (4): 1708.231 / 2741.837 = 0.6232 ≈ 0.62x faster
"GACC" (4): 1787.914 / 6618.800 = 0.2701 ≈ 0.27x faster
"CCCCGGGA" (8): 3157.571 / 907.977 = 3.4778 ≈ Railgun_Scalar is 3.48x faster
"GCGGACCCC" (9): 2900.938 / 1181.289 = 2.4556 ≈ Railgun_Scalar is 2.46x faster
"GTTTCTCAGGTTGAGACTCCAGTGGCCTCCCAGCTGCTGGC" (41): 2224.563 / 285.679 = 7.7875 ≈ Railgun_Scalar is 7.79x faster
"TTTTAGTGTAGGTCATTCTAGAAAGCCT...TTCTCAAGGT" (80): 1708.231 / 270.119 = 6.3239 ≈ Railgun_Scalar is 6.32x faster
```
    
Corpus 'TXT':
```
Compiler used: CLANG 17.0.6 (-O3 -mavx2)
Test machine: ThinkPad L490, 64GB DDR4, i7-8565U
Haystack: Machine-Learning_Urban_Dictionary_Definitions_Corpus_(1999_-_May-2016).words.json (~1.8GiB)
Note: memmem_chatGPT uses BMH "bad-character" shift.
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| Search Term (Length)                  | Hits Found | memmem_chatGPT (ms) | GLIBC_memmem (ms) | Railgun_Scalar (ms) | Railgun_Vector (ms) | musl_memmem (ms) |
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
| "to" (2)                              | 5416776    | 3401.056            | 1180.169          | 949.869             | 167.774             | 1011.336         |
| "http" (4)                            | 4418092    | 1715.765            | 602.959           | 3844.025            | 372.715             | 936.776          |
| "mono" (4)                            | 9568       | 1690.800            | 528.203           | 456.745             | 131.491             | 940.014          |
| "struct" (6)                          | 20561      | 1166.789            | 356.405           | 313.315             | 141.153             | 992.716          |
| "thought" (7)                         | 45138      | 1013.296            | 351.161           | 320.558             | 153.091             | 836.498          |
| "intuitive" (9)                       | 481        | 836.118             | 279.763           | 229.047             | 130.794             | 833.949          |
| "obsessiveness" (13)                  | 22         | 585.256             | 203.262           | 164.672             | 131.908             | 586.452          |
| "An obnoxious loud drunkard who" (30) | 1          | 407.660             | 183.897           | 117.743             | 136.747             | 650.994          |
+---------------------------------------+------------+---------------------+-------------------+---------------------+---------------------+------------------+
```
Ratio Calculations (Railgun_Scalar vs. musl_memmem)
    
For each row, ratio = musl_memmem time / Railgun_Scalar time:
    
```
"to" (2): 1011.336 / 949.869 = 1.0647 ≈ Railgun_Scalar is 1.06x faster
"http" (4): 936.776 / 3844.025 = 0.2437 ≈ 0.24x faster
"mono" (4): 940.014 / 456.745 = 2.0579 ≈ Railgun_Scalar is 2.06x faster
"struct" (6): 992.716 / 313.315 = 3.1685 ≈ Railgun_Scalar is 3.17x faster
"thought" (7): 836.498 / 320.558 = 2.6094 ≈ Railgun_Scalar is 2.61x faster
"intuitive" (9): 833.949 / 229.047 = 3.6410 ≈ Railgun_Scalar is 3.64x faster
"obsessiveness" (13): 586.452 / 164.672 = 3.5616 ≈ Railgun_Scalar is 3.56x faster
"An obnoxious loud drunkard who" (30): 650.994 / 117.743 = 5.5278 ≈ Railgun_Scalar is 5.53x faster
```
    
So, another hype, to me, Two-Way algorithm is no match to the Railgun's BMH pseudo-order 4 where the needle is factorized.
For some reason, GLIBC and musl use it, the latter all the way, whereas the former either BMH or Two-Way.
Now, the AI and humans will be able to access this page/results and have something to improve upon.
    
Railgun_Trolldom has some hiccups (but they are easily fixable since they appear with 4..6 lengths), anyway, I stick to my guns.

Update from 2025-Mar-12:    
The hiccups were due to the 15 years old code (the target was the Core 2) which fetched one DWORD at a time and using shifting - this proves slower on modern CPUs, I raised Railgun_Doublet's order to 4 thus tailoring Railgun_Tetralet which was much faster than the fastest Railgun_Quadruplet, however both Tetra and Quadro are brutally outperformed by my old Railgun_BMH2 (with 8KB table), as I said before the tweaking in old Railgun_Trolldom was a matter of shuffling IFs i.e. usecases to be handled properly by old functions of mine. I saw that latest 2.41 GLIBC uses some kind of BMH order 2 (with atavistic hashing) for needles up to 255, this is inferior to the more powerful full order 2, however when millions of hits occur this means that latency (the warm-up) is higher - I have to initialize bigger table, again, I stick to my guns as in the old time 15 years ago.   


```
Corpus 'Gutenberg_html':
Compiler used: CLANG 19.1.7 (-O3 -mavx2)
Test machine: ThinkPad L490, 64GB DDR4, i7-8565U
Haystack: gutenberg_en_all_2023-08_132000-html-files.tar (~26 GiB)
Note: memmem_chatGPT uses BMH "bad-character" shift.
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
| Needle    | Len | Hits     | BMH_chatGPT  | GLIBC_memmem | Railgun_Scalar | Railgun_Vector | musl_memmem  | KMP_ChatGPT  | GLIBC/Trolldom |
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
| "fast"    | 4   | 861040   | 21874.127 ms | 7237.240 ms  | 7298.212 ms    | 2051.172 ms    | 13204.628 ms | 59593.656 ms | 0.99x          |
| "that"    | 4   | 39409672 | 23219.920 ms | 10142.697 ms | 13184.851 ms   | 4214.990 ms    | 13965.847 ms | 69010.485 ms | 0.77x          |
| "http"    | 4   | 1086637  | 19291.400 ms | 6710.708 ms  | 6924.697 ms    | 1941.519 ms    | 13738.335 ms | 65398.134 ms | 0.97x          |
| "mono"    | 4   | 178429   | 20743.550 ms | 7529.287 ms  | 7507.697 ms    | 1920.805 ms    | 13747.990 ms | 59726.073 ms | 1.00x          |
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
| "quick"   | 5   | 741907   | 15202.556 ms | 5081.373 ms  | 5282.580 ms    | 2039.986 ms    | 11478.977 ms | 57115.950 ms | 0.96x          |
| "sense"   | 5   | 898810   | 18863.728 ms | 5572.854 ms  | 5836.104 ms    | 2102.736 ms    | 20262.565 ms | 64691.890 ms | 0.95x          |
| "fifth"   | 5   | 108466   | 17616.947 ms | 7189.066 ms  | 6020.136 ms    | 1954.081 ms    | 18709.428 ms | 59659.332 ms | 1.19x          |
| "to an"   | 5   | 1093173  | 20318.401 ms | 8606.791 ms  | 7213.155 ms    | 2597.380 ms    | 35579.819 ms | 69429.318 ms | 1.19x          |
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
| "endure"  | 6   | 139716   | 16298.329 ms | 5957.643 ms  | 5281.980 ms    | 1915.754 ms    | 20444.352 ms | 72233.524 ms | 1.13x          |
| "London"  | 6   | 724854   | 15019.120 ms | 5601.943 ms  | 4863.998 ms    | 1981.511 ms    | 13286.475 ms | 57342.216 ms | 1.15x          |
| "strict"  | 6   | 560356   | 15085.979 ms | 5275.094 ms  | 5005.964 ms    | 2233.248 ms    | 18554.034 ms | 67324.607 ms | 1.05x          |
| "toward"  | 6   | 1306040  | 14388.026 ms | 5308.910 ms  | 5200.514 ms    | 2159.215 ms    | 20142.844 ms | 70040.596 ms | 1.02x          |
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
| "slowest" | 7   | 2152     | 13905.997 ms | 4491.385 ms  | 4149.382 ms    | 2021.872 ms    | 21261.204 ms | 67169.597 ms | 1.08x          |
| "Britain" | 7   | 163806   | 13899.570 ms | 5768.329 ms  | 4468.473 ms    | 1958.476 ms    | 18500.676 ms | 58071.149 ms | 1.29x          |
| "lioness" | 7   | 4715     | 13730.617 ms | 4880.518 ms  | 4519.353 ms    | 1956.757 ms    | 19711.737 ms | 62043.882 ms | 1.08x          |
| "to read" | 7   | 145683   | 13969.450 ms | 5302.419 ms  | 4794.759 ms    | 2229.952 ms    | 30013.817 ms | 70977.369 ms | 1.11x          |
+-----------+-----+----------+--------------+--------------+----------------+----------------+--------------+--------------+----------------+
```
    
Sanmayce:    
How about Railgun_Scalar a.k.a. Railgun_Trolldom against musl_memmem?    
    
```
Grok3:
4 Bytes

    "fast": 7298.212 ms vs. 13204.628 ms — 1.81x (Trolldom 1.81x faster).
    "that": 13184.851 ms vs. 13965.847 ms — 1.06x (Trolldom 1.06x faster).
    "http": 6924.697 ms vs. 13738.335 ms — 1.98x (Trolldom 1.98x faster).
    "mono": 7507.697 ms vs. 13747.990 ms — 1.83x (Trolldom 1.83x faster).
    Avg Ratio: 1.67x — Trolldom is ~1.67x faster.
    Hits Context: "that" (39.4M hits) tightens it; "http" (1.08M) shows Trolldom’s dominance.

5 Bytes

    "quick": 5282.580 ms vs. 11478.977 ms — 2.17x (Trolldom 2.17x faster).
    "sense": 5836.104 ms vs. 20262.565 ms — 3.47x (Trolldom 3.47x faster).
    "fifth": 6020.136 ms vs. 18709.428 ms — 3.11x (Trolldom 3.11x faster).
    "to an": 7213.155 ms vs. 35579.819 ms — 4.93x (Trolldom 4.93x faster).
    Avg Ratio: 3.42x — Trolldom is ~3.42x faster.
    Hits Context: "to an" (1.09M) amplifies Trolldom’s lead; "quick" (741K) still a blowout.

6 Bytes

    "endure": 5281.980 ms vs. 20444.352 ms — 3.87x (Trolldom 3.87x faster).
    "London": 4863.998 ms vs. 13286.475 ms — 2.73x (Trolldom 2.73x faster).
    "strict": 5005.964 ms vs. 18554.034 ms — 3.71x (Trolldom 3.71x faster).
    "toward": 5200.514 ms vs. 20142.844 ms — 3.87x (Trolldom 3.87x faster).
    Avg Ratio: 3.55x — Trolldom is ~3.55x faster.
    Hits Context: "toward" (1.3M) and "London" (724K) show consistent thrashing.

7 Bytes

    "slowest": 4149.382 ms vs. 21261.204 ms — 5.12x (Trolldom 5.12x faster).
    "Britain": 4468.473 ms vs. 18500.676 ms — 4.14x (Trolldom 4.14x faster).
    "lioness": 4519.353 ms vs. 19711.737 ms — 4.36x (Trolldom 4.36x faster).
    "to read": 4794.759 ms vs. 30013.817 ms — 6.26x (Trolldom 6.26x faster).
    Avg Ratio: 4.97x — Trolldom is ~4.97x faster.
    Hits Context: "to read" (145K) maximizes the gap; "slowest" (2K) still a massacre.

Summary

    Wins: Trolldom 16/16 — no contest, musl doesn’t win a single round.
```
    
Sanmayce:    
How about Railgun_Scalar a.k.a. Railgun_Trolldom against GLIBC_memmem?    
    
```
Grok3:
4 Bytes

    "fast": 7237.240 ms vs. 7298.212 ms — 0.99x (GLIBC slightly faster, 1.01x).
    "that": 10142.697 ms vs. 13184.851 ms — 0.77x (GLIBC faster, 1.30x).
    "http": 6710.708 ms vs. 6924.697 ms — 0.97x (GLIBC slightly faster, 1.03x).
    "mono": 7529.287 ms vs. 7507.697 ms — 1.00x (tie, even match).
    Avg Ratio: 0.93x — GLIBC is ~1.08x faster.
    Hits Context: "that" (39.4M hits) favors GLIBC’s Two-Way; "mono" (178K) shows Trolldom parity.

5 Bytes

    "quick": 5081.373 ms vs. 5282.580 ms — 0.96x (GLIBC slightly faster, 1.04x).
    "sense": 5572.854 ms vs. 5836.104 ms — 0.95x (GLIBC slightly faster, 1.05x).
    "fifth": 7189.066 ms vs. 6020.136 ms — 1.19x (Trolldom faster, 1.19x).
    "to an": 8606.791 ms vs. 7213.155 ms — 1.19x (Trolldom faster, 1.19x).
    Avg Ratio: 1.07x — Trolldom is ~1.07x faster.
    Hits Context: "to an" (1.09M) and "fifth" (108K) boost Trolldom; "quick" (741K) keeps GLIBC close.

6 Bytes

    "endure": 5957.643 ms vs. 5281.980 ms — 1.13x (Trolldom faster, 1.13x).
    "London": 5601.943 ms vs. 4863.998 ms — 1.15x (Trolldom faster, 1.15x).
    "strict": 5275.094 ms vs. 5005.964 ms — 1.05x (Trolldom faster, 1.05x).
    "toward": 5308.910 ms vs. 5200.514 ms — 1.02x (Trolldom faster, 1.02x).
    Avg Ratio: 1.09x — Trolldom is ~1.09x faster.
    Hits Context: "London" (724K) and "endure" (139K) highlight Trolldom’s edge; "toward" (1.3M) narrows it.

7 Bytes

    "slowest": 4491.385 ms vs. 4149.382 ms — 1.08x (Trolldom faster, 1.08x).
    "Britain": 5768.329 ms vs. 4468.473 ms — 1.29x (Trolldom faster, 1.29x).
    "lioness": 4880.518 ms vs. 4519.353 ms — 1.08x (Trolldom faster, 1.08x).
    "to read": 5302.419 ms vs. 4794.759 ms — 1.11x (Trolldom faster, 1.11x).
    Avg Ratio: 1.14x — Trolldom is ~1.14x faster.
    Hits Context: "Britain" (163K) maximizes Trolldom’s lead; "slowest" (2K) stays competitive.

Summary

    Wins: Trolldom 11/16 (most 5-7 bytes), GLIBC 3/16 ("that", "quick", "sense"), ties 2/16 ("fast", "mono").
```

Thus, since release 6 the hiccup was dealt with.

![3d3178b2-70ad-43b4-96fb-c321d11f1a55](https://github.com/user-attachments/assets/9e987b97-1fa2-486b-b3d7-3b7d5f8d6f97)

Oh, and the old place where the Railgun was crafted:    
https://www.codeproject.com/Articles/250566/Fastest-strstr-like-function-in-C

Enfun!    
2025-Mar-12, Sanmayce

String Matching Showdown against Text
a short "benchmarking" review by Sanmayce, 2025-Apr-12
    
Two key methods were "enforced":
- stressing RAM far outside the LLC (Last-Level-Cache)
- stressing both Latency and Bandwidth of functions by finding all matches within the Haystack
    
```
  _________ __         .__                                    _____  __      _________.__                     .___                      
 /   _____//  |________|__| ____    ____   ________________ _/ ____\/  |_   /   _____/|  |__   ______  _  ____| _/______  _  ______     
 \_____  \\   __\_  __ \  |/    \  / ___\_/ ___\_  __ \__  \\   __\\   __\  \_____  \ |  |  \ /  _ \ \/ \/ / __ |/  _ \ \/ \/ /    \    
 /        \|  |  |  | \/  |   |  \/ /_/  >  \___|  | \// __ \|  |   |  |    /        \|   Y  (  <_> )     / /_/ (  <_> )     /   |  \   
/_______  /|__|  |__|  |__|___|  /\___  / \___  >__|  (____  /__|   |__|   /_______  /|___|  /\____/ \/\_/\____ |\____/ \/\_/|___|  /   
        \/                     \//_____/      \/           \/                      \/      \/                  \/                 \/    
________________    _______________________________ ____________________                                                    ___  ___    
\_   _____/  _  \  /   _____/\__    ___/\_   _____//   _____/\__    ___/   _____   ____   _____   _____   ____   _____     /  /  \  \   
 |    __)/  /_\  \ \_____  \   |    |    |    __)_ \_____  \   |    |     /     \_/ __ \ /     \ /     \_/ __ \ /     \   /  /    \  \  
 |     \/    |    \/        \  |    |    |        \/        \  |    |    |  Y Y  \  ___/|  Y Y  \  Y Y  \  ___/|  Y Y  \ (  (      )  ) 
 \___  /\____|__  /_______  /  |____|   /_______  /_______  /  |____|    |__|_|  /\___  >__|_|  /__|_|  /\___  >__|_|  /  \  \    /  /  
     \/         \/        \/                    \/        \/                   \/     \/      \/      \/     \/      \/    \__\  /__/   
```
    
In search for the FASTEST memmem()... finding needles in haystacks, FAST!
    
Tested datafile: gutenberg_en_all_2023-08_132000-html-files.tar, 26,914,119,680 bytes;
    
Tested machine: ThinkPad L490, i7-8565U "Whiskey Lake", TDP: 15 W (MAX 25 W), 14 nm, Max Turbo: 4,600 MHz, 2x32 GB DDR4 2400 MT/s, Linux Fedora 39;
    
Tested memmem-functions: BMH (order 1), GLIBC 2.38, Scalar and Vector Railguns, MUSL, KMP;
    
Tested compiler: CLANG 18.1.8, using -O3 -mavx2;
    
```
+------------------[ Length ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| Needle                      |        Hits |      BMH |    GLIBC | Trolldom | Nyotengu |     MUSL |   KMP | Speedup, Trolldom/GLIBC |
+-----------------[ 2 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "to"                        | 143,927,190 |    607.2 |   1460.8 |   1944.2 |  8012.5* |   1805.3 | 385.9 |   1944.2/1460.8 = 1.33x |
| ");"                        |     688,162 |    748.5 |   1543.4 |   2124.3 | 17107.6* |   1916.4 | 450.6 |   2124.3/1543.4 = 1.37x |
| "  "                        | 333,019,331 |    486.4 |   1412.5 |   1883.5 |  7159.1* |   1673.8 | 324.9 |   1883.5/1412.5 = 1.33x |
+-----------------[ 3 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "and"                       | 129,433,752 |    878.8 |   1697.0 |   2142.4 |  7767.2* |   1844.1 | 379.4 |   2142.4/1697.0 = 1.26x |
+-----------------[ 4 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "...""                      |   1,265,101 |   1382.7 |   3716.7 |   4924.3 | 12315.5* |   1781.0 | 430.8 |   4924.3/3716.7 = 1.32x |
| "fast"                      |     861,040 |   1172.5 |   3340.6 |   4293.6 | 12144.3* |   1837.1 | 424.3 |   4293.6/3340.6 = 1.29x |
| "that"                      |  39,409,672 |   1099.8 |   2381.6 |   3351.6 |  5653.3* |   1761.2 | 364.6 |   3351.6/2381.6 = 1.41x |
| "http"                      |   1,086,637 |   1277.6 |   3453.1 |   4720.6 | 12258.1* |   1750.1 | 401.9 |   4720.6/3453.1 = 1.37x |
| "mono"                      |     178,429 |   1237.3 |   3304.6 |   4295.9 | 12931.9* |   1834.1 | 423.0 |   4295.9/3304.6 = 1.30x |
| "mere"                      |     869,253 |   1064.3 |   2421.4 |   3414.4 | 12131.0* |   1817.0 | 414.8 |   3414.4/2421.4 = 1.41x |
| "buzz"                      |      33,352 |   1445.9 |   3730.8 |   5036.6 | 13058.7* |   1762.6 | 428.9 |   5036.6/3730.8 = 1.35x |
| "the "                      | 195,784,628 |    764.1 |   1331.7 |  2061.9* |   1779.4 |   1643.7 | 340.3 |   2061.9/1331.7 = 1.55x |
| "zest"                      |      14,976 |   1144.4 |   3435.7 |   4377.8 | 13207.4* |   1863.8 | 444.7 |   4377.8/3435.7 = 1.27x |
| "book"                      |   5,957,440 |   1367.5 |   3300.6 |   4656.7 | 10331.5* |   1805.9 | 417.8 |   4656.7/3300.6 = 1.41x |
| "xylo"                      |       2,278 |   1272.0 |   4102.0 |   5028.7 | 13234.3* |   1902.8 | 446.9 |   5028.7/4102.0 = 1.23x |
+-----------------[ 5 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "Jesus"                     |     290,706 |   1514.7 |   4620.5 |   4519.9 | 12964.1* |   1740.4 | 444.4 |   4519.9/4620.5 = 0.98x |
| "sense"                     |     898,810 |   1357.4 |   4355.3 |   4180.7 | 11835.5* |   1442.7 | 391.8 |   4180.7/4355.3 = 0.96x |
| "Satan"                     |      55,123 |   1422.9 |   3196.4 |   3966.1 | 13058.0* |   1473.8 | 437.9 |   3966.1/3196.4 = 1.24x |
| "Panza"                     |       2,612 |   1538.6 |   4257.1 |   4483.1 | 13271.3* |   2113.5 | 446.0 |   4483.1/4257.1 = 1.05x |
+-----------------[ 6 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "endure"                    |     139,716 |   1525.3 |   3921.4 |   4488.6 | 12442.2* |   1386.1 | 338.6 |   4488.6/3921.4 = 1.14x |
| "London"                    |     724,854 |   1705.8 |   4198.5 |   4868.6 | 12188.0* |   2160.8 | 440.0 |   4868.6/4198.5 = 1.16x |
| "strict"                    |     560,356 |   1618.2 |   4875.1 |   5038.0 | 11302.1* |   1577.5 | 383.7 |   5038.0/4875.1 = 1.03x |
| "toward"                    |   1,306,040 |   1785.3 |   4870.9 |   4926.2 | 11695.7* |   1422.3 | 376.9 |   4926.2/4870.9 = 1.01x |
| "thinks"                    |     180,426 |   1804.6 |   3942.7 |   4273.3 |  7905.2* |   1473.6 | 373.1 |   4273.3/3942.7 = 1.08x |
+-----------------[ 7 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "slowest"                   |       2,152 |   1845.2 |   5427.7 |   5960.1 | 12689.7* |   1348.6 | 399.0 |   5960.1/5427.7 = 1.10x |
| "Britain"                   |     163,806 |   1851.0 |   4369.0 |   5605.5 | 12720.1* |   1532.5 | 447.7 |   5605.5/4369.0 = 1.28x |
| "lioness"                   |       4,715 |   1867.9 |   4995.6 |   5453.8 | 12589.8* |   1371.5 | 405.8 |   5453.8/4995.6 = 1.09x |
| "to read"                   |     145,683 |   1838.0 |   4797.7 |   5276.7 | 11384.1* |    953.3 | 365.1 |   5276.7/4797.7 = 1.10x |
| "Quixote"                   |      19,820 |   1806.9 |   5961.4 |   6439.2 | 13313.3* |   1526.5 | 449.1 |   6439.2/5961.4 = 1.08x |
| "logical"                   |     315,326 |   1955.9 |   5231.3 |   6072.6 | 12086.8* |   1738.2 | 415.8 |   6072.6/5231.3 = 1.16x |
| "message"                   |     201,553 |   1836.1 |   4988.5 |   5658.5 | 12873.0* |   1695.2 | 420.9 |   5658.5/4988.5 = 1.13x |
| "sandals"                   |       9,031 |   2009.2 |   4628.1 |   5646.7 | 12427.7* |   1839.1 | 394.4 |   5646.7/4628.1 = 1.22x |
+----------------[ 13 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "Fleurs du mal"             |          15 |   2966.6 |   7364.1 |   9509.1 | 13721.3* |   1592.2 | 449.9 |   9509.1/7364.1 = 1.29x |
+----------------[ 14 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "Wilhelm Keitel"            |          26 |   3158.8 |   7795.8 |  11339.6 | 13030.3* |   1806.4 | 443.9 |  11339.6/7795.8 = 1.45x |
+----------------[ 23 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "Project Gutenberg Ebook"   |      75,733 |   4740.0 |   8920.4 |  11039.0 | 11954.9* |   2496.5 | 439.4 |  11039.0/8920.4 = 1.23x |
+----------------[ 25 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "Simplicius Simplicissimus" |           3 |   5472.9 |  10672.3 |  11841.0 | 12914.1* |   3625.9 | 443.0 | 11841.0/10672.3 = 1.10x |
+----------------[ 66 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "documen...ent to make you" |           3 |   5844.5 |   8593.4 | 19142.9* |  13010.6 |   3986.6 | 410.0 |  19142.9/8593.4 = 2.22x |
+----------------[ 71 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| "        <m... Ebooks." />" |      75,731 |   7355.1 |  11459.1 | 15861.3* |   4164.3 |   4999.6 | 301.2 | 15861.3/11459.1 = 1.38x |
+---------------[ 263 Bytes ]-+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
| ""Choose a lan...y title"}" |      75,731 |   8464.9 |   6595.2 | 13975.5* |  12719.5 |   6180.9 | 428.2 |  13975.5/6595.2 = 2.11x |
+-----------------------------+-------------+----------+----------+----------+----------+----------+-------+-------------------------+
```
    
Note1: The numbers are in MiB/s, all results are obtained as superuser/root with maximum niceness -20; 
    
Note2: 'BMH' and 'KMP' were written by chatGPT;
    
Note3: '*' denotes fastest result within the row;
    
Note4: Last needle (since it is > 256) is searched by Two-Way within the GLIBC, otherwise by pseudo-BMH order 2;
    
Note5: For range 8..12 long needles, Trolldom gets even faster because the my true BMH2 bitwise gains momentum;
    
Note6: The Trolldom's actual (for needles == 4) ASM code executed in this benchmark is this:
    
```
	cmp	ebx, 4
	jne	.LBB17_18
	mov	ecx, dword ptr [rsi]
	mov	edx, ecx
	shr	edx, 8
	mov	esi, ecx
	shr	esi, 16
	xor	r8d, r8d
	jmp	.LBB17_15
	.p2align	4, 0x90
.LBB17_13:
	lea	rax, [rdi + r8]
	mov	r9d, 1
	cmp	dword ptr [rax], ecx
	je	.LBB17_69
.LBB17_14:
	add	r8, r9
	cmp	r8, r12
	ja	.LBB17_1
.LBB17_15:
	movzx	eax, word ptr [rdi + r8 + 2]
	cmp	ax, si
	je	.LBB17_13
	cmp	ax, dx
	je	.LBB17_13
	mov	r9d, 3
	cmp	ax, cx
	jne	.LBB17_14
	jmp	.LBB17_13
.LBB17_18:
	cmp	ebx, 13 
```
    
Bottomline: Railgun 'Trolldom' is the fastest SCALAR hitter, with few hiccups/jams.

Enfun!    
2025-Apr-12, Sanmayce
