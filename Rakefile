# A typical Rakefile

PORT  = 6238
HOSTS = localhost localhost:6239 localhost:6240

actionset1:
	remote-echo starting actionset1 action 1
	remote-echo starting actionset1 action 2
	remote-echo starting actionset1 action 3
	echo starting actionset1 action 4
	echo starting actionset1 action 5
	#remote-cc [optional-flags] -c program.c
		#requires program.c program.h allfunctions.h
	#remote-cc [optional-flags] -c square.c
		#requires square.c allfunctions.h
	#remote-cc [optional-flags] -c cube.c
		#requires cube.c allfunctions.h

actionset2:
	echo starting actionset2 action 1
	remote-echo starting actionset2 action 2
	#remote-cc [optional-flags] -o program program.o square.o cube.o
		#requires program.o square.o cube.o
