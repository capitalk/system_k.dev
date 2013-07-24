CC=g++

OB1=./order_book.v1
OB2=./order_book.v2
TEST=./test

all: v2 v1 test

v1: 
	cd $(OB1) && make 

v2: 
	cd $(OB2) && make

test: v1 v2
	cd $(OB1)/test && make
	cd $(OB2)/test && make
	cd $(TEST) && make


clean :
	cd $(TEST) && make clean
	cd $(OB1) && make clean
	cd $(OB1)/test && make clean
	cd $(OB2) && make clean
	cd $(OB2)/test && make clean
