#include <string.h>
//#include <math.h>
extern double sqrt(double);
int a;
int main(int b, char** c) {
  int d;
  d = sqrt(b);
  memcpy(&d, &b, 2);
  return 0;
}
