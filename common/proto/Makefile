
PROTOC=protoc
PROTO_OUT?=./
PROTO_DIR=.

all: cpp py 

PROTOFILES=capk_globals.proto \
execution_ack.proto \
execution_report.proto \
interactive_brokers.proto \
new_order_single.proto \
order_cancel.proto \
order_cancel_reject.proto \
order_cancel_replace.proto \
order_replace.proto \
position.proto \
recovered_orders.proto \
venue_configuration.proto \
spot_fx_md_1.proto \
spot_fx_md_n.proto 

# PROTOS
py: $(PROTOFILES)
	protoc -I $(PROTO_DIR) --python_out=$(PROTO_OUT) $(PROTO_DIR)/*.proto

cpp: $(PROTOFILES)
	protoc -I $(PROTO_DIR) --cpp_out=$(PROTO_OUT) $(PROTO_DIR)/*.proto

ctags:
	ctags *.h *.cc

clean :
	rm -f *.o $(PROTO_DIR)/*.cc $(PROTO_DIR)/*.h $(PROTO_DIR)/*.py


