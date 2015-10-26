class Operation {
private:
  int point_m;
  int arrival_m;
  int quantity_m;
public:
  Operation(int point_p, int arrival_p, int quantity_p):
    point_m(point_p), arrival_m(arrival_p), quantity_m(quantity_p) {}
};