COMPILER = gcc
HEADER = keyvalue.h
all:
ifneq (,$(wildcard client.c))
	${COMPILER} client.c -o client
endif
ifneq (,$(wildcard serv1.c))
	${COMPILER} serv1.c ${HEADER} -o serv1
endif
ifneq (,$(wildcard serv2.c))
	${COMPILER} serv2.c ${HEADER} -o serv2
endif
ifneq (,$(wildcard serv3.c))
	${COMPILER} serv3.c ${HEADER} -o serv3
endif
ifneq (,$(wildcard serv4.c))
	${COMPILER} serv4.c ${HEADER} -o serv4 -pthread
endif