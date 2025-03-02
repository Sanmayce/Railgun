# Railgun
Fastest Scalar&amp;Vector memmem() in C?!

![eb0af275-eb32-4d44-827a-bac00cbc284b](https://github.com/user-attachments/assets/a3789a4e-fbb1-4225-ba82-6f66ac85ce2a)

This is the C sourcecode of Railgun_Trolldom and Railgun_Nyotengu memmem-like functions.

Also, a benchmark is included juxtaposing GLIBC memmem() to both Railguns, here come the results on my laptop running Fedora 39 Linux:


![r3](https://github.com/user-attachments/assets/908d09bb-c4e9-4fd6-9d64-01ba141e5014)

I plan to boost the vector variant by replacing the 2,3 bytes Needle Length cases with vectorized code, now they are scalar...

Oh, and the old place where the Railgun was crafted:

https://www.codeproject.com/Articles/250566/Fastest-strstr-like-function-in-C

Enfun!    
2025-Mar-02, Sanmayce
