# Encoder
Project description link: https://cs.nyu.edu/courses/spring23/CSCI-GA.2250-002/nyuenc

Introduction:
Data compression is the process of encoding information using fewer bits than the original representation. Run-length encoding (RLE) is a simple yet effective compression algorithm: repeated data are stored as a single data and the count. In this lab, you will build a parallel run-length encoder called Not Your Usual ENCoder, or nyuenc for short.

Developed a high-performance multithreaded tour encoding program in C. Implemented a robust thread pool through the use of pthreads; achieved up to 75% runtime reduction on large datasets larger than 1GB compared to the single-threaded version.
Customized task data structures were designed using the concept of task queues to ensure balanced task workloads and efficient future merge processes; dynamic scheduling was used to allow simultaneous task encoding and merging, reducing processing latency.
Mutual-exclusion locks and condition variables were utilized to prevent race conditions, busy waits, and deadlocks on shared data structures; fine-grained and read/write locks were used to minimize the amount of time spent holding mutual-exclusion locks and reduce contention between threads.
