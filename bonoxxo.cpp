#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <memory>
#include <tuple>

using namespace std;

enum class CellValue { X, O, Empty };

string cellToString(CellValue v) {
  if (v == CellValue::O) {
    return "O";
  }
  if (v == CellValue::X) {
    return "X";
  }
  return " ";
}
class Solutions {
public:
  Solutions() = default;

  bool isEmpty() { return sols_.empty(); }

  void addSolution(pair<int, int> sol) { sols_.emplace_back(sol); }

  bool isSolution(pair<int, int> p) {
    return find(sols_.begin(), sols_.end(), p) != sols_.end();
  }

private:
  vector<pair<int, int>> sols_;
};
class Cell {
public:
  Cell() : c_(CellValue::Empty) {}

  Cell(CellValue c) : c_(c) {}

  // Cell(CellValue value): c_(value) {};
  CellValue getValue() const { return c_; }

  void setValue(CellValue c) { c_ = c; }

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
                   [](const shared_ptr<Cell> &c) { return c->getValue(); });
    return {values, is_line_};
  }

  void setValue(unsigned i, CellValue value) { line_[i]->setValue(value); }

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
    line_row_combined_.clear();
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

      line_row_combined_.emplace_back(i, true);
      line_row_combined_.emplace_back(i, false);
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
    for (auto &sol : solutions) {
      sol.setIsLine(input.isLine());
      if (isLineValidSolved(sol)) {
        valid_solutions.push_back(sol);
      }
    }

    return valid_solutions;
  }

  Solutions solveLineTrivial(unsigned line_nr, bool is_line) {
    Solutions sol;
    VLine line = lines_[line_nr].getVLine();
    BLine bline = lines_[line_nr];
    if (!is_line) {
      line = rows_[line_nr].getVLine();
      bline = rows_[line_nr];
    }

    auto addSol = [&sol, is_line, line_nr](unsigned other_nr) {
      if (is_line)
        sol.addSolution({line_nr, other_nr});
      else
        sol.addSolution({other_nr, line_nr});
    };

    // Check if only one is missing
    if (line.countE() == 1) {
      CellValue n = line.countO() > line.countX() ? CellValue::X : CellValue::O;
      for (int i = 0; i < line.size(); i++) {
        if (line.getValue(i) == CellValue::Empty) {
          bline.setValue(i, n);
          addSol(i);
          return sol;
        }
      }
    }

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
        addSol(i - 2);
      }

      if (prev == prevprev && line.getValue(i) == CellValue::Empty &&
          prev != CellValue::Empty) {
        if (prev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        bline.setValue(i, n);
        addSol(i);
      }

      if (line.getValue(i) == prevprev && prev == CellValue::Empty &&
          prevprev != CellValue::Empty) {
        if (prevprev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        bline.setValue(i - 1, n);
        addSol(i - 1);
      }

      prevprev = prev;
      prev = line.getValue(i);
    }

    return sol;
  }

  Solutions searchNonTrivialLineSolution(int line_nr, bool is_line) {
    Solutions sol;
    VLine line = lines_[line_nr].getVLine();
    BLine bline = lines_[line_nr];
    if (!is_line) {
      line = rows_[line_nr].getVLine();
      bline = rows_[line_nr];
    }

    auto addSol = [&sol, is_line, line_nr](unsigned other_nr) {
      if (is_line)
        sol.addSolution({line_nr, other_nr});
      else
        sol.addSolution({other_nr, line_nr});
    };

    if (line.isSolved())
      return sol;
    auto possible_sols = getValidSolutions(line);

    if (possible_sols.empty()) {
      return sol;
    }

    auto binary_op = [](VLine left, const VLine &right) {
      return (left & right);
    };
    VLine begin = possible_sols[0];
    VLine common = std::accumulate(possible_sols.begin(), possible_sols.end(),
                                   begin, binary_op);

    for (unsigned i = 0; i < line.size(); i++) {
      if (line.getValue(i) == CellValue::Empty &&
          common.getValue(i) != CellValue::Empty) {
        bline.setValue(i, common.getValue(i));
        {
          addSol(i);
          return sol;
        }
      }
    }
    return sol;
  }

  void print(Solutions highlight) {
    int i = 0;
    for (const auto &line : lines_) {
      int j = 0;
      for (auto &c : line.getVLine().getValues()) {
        if (highlight.isSolution({i, j}))
          cout << "\033[1;31m" << cellToString(c) << "\033[0m" << ' ';
        else
          cout << cellToString(c) << ' ';
        j++;
      }
      cout << endl;
      i++;
    }
    cout << "-----------------------" << endl;

  }

  vector<pair<int, bool>> getLinesAndRowsSorted()
  {

    auto sorter = [this](pair<int, bool> left, pair<int,bool> right)
    {
      VLine vleft = left.second ? lines_[left.first].getVLine() : rows_[left.first].getVLine();
      VLine vright = right.second ? lines_[right.first].getVLine() : rows_[right.first].getVLine();

      return vleft.countE() < vright.countE();
    };

    std::sort (line_row_combined_.begin(), line_row_combined_.end(), sorter);
    return line_row_combined_;
  }



private:
  vector<BLine> lines_;
  vector<BLine> rows_;

  vector<pair<int, bool>> line_row_combined_;


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

  Solutions sol;
  while (!b.solved()) {

    b.print(sol);
    for (int i = 0; i < b.getSize(); i++) {
      sol = b.solveLineTrivial(i, true);
      if (!sol.isEmpty()) {
        cout << "Trivial Line: " << i << endl;
        break;
      }
    }

    if (!sol.isEmpty())
      continue;

    for (int i = 0; i < b.getSize(); i++) {
      sol = b.solveLineTrivial(i, false);
      if (!sol.isEmpty()) {
        cout << "Trivial Row: " << i << endl;
        break;
      }
    }
    if (!sol.isEmpty())
      continue;


    vector<pair<int, bool>> linesAndRows = b.getLinesAndRowsSorted();
    for(auto p: linesAndRows)
    {
      sol = b.searchNonTrivialLineSolution(p.first, p.second);
      if (!sol.isEmpty()) {
        if(p.second)
        cout << "Non Trivial Line: " << p.first << endl;
        else
          cout << "Non Trivial Row: " << p.first << endl;
        break;
      }

    }
  }
  b.print(sol);
}
