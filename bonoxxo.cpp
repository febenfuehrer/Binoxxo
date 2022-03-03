#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

enum class CellValue { X, O, Empty };

class Cell {
public:
  Cell() : c_(CellValue::Empty) {}

  Cell(CellValue c) : c_(c) {}

  /*Cell& operator=(const Cell &other) {
    if (this == &other)
      return *this;

    this->c_ = other.getValue();
    return *this;
  }*/

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

class Binoxxo {

public:
  Binoxxo() = default;
  Binoxxo(vector<vector<char>> input) {
    for (auto line : input) {
      vector<Cell *> new_line;
      for (auto c : line) {
        if (c == 'X') {
          new_line.emplace_back(new Cell(CellValue::X));
          continue;
        }
        if (c == 'O') {
          new_line.emplace_back(new Cell(CellValue::O));
          continue;
        }
        new_line.emplace_back(new Cell(CellValue::Empty));
      }
      binoxxo.emplace_back(new_line);
    }
  }

  unsigned getSize() { return binoxxo.size(); }

  vector<Cell *> getLine(int line_nr) { return binoxxo[line_nr]; }

  vector<Cell *> getRow(int row_nr) {
    vector<Cell *> row = {};
    for (auto &i : binoxxo) {
      row.push_back(i[row_nr]);
    }
    return row;
  }

  bool solved() {
    for (auto line : binoxxo) {
      if (any_of(line.begin(), line.end(),
                 [](Cell *c) { return c->getValue() == CellValue::Empty; })) {
        return false;
      }
    }
    return true;
  }

  bool isLineValidSolved(vector<Cell> line, bool is_line) {
    unsigned c_empty = std::count_if(line.begin(), line.end(), [](Cell c) {
      return c.getValue() == CellValue::Empty;
    });
    unsigned c_x = std::count_if(line.begin(), line.end(), [](Cell c) {
      return c.getValue() == CellValue::X;
    });
    unsigned c_o = std::count_if(line.begin(), line.end(), [](Cell c) {
      return c.getValue() == CellValue::O;
    });
    if (c_empty > 0 || c_x != c_o) {
      return false;
    }

    CellValue prev = line[1].getValue();
    CellValue prevprev = line[0].getValue();
    for (int i = 2; i < line.size(); i++) {
      if (prev == prevprev && prev == line[i].getValue())
        return false;
      prevprev = prev;
      prev = line[i].getValue();
    }

    if (is_line) {
      for (unsigned i = 0; i < getSize(); i++) {

        auto check_line = getLine(i);
        unsigned c_empty =
            std::count_if(check_line.begin(), check_line.end(), [](Cell *c) {
              return c->getValue() == CellValue::Empty;
            });
        if (c_empty == 0) {
          bool equal = true;
          // compare
          for (int j = 0; j < check_line.size(); j++) {
            if (check_line[j]->getValue() != line[j].getValue()) {
              equal = false;
            }
          }
          if (equal) {
            return false;
          }
        }
      }
    } else {
      for (unsigned i = 0; i < getSize(); i++) {

        auto check_line = getRow(i);
        unsigned c_empty =
            std::count_if(check_line.begin(), check_line.end(), [](Cell *c) {
              return c->getValue() == CellValue::Empty;
            });
        if (c_empty == 0) {
          bool equal = true;
          // compare
          for (int j = 0; j < check_line.size(); j++) {
            if (check_line[j]->getValue() != line[j].getValue()) {
              equal = false;
            }
          }
          if (equal) {
            return false;
          }
        }
      }
    }
    return true;
  }

  vector<vector<CellValue>> getArrangements(int x_c, int o_c) {

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

  vector<vector<Cell>> getValidSolutions(vector<Cell *> input, bool is_line) {

    unsigned c_x = std::count_if(input.begin(), input.end(), [](Cell *c) {
      return c->getValue() == CellValue::X;
    });
    unsigned c_o = std::count_if(input.begin(), input.end(), [](Cell *c) {
      return c->getValue() == CellValue::O;
    });

    vector<vector<CellValue>> arrs =
        getArrangements(input.size() / 2 - c_x, input.size() / 2 - c_o);
    vector<vector<Cell>> solutions(arrs.size());
    unsigned arr_counter = 0;
    for (unsigned i = 0; i < input.size(); i++) {
      for (unsigned j = 0; j < arrs.size(); j++) {
        if (input[i]->getValue() == CellValue::Empty) {
          solutions[j].push_back(arrs[j][arr_counter]);
        } else {
          solutions[j].push_back(input[i]->getValue());
        }
      }
      if (input[i]->getValue() == CellValue::Empty) {
        arr_counter++;
      }
    }
    vector<vector<Cell>> valid_solutions;
    for (auto sol : solutions) {
      if (isLineValidSolved(sol, is_line)) {
        valid_solutions.push_back(sol);
      }
    }

    return valid_solutions;
  }

  bool solveLineTrivial(vector<Cell *> line) {
    bool found = false;
    CellValue prev = line[1]->getValue();
    CellValue prevprev = line[0]->getValue();
    CellValue n;

    for (int i = 2; i < line.size(); i++) {
      if (prevprev == CellValue::Empty && prev == line[i]->getValue() &&
          prev != CellValue::Empty) {
        if (prev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        line[i - 2]->setValue(n);
        found = true;
      }

      if (prev == prevprev && line[i]->getValue() == CellValue::Empty &&
          prev != CellValue::Empty) {
        if (prev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        line[i]->setValue(n);
        found = true;
      }

      if (line[i]->getValue() == prevprev && prev == CellValue::Empty &&
          prevprev != CellValue::Empty) {
        if (prevprev == CellValue::X)
          n = CellValue::O;
        else
          n = CellValue::X;
        line[i - 1]->setValue(n);
        found = true;
      }

      prevprev = prev;
      prev = line[i]->getValue();
    }

    return found;
  }

  bool searchNonTrivialLineSolution(vector<Cell *> line, bool is_line) {
    auto possible_sols = getValidSolutions(line, is_line);

    if (possible_sols.size() == 0) {
      return false;
    }

    auto binary_op = [](vector<Cell> left, vector<Cell> right) {
      vector<Cell> result(left.size());
      for (unsigned i = 0; i < left.size(); i++) {
        if (left[i].getValue() == right[i].getValue()) {
          result[i] = Cell(left[i]);
        } else {
          result[i] = Cell(CellValue::Empty);
        }
      }
      return result;
    };
    vector<Cell> begin = possible_sols[0];
    vector<Cell> common = std::accumulate(
        possible_sols.begin(), possible_sols.end(), begin, binary_op);

    for (unsigned i = 0; i < line.size(); i++) {
      if (line[i]->getValue() == CellValue::Empty &&
          common[i].getValue() != CellValue::Empty) {
        line[i]->setValue(common[i].getValue());
        { return true; }
      }
    }
    return false;
  }

  void print() {
    for (const auto &line : binoxxo) {
      for (auto c : line) {
        cout << c->toString() << ' ';
      }
      cout << endl;
    }
    cout << "-----------------------" << endl;
  }

private:
  vector<vector<Cell *>> binoxxo;
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

      if (b.solveLineTrivial(b.getLine(i))) {
        cout << "Trivial Line: " << i << endl;
        found = true;
        break;
      }
    }

    if (found)
      continue;

    for (int i = 0; i < b.getSize(); i++) {
      if (b.solveLineTrivial(b.getRow(i))) {
        cout << "Trivial Row: " << i << endl;
        found = true;
        break;
      }
    }
    if (found)
      continue;


    for (int i = 0; i < b.getSize(); i++) {
      if (b.searchNonTrivialLineSolution(b.getLine(i), true)) {
        cout << "Non Trivial Line: " << i << endl;
        found = true;
        break;
      }
    }
    if (found)
      continue;

    for (int i = 0; i < b.getSize(); i++) {
      if (b.searchNonTrivialLineSolution(b.getRow(i), false)) {
        cout << "Non Trivial Row: " << i << endl;
        break;
      }
    }
  }
  b.print();
}
