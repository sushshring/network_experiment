//
// Created by sps5394 on 11/7/18.
//
#include "ccommon.h"


double rand_expo(double lambda) {
  double u;
  u = rand() / (RAND_MAX + 1.0);
  return (-log(1- u)) / lambda;
}
