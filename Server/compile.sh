gcc src/main.c src/constants.h src/main_server.c src/main_server.h src/thread.c src/thread.h src/messaging.c src/messaging.h src/structs/list_head.c src/structs/list_head.h src/structs/user.c src/structs/user.h -l pthread -o server -Wall