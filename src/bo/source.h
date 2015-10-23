/** @file */

#ifndef SOURCE_H
#define SOURCE_H

class Source {
private:
  int index_m;
  int setupTime_m;


public:
  Source(/* args */) = default;

  // index
  void index(int index_p) {index_m = index_p;}
  int index() {return index_m;}

  // setupTime
  void setupTime(int setupTime_p) {setupTime_m = setupTime_p;}
  int setupTime() {return setupTime_m;}

};

#endif // SOURCE_H