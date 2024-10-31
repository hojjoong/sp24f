SRCS1 = static_main.c
SRCS2 = dynamic_main.c
LIBSRCS = open.c close.c put.c get.c 
TARG_STATIC = kvs_static
TARG_DYNAMIC = kvs_dynamic

CC = gcc
OPTS = -Wall -g
LIBS = -L. -lkvs

OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
LOBJS = $(LIBSRCS:.c=.o)
LARS = libkvs.a
LARD = libkvs.so

#환경 변수에 경로 지정
export LD_LIBRARY_PATH=.

all: static dynamic

static: $(TARG_STATIC)
#static_main.o와 libkvs.a link
$(TARG_STATIC) : $(OBJS1) $(LARS)
	$(CC) -o $(TARG_STATIC) $(OBJS1) $(LIBS) -static

#libkvs.a 생성
$(LARS) : $(LOBJS)
	ar rs $(LARS) $(LOBJS)

#dynamic 라이브러리로 실행 파일 생성
dynamic: $(TARG_DYNAMIC)

#dynamic_main.c와 libkvs.so link
$(TARG_DYNAMIC): $(LARD)
	$(CC) -o $(TARG_DYNAMIC) $(SRCS2) -ldl

#libkvs.so 생성
$(LARD):
	$(CC) -shared -o $(LARD) $(LIBSRCS)


%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

# dynamic: $(TARG_DYNAMIC)
# 	$(CC) -o $(TARG) $(OBJS) $(LIBS) 

clean:
	rm -f $(OBJS1) $(OBJS2) $(LOBJS) $(LARS) $(LARD) $(TARG_STATIC) $(TARG_DYNAMIC)

