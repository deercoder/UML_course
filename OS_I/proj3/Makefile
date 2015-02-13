all: node_controller buf_manager test_buf net_producer

node_controller: node_controller.c global.h ddonuts.h buf_mgr.h
	gcc -o node_controller node_controller.c

buf_manager: buf_mgr.c buf_mgr.h global.h
	gcc -o buf_manager buf_mgr.c

test_buf: test_buf_mgr.c global.h buf_mgr.h
	gcc -o test_buf test_buf_mgr.c

net_producer: net_producer.o util.o
	gcc -o net_producer net_producer.o util.o

net_producer.o: net_producer.c global.h util.h
	gcc -c net_producer.c

util.o: util.c global.h util.h
	gcc -c util.c

clean:
	rm node_controller *.o
