#include "py_lib.h"

#include "digit_dnn.h"
#include "fpga_api.h"

#include <string>

void* getNet(void* network)
{
  char* net_char = static_cast<char*>(network);
  FPGA* dev = new FPGA(0x40000000, 0x43c00000);
  return new DigitDNN(std::string(net_char), dev);
}

void delNet(void* net_ptr)
{
  DigitDNN* net = static_cast<DigitDNN*>(net_ptr);
  FPGA* dev = net->getDev();
  
  delete net;
  delete dev;
}

void inference(void* net_ptr, const void* in, void* out)
{
  DigitDNN* net = static_cast<DigitDNN*>(net_ptr);
  const float* rtn = net->run(static_cast<const float*>(in), false);

  float* out_f = static_cast<float*>(out);

  for(int i = 0 ; i < 10; ++i)
  {
    out_f[i] = rtn[i];
  }
}
