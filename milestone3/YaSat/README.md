Yet Another SAT Solver
===
*Sample_benchmarks, parser.h, parser.cpp and Makefile was not created by me.  

Introduce
---
A brute force boolean satisfiability problem solver with these optimization:
- 2-literal watching
- First UIP
- Non-chronological backtracking
- Assign true or false randomly
- Restart if conflict 10 * the number of variables times,

Build and Run
---
```
make
./yasat <input.cnf>
```
***
Test all .cnf file in Sample_benchmarks (except par32-1 and par32-1-c).  
The result will be in Sample_benchmarks/filename.sat
```
make
./test.sh
```
