pc_threads_proc:
	gcc -o pc_threads_proc pc_np_ws_lin_proc.c -lpthread -DGLOBAL -DWRITE_CONSUMER_OUTPUT
	gcc -o pc_threads pc_np_ws_lin_sys.c -lpthread -DGLOBAL

#### This commented commands are used for debugging, generating prod/cons files	
#gcc -o pc_threads_proc pc_np_ws_lin_proc.c -lpthread -DGLOBAL -DDEBUG
#gcc -o pc_threads pc_np_ws_lin_sys.c -lpthread -DGLOBAL -DDEBUG

clean:
	rm pc_threads_proc pc_threads
	rm cons* prod*
