all:findtopk	findtopk_mqueue	findtopk_thread
findtopk:
	gcc	-g	-Wall	-o findtopk	findtopk.c
findtopk_mqueue:
	gcc	-g	-Wall	-o	findtopk_mqueue	findtopk_mqueue.c
findtopk_thread:
	gcc	-g	-Wall	-o	findtopk_thread	findtopk_thread.c	-lpthread
clean:	
	rm	-fr	findtopk	findtopk.o	*~	
	rm	-fr	findtopk_mqueue	findtopk_mqueue.o	*~	
	rm	-fr	findtopk_thread	findtopk_thread.o	*~
