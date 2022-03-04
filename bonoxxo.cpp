#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <memory>

using namespace std;

enum class CellValue { X, O, Empty };

class Cell {
public:
  Cell() : c_(CellValue::Empty) {}

  Cell(CellValue c) : c_(c) {}

  // Cell(CellValue value): c_(value) {};
  CellValue getValue() const { return c_; }

  void setValue(CellValue c) { c_ = c; }

  string toString() {
    if (c_ == CellValue::O) {
      return "O";
    }
    if (c_ == CellValue::X) {
      return "X";
    }
    return " ";
  }

private:
  CellValue c_;
};

class VLine {
public:
  VLine() = default;
  VLine(const std::vector<CellValue>& line, bool is_line) : is_line_(is_line) {
    values_.clear();
    values_.reserve(line.size());
    for (auto &l : line) {
      values_.emplace_back(l);
    }
  }

  unsigned countX() {
    return std::count_if(values_.begin(), values_.end(),
                         [](Cell c) { return c.getValue() == CellValue::X; });
  }

  unsigned countO() {
    return std::count_if(values_.begin(), values_.end(),
                         [](Cell c) { return c.getValue() == CellValue::O; });
  }

  unsigned countE() {
    return std::count_if(values_.begin(), values_.end(), [](Cell c) {
      return c.getValue() == CellValue::Empty;
    });
  }

  unsigned size() { return values_.size(); }

  bool isValid() {
    if (countO() != countX()) {
      return false;
    }

    CellValue prev = values_[1];
    CellValue prevprev = values_[0];
    for (int i = 2; i < values_.size(); i++) {
      if (prev == prevprev && prev == values_[i])
        return false;
      prevprev = prev;
      prev = values_[i];
    }
    return true;
  }

  bool isSolved() { return countE() == 0; }

  bool operator==(const VLine &lhs) { return lhs.getValues() == values_; }

  VLine operator&(const VLine &lhs) {
    VLine result{{}, is_line_};

    for (int i = 0; i < values_.size(); ++i) {
      if (values_[i] == lhs.getValues()[i]) {
        result.addValue(values_[i]);
      } else {
        result.addValue(CellValue::Empty);
      }
    }
    return result;
  }

  bool isLine() const { return is_line_; }

  vector<CellValue> getValues() const { return values_; }

  CellValue getValue(unsigned line_nr) { return values_[line_nr]; }

  void addValue(CellValue value) { values_.emplace_back(value); }

  void setIsLine(bool is_line) {
    is_line_ = is_line;
  }

private:
  vector<CellValue> values_;
  bool is_line_ = true;
};

class BLine {
public:
  BLine() = default;
  BLine(bool is_line) : is_line_(is_line){};

  void addValue(const shared_ptr<Cell>& c) { line_.emplace_back(c); }

  bool solved() {
    return !any_of(line_.begin(), line_.end(), [](const shared_ptr<Cell>& c) {
          return c->getValue() == CellValue::Empty;
        });
  }

  VLine getVLine() const {
    vector<CellValue> values;
    std::transform(line_.begin(), line_.end(), std::back_inserter(values),
                   [](const shared_ptr<Cell>& c) { return c->getValue(); });
    return {values, is_line_};
  }

  void setValue(unsigned i, CellValue value) { line_[i]->setValue(value); }

  void print() const {
    for(auto & c: line_)
    {
      cout << c->toString() << ' ';
    }
  }

  void setIsLine(bool is_line) {
    is_line_ = is_line;
  }

private:
  vector<std::shared_ptr<Cell>> line_;
  bool is_line_ = true;
};

class Binoxxo {

public:
  explicit Binoxxo(vector<vector<char>> input) {
    lines_.clear();
    rows_.clear();
    lines_ = vector<BLine>(input.size());
    rows_ = vector<BLine>(input.size());

    for (int i = 0; i < input.size(); i++) {
      for (int j = 0; j < input[i].size(); j++) {
        char c = input[i][j];
        CellValue value = CellValue::Empty;
        if (c == 'X') {
          value = CellValue::X;
        }
        if (c == 'O') {
          value = CellValue::O;
        }
        shared_ptr<Cell> p = make_shared<Cell>(value);
        lines_[i].addValue(p);
        rows_[j].addValue(p);

        lines_[i].setIsLine(true);
        rows_[j].setIsLine(false);
      }
    }
  }

  unsigned getSize() { return lines_.size(); }

  bool solved() {
    return (all_of(lines_.begin(), lines_.end(),
                   [](BLine l) { return l.solved(); }));
  }

  bool isLineValidSolved(VLine line) {

    bool complete = line.isValid() && line.isSolved();

    if (!complete)
      return false;

    if (line.isLine()) {
      return none_of(lines_.begin(), lines_.end(),
                     [line](const BLine& b) { return b.getVLine() == line; });
    } else {
      return none_of(rows_.begin(), rows_.end(),
                     [line](const BLine& b) { return b.getVLine() == line; });
    }
  }

  vector<vector<CellValue>> getArrangements(unsigned x_c, unsigned o_c) {

    if (x_c == 1 && o_c == 0) {
      return {{CellValue::X}};
    }
    if (x_c == 0 && o_c == 1) {
      return {{CellValue::O}};
    }
    vector<vector<CellValue>> all_arrs;
    if (x_c >= 1) {
      vector<vector<CellValue>> smaller_arr = getArrangements(x_c - 1, o_c);
      for (auto &small_arr : smaller_arr) {
        small_arr.insert(small_arr.begin(), CellValue::X);
        all_arrs.push_back(small_arr);
      }
    }
    if (o_c >= 1) {
      vector<vector<CellValue>> smaller_arr = getArrangements(x_c, o_c - 1);
      for (auto &small_arr : smaller_arr) {

        small_arr.insert(small_arr.begin(), CellValue::O);
        all_arrs.push_back(small_arr);
      }
    }

    return all_arrs;
  }

  vector<VLine> getValidSolutions(VLine input) {

    unsigned c_x = input.countX();
    unsigned c_o = input.countO();

    vector<vector<CellValue>> arrs =
        getArrangements(unsigned(input.size() / 2 - c_x), unsigned(input.size() / 2 - c_o));

    vector<VLine> solutions(arrs.size());
    unsigned arr_counter = 0;
    for (unsigned i = 0; i < input.size(); i++) {
      for (unsigned j = 0; j < arrs.size(); j++) {
        if (input.getValue(i) == CellValue::Empty) {
          solutions[j].addValue(arrs[j][arr_counter]);
        } else {
          solutions[j].addValue(input.getValue(i));
        }
      }
      if (input.getValue(i) == CellValue::Empty) {
        arr_counter++;
      }
    }
    vector<VLine> valid_solutions;
    for (auto& sol : solutions) {
      sol.setIsLine(input.isLine());
      if (isLineValidSolved(sol)) {
        valid_solutions.push_back(sol);
      }
    }

    return valid_solutions;
  }

  bool solveLineTrivial(unsigned line_nr, bool is_line) {
    VLine line = lines_[line_nr].getVLine();
    BLine bline = lines_[line_nr];
    if (!is_line) {
      line = rows_[line_nr].getVLine();
      bline = rows_[line_nr];
    }

    bool found = false;
    CellValue prev = line.getValue(1);
    CellValue prevprev = line.getValue(0);
    CellValue n;

    for (int i = 2; i < line.size(); i++) {
      if (prevprev == CellValue::Empty && prev == line.getValue(i) &&
          prev != CellValue::Empty) {
        if (prev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        bline.setValue(i - 2, n);
        found = true;
      }

      if (prev == prevprev && line.getValue(i) == CellValue::Empty &&
          prev != CellValue::Empty) {
        if (prev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        bline.setValue(i, n);
        found = true;
      }

      if (line.getValue(i) == prevprev && prev == CellValue::Empty &&
          prevprev != CellValue::Empty) {
        if (prevprev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        bline.setValue(i - 1, n);
        found = true;
      }

      prevprev = prev;
      prev = line.getValue(i);
    }

    return found;
  }

  bool searchNonTrivialLineSolution(int line_nr, bool is_line) {
    VLine line = lines_[line_nr].getVLine();
    BLine bline = lines_[line_nr];
    if (!is_line) {
      line = rows_[line_nr].getVLine();
      bline = rows_[line_nr];
    }

    auto possible_sols = getValidSolutions(line);

    if (possible_sols.empty()) {
      return false;
    }

    auto binary_op = [](VLine left, const VLine& right) { return (left & right); };
    VLine begin = possible_sols[0];
    VLine common = std::accumulate(possible_sols.begin(), possible_sols.end(),
                                   begin, binary_op);

    for (unsigned i = 0; i < line.size(); i++) {
      if (line.getValue(i) == CellValue::Empty &&
          common.getValue(i) != CellValue::Empty) {
        bline.setValue(i, common.getValue(i));
        { return true; }
      }
    }
    return false;
  }

  void print() {
    for (const auto &line : lines_) {
     line.print();
     cout << endl;
    }
    cout << "-----------------------" << endl;
  }

private:
  vector<BLine> lines_;
  vector<BLine> rows_;
};

int main() {

  Binoxxo b({{'_', '_', '_', '_', '_', '_', '_', '_', '_', 'X', '_', '_'},
             {'X', '_', '_', '_', '_', 'O', 'O', '_', '_', '_', '_', 'O'},
             {'X', '_', '_', 'O', '_', '_', '_', '_', '_', 'O', '_', '_'},
             {'_', '_', '_', 'O', '_', '_', '_', '_', '_', '_', '_', '_'},
             {'O', 'O', '_', '_', '_', '_', '_', '_', 'O', '_', '_', '_'},
             {'_', '_', '_', 'X', '_', '_', 'X', '_', '_', '_', '_', '_'},
             {'_', '_', '_', '_', 'O', '_', '_', '_', '_', '_', '_', '_'},
             {'_', '_', '_', '_', '_', '_', '_', '_', '_', 'O', '_', '_'},
             {'_', '_', 'X', '_', '_', 'O', '_', '_', '_', 'O', '_', '_'},
             {'_', '_', 'X', '_', '_', '_', 'X', '_', '_', '_', '_', '_'},
             {'O', '_', '_', '_', '_', '_', '_', '_', 'O', '_', '_', '_'},
             {'_', 'X', 'X', '_', '_', 'X', 'X', '_', '_', '_', 'O', '_'}});

  while (!b.solved()) {

    bool found = false;

    b.print();

    for (int i = 0; i < b.getSize(); i++) {

      if (b.solveLineTrivial(i, true)) {
        cout << "Trivial Line: " << i << endl;
        found = true;
        break;
      }
    }

    if (found)
      continue;

    for (int i = 0; i < b.getSize(); i++) {
      if (b.solveLineTrivial(i, false)) {
        cout << "Trivial Row: " << i << endl;
        found = true;
        break;
      }
    }
    if (found)
      continue;


    for (int i = 0; i < b.getSize(); i++) {
      if (b.searchNonTrivialLineSolution(i, true)) {
        cout << "Non Trivial Line: " << i << endl;
        found = true;
        break;
      }
    }
    if (found)
      continue;

    for (int i = 0; i < b.getSize(); i++) {
      if (b.searchNonTrivialLineSolution(i, false)) {
        cout << "Non Trivial Row: " << i << endl;
        break;
      }
    }
  }
  b.print();
}
