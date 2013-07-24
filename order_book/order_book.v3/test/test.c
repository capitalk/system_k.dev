#include <math.h>
#include <stdio.h>
#include <time.h>



int main() {
  double tick = 0.32;
  int multiplier = 10;
  double x = tick*multiplier;
 fprintf(stderr, "%f", x); 
 double y = x;
 x = y-(int)x;
 if (x == 0.00) {
  fprintf(stderr, "%s\n", "ZERO"); 
 }
 else {
  fprintf(stderr, "%s\n", "NOT ZERO"); 
 }

 struct timespec st;
 struct timespec et;
 clock_gettime(CLOCK_REALTIME, &st);
 fprintf(stderr, "Clock begin: %d\n", clock());

 while (clock() < CLOCKS_PER_SEC) {
    fprintf(stderr, "%s", "|"); fprintf(stderr, "%c", '\b');
    fprintf(stderr, "%s", "/"); fprintf(stderr, "%c", '\b');
    fprintf(stderr, "%s", "-"); fprintf(stderr, "%c", '\b');
    fprintf(stderr, "%s", "\\"); fprintf(stderr, "%c", '\b');
 }
 fprintf(stderr, "Clock end  : %d\n", clock());
 clock_gettime(CLOCK_REALTIME, &et);

 fprintf(stderr, "Start time: %ld %ld\n", st.tv_sec, st.tv_nsec);
 fprintf(stderr, "End time  : %ld %ld\n", et.tv_sec, et.tv_nsec);
}
