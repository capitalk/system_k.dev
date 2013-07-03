
PROTOBUF_OBJS = new_order_single.pb.o \
				order_cancel.pb.o \
				order_cancel_reject.pb.o \
				order_cancel_replace.pb.o \
				capk_globals.pb.o \
				execution_report.pb.o \
				spot_fx_md_1.pb.o \
				order_status.pb.o \
                position.pb.o \
                venue_configuration.pb.o \
                recovered_orders.pb.o 

capk_globals.pb.o: $(PROTO)/capk_globals.pb.cc $(PROTO)/capk_globals.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/capk_globals.pb.cc

new_order_single.pb.o: $(PROTO)/new_order_single.pb.cc $(PROTO)/new_order_single.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/new_order_single.pb.cc

order_cancel.pb.o: $(PROTO)/order_cancel.pb.cc $(PROTO)/order_cancel.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel.pb.cc
   
order_cancel_reject.pb.o: $(PROTO)/order_cancel_reject.pb.cc $(PROTO)/order_cancel_reject.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel_reject.pb.cc

order_cancel_replace.pb.o: $(PROTO)/order_cancel_replace.pb.cc $(PROTO)/order_cancel_replace.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_cancel_replace.pb.cc

execution_report.pb.o: $(PROTO)/execution_report.pb.cc $(PROTO)/execution_report.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/execution_report.pb.cc

spot_fx_md_1.pb.o: $(PROTO)/spot_fx_md_1.pb.cc $(PROTO)/spot_fx_md_1.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/spot_fx_md_1.pb.cc

order_status.pb.o: $(PROTO)/order_status.pb.cc $(PROTO)/order_status.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/order_status.pb.cc

position.pb.o: $(PROTO)/position.pb.cc $(PROTO)/position.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/position.pb.cc

venue_configuration.pb.o: $(PROTO)/venue_configuration.pb.cc $(PROTO)/venue_configuration.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/venue_configuration.pb.cc

recovered_orders.pb.o: $(PROTO)/recovered_orders.pb.cc $(PROTO)/recovered_orders.pb.h
	$(CC) -o $@ -c $(INCLUDE) $(CFLAGS) $(PROTO)/recovered_orders.pb.cc
