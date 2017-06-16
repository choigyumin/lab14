CPP = g++
CPPFLAGS = -std=c++11 -O3
PROTO = ./proto
SRC = ./src
BUILD = ./build
INCLUDE =  -I ./include -I$(PROTO)

all: $(BUILD)/libpylib.so

$(BUILD)/libpylib.so: $(BUILD)/py_lib.o $(BUILD)/caffe.pb.o $(BUILD)/digit_dnn.o $(BUILD)/fpga_api.o
	$(CPP) -shared -o $@  $^ -lprotobuf

$(PROTO)/caffe.pb.h : $(PROTO)/caffe.pb.cc	
$(PROTO)/caffe.pb.cc : $(PROTO)/caffe.proto
	protoc -I=$(PROTO) --cpp_out=$(PROTO) $^

$(BUILD)/caffe.pb.o: $(PROTO)/caffe.pb.cc 
	$(CPP) -fPIC $(CPPFLAGS) $(INCLUDE) -o $@ -c $^ 
	
$(BUILD)/digit_dnn.o: $(SRC)/digit_dnn.cpp | $(PROTO)/caffe.pb.h 
	$(CPP) -fPIC $(CPPFLAGS) $(INCLUDE) -o $@ -c $^ 
	
$(BUILD)/fpga_api.o: $(SRC)/fpga_api_pseudo.cpp | $(PROTO)/caffe.pb.h 
	$(CPP) -fPIC $(CPPFLAGS) $(INCLUDE) -o $@ -c $^ 
	
$(BUILD)/py_lib.o: $(SRC)/py_lib.cpp | $(PROTO)/caffe.pb.h 
	$(CPP) -fPIC $(CPPFLAGS) $(INCLUDE) -o $@ -c $^ 
	

clean:	
	rm $(PROTO)/caffe.pb.* ./build/* 
