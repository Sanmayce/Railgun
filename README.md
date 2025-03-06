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

Oh, and the old place where the Railgun was crafted:    
https://www.codeproject.com/Articles/250566/Fastest-strstr-like-function-in-C

Enfun!    
2025-Mar-02, Sanmayce
