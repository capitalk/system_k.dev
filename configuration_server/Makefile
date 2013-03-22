CAPK_ROOT=../..
PROTOC=protoc
PROTO=$(CAPK_ROOT)/common/proto
PROTO_OUT=./proto_objs

all: protos


protos: configuration_server.py
	mkdir -p $(PROTO_OUT)
	$(PROTOC) -I$(PROTO) --python_out=$(PROTO_OUT) $(PROTO)/*.proto
	touch $(PROTO_OUT)/__init__.py

clean :
	rm -f $(PROTO_OUT)/*.py $(PROTO_OUT)/*.pyc *.pid.lock dev-1*

.phony: configuration_server.py

