CC=g++

CAPK_ROOT = ~/capitalk
COMMON=$(CAPK_ROOT)/common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils

SRC=../

INCLUDE= -I$(SRC) \
         -I$(COMMON) \
         -I$(UTILS)

LIBS= -L$(UTILS) \
      -L$(SRC) \
      -lgtest \
      -lorder_book3 \
      -lpthread

CFLAGS=-Wall  -std=c++0x -O3 # -g -D DEBUG

#%.o: %.cpp
	#$(CC) $(CFLAGS) $(INCLUDE) -c $(input) -o $(output)


all: liborder_book3.a gtest_add # gtest_del gtest_mod gtest_oob

liborder_book3.a: 
	cd .. && make 



# Other tests
gtest_add: gtest_add.cpp
	$(CC) $(CFLAGS) gtest_add.cpp  $(INCLUDE) -L $(LIBS) -o $@

clean :
	rm -f  *.o 

.PHONY: clean 


