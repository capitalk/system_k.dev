
PROTOC=protoc
PROTO=./

all: protos

# PROTOS
protos: $(PROTO)/new_order_single.proto
	protoc -I $(PROTO) --cpp_out=$(PROTO) $(PROTO)/*.proto

clean :
	rm -f *.o $(PROTO)/*.cc $(PROTO)/*.h


