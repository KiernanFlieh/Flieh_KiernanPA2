# Part B answers

For each program, one sentence naming the actual defect, and one sentence
saying what you changed. Do not paste code here; the code is in the .c files.

## bug1_args.c
Defect: Issue was that we gave all the threads the same variable i, causing them to read incorrect values whenever the main thread updated it. 
Fix: Created an array that stores each threads index separetely allowing us to pass each thread its own index. 

## bug2_nojoin.c
Defect:
Fix:

## bug3_stack.c
Defect:
Fix:

## bug4_errno.c
Defect:
Fix:
