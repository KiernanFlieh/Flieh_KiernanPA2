# Group Members: Kiernan Flieh, Cyril Tabaranza, Nathan Nguyen, Jared Fabian, Nick Pieratos

# Part B answers

For each program, one sentence naming the actual defect, and one sentence
saying what you changed. Do not paste code here; the code is in the .c files.

## bug1_args.c
Defect: Issue was that we gave all the threads the same variable i, causing them to read incorrect values whenever the main thread updated it. 
Fix: Created an array that stores each threads index separetely allowing us to pass each thread its own index. 

## bug2_nojoin.c
Defect: Issue was that the main thread tried to calculate total before the other threads finished.
Fix: Created a loop that uses pthread_join() which waits for every thread before attempting to calculate total. 

## bug3_stack.c
Defect: Issue was that the thread returned the address of a variable that no longer exist after the function finishes. 
Fix: Made a static array that stores each threads result separately and returns address of position in the array. 

## bug4_errno.c
Defect: Issue was that we were checking for -1 and not checking for a nonzero error code from pthread_join(). 
Fix: Adjusted the checks so that they store the error code in rc and check if its nonzero, also changed strerror(errno) to strerror(rc) to correct the message.
