class Solution {
private:
  // To be exported.
  std::vector<Shift> shifts_m;

  // Qualitative data to check if this solution is admissible.
  bool is_admissible_m;
  std::vector< std::vector<double> > customers_content_m;
  
public:
  Solution(/* args */) = default;
};