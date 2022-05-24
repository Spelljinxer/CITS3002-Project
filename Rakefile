# A typical Rakefile

PORT  = 6230
HOSTS = localhost #localhost:6239 localhost:6240

actionset1:
	remote-true
	ls Rakefile not-a-file
	remote-true
	#remote-ls Rakefile not-a-file
	echo starting actionset1 action 3
		requires test1 test2 test3
	remote-echo starting actionset1 action 4
	remote-echo starting actionset1 action 5
	#remote-cc [optional-flags] -c program.c
		#requires program.c program.h allfunctions.h
	#remote-cc [optional-flags] -c square.c
		#requires square.c allfunctions.h
	#remote-cc [optional-flags] -c cube.c
		#requires cube.c allfunctions.h

actionset2:
	remote-false
	remote-echo starting actionset2 action 2
	fwefwoifhwe
	#remote-cc [optional-flags] -o program program.o square.o cube.o
		#requires program.o square.o cube.o
