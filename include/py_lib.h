#ifndef _PY_LIB_H_
#define _PY_LIB_H_

extern "C"
{
  void* getNet(void* network);
  void delNet(void* net_ptr);
  void inference(void* net_ptr, const void* in, void* out);
}

#endif