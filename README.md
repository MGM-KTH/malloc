malloc
======

Testing
-------

The following changes has been made to the original test files:
– The shebang in RUN_TESTS has been changed from #!/bin/csh to #!/bin/bash.
– #include <sys/types.h> was added to tstcrash_complex.c to enable usage of caddr_t.

To run the supplied test files run the script 'test.sh'.
It will copy the files malloc.c, malloc.h, and brk.h to the testing directory
before compiling and running the tests.


To run our tests, run the script 'MANY\_RUNS.sh'.
It writes the results to .dat files in the directory data/.
The data is then plotted using gnuplot with the script gnu_plot.sh


To run 'main()' in malloc.c, which contains some basic tests,
run the following commands: 

<ol>
<li>make clean</li>
<li>make malloc</li>
<li>./malloc</li>
</ol>
