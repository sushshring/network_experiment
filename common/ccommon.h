//
// Created by sps5394 on 11/7/18.
//

#ifndef CLIENT_CCOMMON_H
#define CLIENT_CCOMMON_H

#include <stdlib.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//
// Function     : rand_expo
// Description  : Generates random numbers (with limited randomness) based on an
//                exponential distribution
//
// Inputs       : lambda: lambda factor for exponential dist
// Outputs      : next random double
double rand_expo(double lambda);
#endif //CLIENT_CCOMMON_H
