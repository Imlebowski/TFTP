IFLAG := -I ../include
SRCS := $(wildcard *.c)
TARGET := client

${TARGET} : ${SRCS}
	gcc $^ ${IFLAG} -o $@

clean :
	rm ${TARGET}
