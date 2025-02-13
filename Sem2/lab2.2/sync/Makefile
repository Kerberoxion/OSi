TARGET_1 = queue-example
SRCS_1 = queue.c queue-example.c

TARGET_2 = queue-spinlock
SRCS_2 = queue.c queue-spinlock.c

TARGET_3 = queue-mutex
SRCS_3 = queue.c queue-mutex.c

TARGET_4 = queue-condvar
SRCS_4 = queue.c queue-condvar.c

TARGET_5 = queue-sem
SRCS_5 = queue.c queue-sem.c


CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2} ${TARGET_3} ${TARGET_4} ${TARGET_5}

${TARGET_1}: queue.h ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}

${TARGET_2}: queue.h ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}

${TARGET_3}: queue.h ${SRCS_3}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_3} ${LIBS} -o ${TARGET_3}

${TARGET_4}: queue.h ${SRCS_4}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_4} ${LIBS} -o ${TARGET_4}

${TARGET_5}: queue.h ${SRCS_5}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_5} ${LIBS} -o ${TARGET_5}

clean:
	${RM} -f *.o ${TARGET_1} ${TARGET_2} ${TARGET_3} ${TARGET_4} ${TARGET_5}
