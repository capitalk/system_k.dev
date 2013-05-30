CC=g++

OB1=./order_book.v1
OB2=./order_book.v2
TEST=./test

all: libOrderBook2.a libOrderBook.a test

libOrderBook.a: 
	cd $(OB1) && make 

libOrderBook2.a: 
	cd $(OB2) && make

test: libOrderBook.a libOrderBook2.a
	cd $(OB1)/test && make
	cd $(OB2)/test && make
	cd $(TEST) && make


clean :
	cd $(TEST) && make clean
	cd $(OB1) && make clean
	cd $(OB1)/test && make clean
	cd $(OB2) && make clean
	cd $(OB2)/test && make clean
