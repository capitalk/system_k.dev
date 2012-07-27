
PROTOC=protoc
PROTO_OUT?=./
PROTO_DIR=.

all: cpp py ctags


# PROTOS
py: 
	protoc -I $(PROTO_DIR) --python_out=$(PROTO_OUT) $(PROTO_DIR)/*.proto

cpp: 
	protoc -I $(PROTO_DIR) --cpp_out=$(PROTO_OUT) $(PROTO_DIR)/*.proto

ctags:
	ctags *.h *.cc

clean :
	rm -f *.o $(PROTO_DIR)/*.cc $(PROTO_DIR)/*.h $(PROTO_DIR)/*.py


