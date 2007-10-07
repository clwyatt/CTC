#include <iostream>
#include <cstdlib>
#include "StringtoDouble.h"

using namespace std;
double string2double(char * digit)
{
   
   int sign = 1;
   double result = 0;

   /* check sign */
   if (*digit == '-') {
      sign = -1;
      digit++;
   }

   /* get integer portion */
   while (*digit >= '0' && *digit <='9') {
      result = (result * 10) + *digit-'0';
      digit++;
   }

   /* get decimal point and fraction */
   if (*digit == '.') {
      digit++;
      double scale = 0.1;
      while (*digit >= '0' && *digit <='9') {
         result += (*digit-'0') * scale;
         scale *= 0.1;
         digit++;
      }
   }

   result = result * sign;
   return result;
}


