
PROTOC=protoc
PROTO=./

all: cpp py

# PROTOS
py: 
	protoc -I $(PROTO) --python_out=$(PROTO) $(PROTO)/*.proto

cpp: 
	protoc -I $(PROTO) --cpp_out=$(PROTO) $(PROTO)/*.proto

clean :
	rm -f *.o $(PROTO)/*.cc $(PROTO)/*.h $(PROTO)/*.py


