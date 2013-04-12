ORDER_BOOK_OBJS = order.o order_book.o limit.o time_utils.o fix_convertors.o


all: liborder_book2.a

liborder_book2.a: $(ORDER_BOOK_OBJS)
	ar rcs liborder_book2.a $+

order.o: $(ORDER_BOOK2)/order.cpp 
	$(CC) -c $(INCLUDES) $(CFLAGS) $(ORDER_BOOK2)/order.cpp 

order_book.o: $(ORDER_BOOK2)/order_book.cpp 
	$(CC) -c $(INCLUDES) $(CFLAGS) $(ORDER_BOOK2)/order_book.cpp

limit.o: $(ORDER_BOOK2)/limit.cpp
	$(CC) -c $(INCLUDES) $(CFLAGS) $(ORDER_BOOK2)/limit.cpp

# clean :
# 	rm -f  $(ORDER_BOOK_OBJS) liborder_book2.a 




