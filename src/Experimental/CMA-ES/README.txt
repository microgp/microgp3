Beautify
========
- replace all instances of new_void() with meaningful allocation (new, throw, etc.)
- replace rgrand with a vector<long> 
- FATAL must be removed *everywhere*

How to
======
- compile
	qmake
	make

- run
	./cmaes (it will list all the options)

2014-09-22
----------
The part that prints out the logs is inside cmaes_ReadSignals; and it does not work if a file "signals.par" is not inside the working directory.

2014-04-18
----------
Now it's compiling, and the messy initialization functions are (almost) all replaced by ::readXml

However, I need to keep track of the dreadful jumps in the code between functions during initialization.

cmaes_init()
	-> readpara_init()
	 

2013-07-10
----------
Started working on RandomT class, needed to implement a random number generator.
