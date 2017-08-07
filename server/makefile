IFLAG := -I ../include
SRCS := $(wildcard *.c)
TARGET := server

${TARGET} : ${SRCS}
	gcc $^ ${IFLAG} -o $@

clean :
	rm ${TARGET}
