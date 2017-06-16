#-*- coding: utf-8 -*-
import numpy as np
import ctypes

import sys
sys.path.append('./mnist')
from load_mnist import load_mnist

print("read dataset...")
images, labels = load_mnist("testing", path="./mnist")
images = images.astype(np.float32)/255.

NUM_TEST_IMAGES = 100

class Network(object):
	def __init__(self, net_path):
		self.lib = ctypes.cdll.LoadLibrary("./build/libpylib.so")
		self.net = self.lib.getNet(ctypes.c_char_p(net_path.encode('utf-8')))
		self.out_buf = np.zeros((10), dtype = np.float32)
		
	def __del__(self):
		self.lib.delNet(self.net)	
		
	def inference(self, input):
		self.lib.inference(self.net, input.ctypes.data, self.out_buf.ctypes.data)
		return self.out_buf

print("create network...")		
net = Network("./mnist_model/mnist_16.caffemodel")

test_images = [images[idx, :,:].copy() for idx in xrange(NUM_TEST_IMAGES)]
print("run test...")		

import time
start_time = time.time()

for idx in xrange(NUM_TEST_IMAGES):
	print("image %d"%(idx))
	out = net.inference(test_images[idx])
	print("real number: %d"%(labels[idx, :]))
	print("prediction result: %d"%(np.argmax(out)))

end_time = time.time()
print("elapsed time: %f"%(end_time-start_time))
