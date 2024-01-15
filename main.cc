#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

class FlowGraph {
public:
  FlowGraph(int size) : _size(size + 2) {
    _edges = new int[_size * _size]();
    _used = new int[_size]();
    _killed = new int[_size]();
    _downsafety = new int[_size];
    _earliestness = new int[_size];
    _delay = new int[_size];
    _latest = new int[_size]();
    _isolated = new int[_size];
  }

  ~FlowGraph() {
    delete _edges;
    delete _used;
    delete _killed;
    delete _downsafety;
    delete _earliestness;
    delete _delay;
    delete _latest;
    delete _isolated;
  }

  void addEdge(int u, int v) { _edges[u * _size + v] = 1; }

  void setUsed(int u) { _used[u] = 1; }

  void setKilled(int u) { _killed[u] = 1; }

  std::vector<int> getPrecessors(int v) const {
    std::vector<int> res;
    for (int u = 0; u < _size; ++u) {
      if (_edges[u * _size + v] == 1) {
        res.emplace_back(u);
      }
    }
    return res;
  }

  std::vector<int> getSuccessors(int u) const {
    std::vector<int> res;
    for (int v = 0; v < _size; ++v) {
      if (_edges[u * _size + v] == 1) {
        res.emplace_back(v);
      }
    }
    return res;
  }

  int *getDownSafety() const { return _downsafety; }

  int *getEarliestness() const { return _earliestness; }

  int *getDelay() const { return _delay; }

  int *getLatest() const { return _latest; }

  int *getIsolated() const { return _isolated; }

  void printVector(int *vec) const {
    for (int i = 1; i < _size - 1; ++i) {
      if (vec[i] == 1) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";
  }

  void getPlacementBCM() {
    std::cout << "[Get Placement of BCM]: ";
    for (int i = 1; i < _size - 1; ++i) {
      if ((_downsafety[i] == 1) && (_earliestness[i] == 1)) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";
  }

  void getPlacementLCM() {
    std::cout << "[Get Placement of LCM]:\n";
    std::cout << "[Optimal Computation Points]: ";
    for (int i = 1; i < _size - 1; ++i) {
      if ((_latest[i] == 1) && (_isolated[i] == 0)) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";

    std::cout << "[Isolated Computation]: ";
    for (int i = 1; i < _size - 1; ++i) {
      if ((_latest[i] == 1) && (_isolated[i] == 1)) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";

    std::cout << "[Redundant Occurence]: ";
    for (int i = 1; i < _size - 1; ++i) {
      if ((_used[i] == 1) && !((_latest[i] == 1) && (_isolated[i] == 1))) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";
  }

  void drawBCM(std::string Filepath, int isPlaced) {
    std::ofstream dotOuts;
    dotOuts.open(Filepath, std::ios::out | std::ios::trunc);

    dotOuts << "digraph G {\n";
    dotOuts << "\tnode[shape=box;];\n";
    dotOuts << "\tedge[arrowhead=open;];\n";
    dotOuts << "\n";

    for (int i = 1; i < _size - 1; i++) {
      drawNodesBCM(dotOuts, i, _used[i], _killed[i], isPlaced, _downsafety[i],
                   _earliestness[i]);
    }
    dotOuts << "\n";
    for (int i = 1; i < _size - 1; i++) {
      for (int j = 1; j < _size - 1; j++) {
        if (_edges[i * _size + j] == 1) {
          drawEdges(dotOuts, i, j);
        }
      }
    }
    dotOuts << "}\n";

    dotOuts.close();
  }

  void drawALCM(std::string Filepath, int isPlaced) {
    std::ofstream dotOuts;
    dotOuts.open(Filepath, std::ios::out | std::ios::trunc);

    dotOuts << "digraph G {\n";
    dotOuts << "\tnode[shape=box;];\n";
    dotOuts << "\tedge[arrowhead=open;];\n";
    dotOuts << "\n";

    for (int i = 1; i < _size - 1; i++) {
      drawNodesALCM(dotOuts, i, _used[i], _killed[i], isPlaced, _delay[i],
                    _latest[i]);
    }
    dotOuts << "\n";
    for (int i = 1; i < _size - 1; i++) {
      for (int j = 1; j < _size - 1; j++) {
        if (_edges[i * _size + j] == 1) {
          drawEdges(dotOuts, i, j);
        }
      }
    }
    dotOuts << "}\n";

    dotOuts.close();
  }

  void drawLCM(std::string Filepath) {
    std::ofstream dotOuts;
    dotOuts.open(Filepath, std::ios::out | std::ios::trunc);

    dotOuts << "digraph G {\n";
    dotOuts << "\tnode[shape=box; color=black;];\n";
    dotOuts << "\tedge[arrowhead=open;];\n";
    dotOuts << "\n";

    for (int i = 1; i < _size - 1; i++) {
      drawNodesLCM(dotOuts, i, _killed[i], (_latest[i] && (!_isolated[i])),
                   (_latest[i] && _isolated[i]),
                   (_used[i] && !(_latest[i] && _isolated[i])));
    }
    dotOuts << "\n";
    for (int i = 1; i < _size - 1; i++) {
      for (int j = 1; j < _size - 1; j++) {
        if (_edges[i * _size + j] == 1) {
          drawEdges(dotOuts, i, j);
        }
      }
    }
    dotOuts << "}\n";

    dotOuts.close();
  }

  class DownSafety {
  public:
    DownSafety(FlowGraph &g) : _g(g), _size(g._size), _result(g._downsafety) {}

    ~DownSafety() {}

    void compute() {
      for (int i = 0; i < _size - 1; ++i) {
        _result[i] = 1;
      }
      _result[_size - 1] = 0;

      std::queue<int> worklist{};
      for (int i = 0; i < _size - 1; ++i) {
        worklist.push(i);
      }

      while (!worklist.empty()) {
        int tmp_u = worklist.front();
        worklist.pop();
        int tmp_res_u = !_g._killed[tmp_u];

        for (auto tmp_v : _g.getSuccessors(tmp_u)) {
          tmp_res_u = tmp_res_u && _result[tmp_v];
        }

        tmp_res_u = tmp_res_u || _g._used[tmp_u];
        if (tmp_res_u != _result[tmp_u]) {
          std::cout << "[Update] D-Safe[" << tmp_u
                    << "] := " << ((tmp_res_u == 1) ? "True" : "False") << "\n";
          _result[tmp_u] = tmp_res_u;
          for (auto tmp_u_prec : _g.getPrecessors(tmp_u)) {
            worklist.push(tmp_u_prec);
            // std::cout << "[Debug] Add to worklist: " << tmp_u_prec << "\n";
          }
        }
      }
    }

  private:
    FlowGraph &_g;
    int _size;
    int *_result;
  };

  class Earliestness {
  public:
    Earliestness(FlowGraph &g)
        : _g(g), _size(g._size), _result(g._earliestness) {}

    ~Earliestness() {}

    void compute() {
      for (int i = 1; i < _size; ++i) {
        _result[i] = 0;
      }
      _result[0] = 1;

      std::queue<int> worklist{};
      for (int i = _size - 2; i > 0; --i) {
        worklist.push(i);
      }

      while (!worklist.empty()) {
        int tmp_v = worklist.front();
        worklist.pop();
        int tmp_res_v = 0;

        for (auto tmp_u : _g.getPrecessors(tmp_v)) {
          int tmp =
              (!_g._downsafety[tmp_u] && _result[tmp_u]) || (_g._killed[tmp_u]);
          tmp_res_v = tmp_res_v || tmp;
        }
        if (tmp_res_v != _result[tmp_v]) {
          std::cout << "[Update] Earliestness[" << tmp_v
                    << "] := " << ((tmp_res_v == 1) ? "True" : "False") << "\n";
          _result[tmp_v] = tmp_res_v;
          for (auto tmp_v_succ : _g.getSuccessors(tmp_v)) {
            worklist.push(tmp_v_succ);
            // std::cout << "[Debug] Add to worklist: " << tmp_u_prec << "\n";
          }
        }
      }
    }

  private:
    FlowGraph &_g;
    int _size;
    int *_result;
  };

  class DelayLatest {
  public:
    DelayLatest(FlowGraph &g) : _g(g), _size(g._size), _result(g._delay) {}

    ~DelayLatest() {}

    void compute() {
      for (int i = 1; i < _size; ++i) {
        _result[i] = 1;
      }
      _result[0] = 0;

      std::queue<int> worklist{};
      for (int i = _size - 2; i > 0; --i) {
        worklist.push(i);
      }

      while (!worklist.empty()) {
        int tmp_v = worklist.front();
        worklist.pop();
        int tmp_res_v = 1;

        for (auto tmp_u : _g.getPrecessors(tmp_v)) {
          int tmp = (!_g._used[tmp_u]) && _result[tmp_u];
          tmp_res_v = tmp_res_v && tmp;
        }
        tmp_res_v =
            tmp_res_v || (_g._downsafety[tmp_v] && _g._earliestness[tmp_v]);

        if (tmp_res_v != _result[tmp_v]) {
          std::cout << "[Update] Delay[" << tmp_v
                    << "] := " << ((tmp_res_v == 1) ? "True" : "False") << "\n";
          _result[tmp_v] = tmp_res_v;

          for (auto tmp_v_succ : _g.getSuccessors(tmp_v)) {
            worklist.push(tmp_v_succ);
            // std::cout << "[Debug] Add to worklist: " << tmp_u_prec << "\n";
          }
        }
      }

      for (int i = 1; i < _size - 1; ++i) {
        int tmp_res_delay_succ = 1;
        for (auto tmp_v : _g.getSuccessors(i)) {
          tmp_res_delay_succ = tmp_res_delay_succ && _g._delay[tmp_v];
        }

        _g._latest[i] = _result[i] && (_g._used[i] || (!tmp_res_delay_succ));
      }
    }

  private:
    FlowGraph &_g;
    int _size;
    int *_result;
  };

  class Isolated {
  public:
    Isolated(FlowGraph &g) : _g(g), _size(g._size), _result(g._isolated) {}

    ~Isolated() {}

    void compute() {
      for (int i = 0; i < _size - 1; ++i) {
        _result[i] = 1;
      }
      // It should be True at the exit node to have the greatest solution!
      _result[_size - 1] = 1;

      std::queue<int> worklist{};
      for (int i = 0; i < _size - 1; ++i) {
        worklist.push(i);
      }

      while (!worklist.empty()) {
        int tmp_u = worklist.front();
        worklist.pop();
        int tmp_res_u = 1;

        for (auto tmp_v : _g.getSuccessors(tmp_u)) {
          int tmp_res_v =
              _g._latest[tmp_v] || ((!_g._used[tmp_v]) && _result[tmp_v]);
          tmp_res_u = tmp_res_u && tmp_res_v;
        }

        if (tmp_res_u != _result[tmp_u]) {
          std::cout << "[Update] Isolated[" << tmp_u
                    << "] := " << ((tmp_res_u == 1) ? "True" : "False") << "\n";
          _result[tmp_u] = tmp_res_u;
          for (auto tmp_u_prec : _g.getPrecessors(tmp_u)) {
            worklist.push(tmp_u_prec);
            // std::cout << "[Debug] Add to worklist: " << tmp_u_prec << "\n";
          }
        }
      }
    }

  private:
    FlowGraph &_g;
    int _size;
    int *_result;
  };

private:
  int _size;
  int *_edges;
  int *_used;
  int *_killed;
  int *_downsafety;
  int *_earliestness;
  int *_delay;
  int *_latest;
  int *_isolated;

  void drawNodesBCM(std::ofstream &dotOuts, int i, int isUsed, int isKilled,
                    int isPlaced, int isSafety, int isEarliest) {
    dotOuts << "\t"
            << "BB" << i << " [label=\"";

    if (isPlaced && isSafety && isEarliest) {
      dotOuts << "h := x + y;\\n";
    }
    if (isUsed) {
      if (isPlaced) {
        dotOuts << "... := h;\\n";
      } else {
        dotOuts << "... := x + y;\\n";
      }
    }
    if (isKilled) {
      dotOuts << "x := ...;\\n";
    }
    dotOuts << "\"; "
            << "xlabel=\"BB" << i << ":\";";
    if (isPlaced && isSafety) {
      dotOuts << " color=Turquoise;";
    }
    if (isPlaced && isEarliest) {
      dotOuts << " style=filled;";
    }
    dotOuts << "];\n";
  }

  void drawNodesALCM(std::ofstream &dotOuts, int i, int isUsed, int isKilled,
                     int isPlaced, int isDelay, int isLatest) {
    dotOuts << "\t"
            << "BB" << i << " [label=\"";

    if (isPlaced && isLatest) {
      dotOuts << "h := x + y;\\n";
    }
    if (isUsed) {
      if (isPlaced) {
        dotOuts << "... := h;\\n";
      } else {
        dotOuts << "... := x + y;\\n";
      }
    }
    if (isKilled) {
      dotOuts << "x := ...;\\n";
    }
    dotOuts << "\"; "
            << "xlabel=\"BB" << i << ":\";";
    if (isPlaced && isDelay) {
      dotOuts << " style=filled;";
    }
    if (isPlaced && isLatest) {
      dotOuts << " color=yellow;";
    }
    dotOuts << "];\n";
  }

  void drawNodesLCM(std::ofstream &dotOuts, int i, int isKilled, int isOCP,
                    int isIC, int isRO) {
    dotOuts << "\t"
            << "BB" << i << " [label=\"";

    if (isOCP) {
      dotOuts << "h := x + y;\\n";
    } else if (isIC) {
      dotOuts << "... := x + y;\\n";
    }
    if (isRO) {
      dotOuts << "... := h;\\n";
    }
    if (isKilled) {
      dotOuts << "x := ...;\\n";
    }
    dotOuts << "\"; "
            << "xlabel=\"BB" << i << ":\";";

    if (isOCP && !isRO) {
      dotOuts << " fillcolor=yellow; style=filled;";
    } else if (isIC) {
      dotOuts << " fillcolor=pink; style=filled;";
    }
    if (isKilled) {
      dotOuts << " fillcolor=cornsilk; style=filled;";
    }
    if (isRO) {
      if (!isOCP) {
        dotOuts << " fillcolor=darkseagreen3; style=filled;";
      } else {
        dotOuts << " fillcolor=\"yellow:darkseagreen3\"; style=filled;";
      }
    }

    dotOuts << "];\n";
  }

  void drawEdges(std::ofstream &dotOuts, int i, int j) {
    dotOuts << "\t"
            << "BB" << i << "->BB" << j << ";\n";
  }
};

// Original Paper Demo
void test1() {
  FlowGraph g(18);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(3, 5);
  g.addEdge(4, 5);
  g.addEdge(5, 6);
  g.addEdge(5, 7);
  g.addEdge(6, 8);
  g.addEdge(6, 9);
  g.addEdge(7, 18);
  g.addEdge(8, 11);
  g.addEdge(9, 12);
  g.addEdge(10, 11);
  g.addEdge(11, 10);
  g.addEdge(11, 14);
  g.addEdge(12, 13);
  g.addEdge(12, 15);
  g.addEdge(12, 17);
  g.addEdge(13, 12);
  g.addEdge(14, 16);
  g.addEdge(15, 16);
  g.addEdge(16, 18);
  g.addEdge(17, 18);
  g.addEdge(18, 19); // exit node 's edges

  g.setUsed(3);
  g.setUsed(10);
  g.setUsed(15);
  g.setUsed(16);
  g.setUsed(17);
  g.setKilled(2);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo1_before.dot", 0);
  g.drawBCM("demo1_after.dot", 1);
}

// PRE & Computional Optimal
void test2() {
  FlowGraph g(6);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(4, 5);
  g.addEdge(3, 6);
  g.addEdge(5, 6);
  g.addEdge(6, 7); // exit node 's edges

  g.setUsed(3);
  g.setUsed(6);
  g.setKilled(2);
  g.setKilled(4);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo2_before.dot", 0);
  g.drawBCM("demo2_after.dot", 1);
}

// Safety
void test3() {
  FlowGraph g(7);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(4, 5);
  g.addEdge(3, 6);
  g.addEdge(5, 6);
  g.addEdge(5, 7);
  g.addEdge(6, 8); // exit node 's edges
  g.addEdge(7, 8); // exit node 's edges

  g.setUsed(3);
  g.setUsed(6);
  g.setKilled(2);
  g.setKilled(4);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo3_before.dot", 0);
  g.drawBCM("demo3_after.dot", 1);
}

// FRE of test2
void test4() {
  FlowGraph g(6);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(4, 5);
  g.addEdge(3, 6);
  g.addEdge(5, 6);
  g.addEdge(6, 7); // exit node 's edges

  g.setUsed(3);
  g.setUsed(5);
  g.setUsed(6);
  g.setKilled(2);
  g.setKilled(4);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo4_before.dot", 0);
  g.drawBCM("demo4_after.dot", 1);
}

// PRE: Loop Invariant
void test5() {
  FlowGraph g(4);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(2, 3);
  g.addEdge(3, 3);
  g.addEdge(3, 4);
  g.addEdge(4, 5); // exit node 's edges

  g.setUsed(3);
  g.setKilled(1);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo5_before.dot", 0);
  g.drawBCM("demo5_after.dot", 1);
}

// FRE: Loop Invariant
void test6() {
  FlowGraph g(4);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(2, 3);
  g.addEdge(3, 3);
  g.addEdge(3, 4);
  g.addEdge(4, 5); // exit node 's edges

  g.setUsed(2);
  g.setUsed(3);
  g.setKilled(1);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo6_before.dot", 0);
  g.drawBCM("demo6_after.dot", 1);
}

// Greatest solution for down-safety, the same as test1()
void test7() {}

// Safety & Split critical edges
void test8() {
  FlowGraph g(8);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(4, 5);
  g.addEdge(3, 6);
  g.addEdge(5, 8);
  g.addEdge(8, 6);
  g.addEdge(5, 7);
  g.addEdge(6, 9); // exit node 's edges
  g.addEdge(7, 9); // exit node 's edges

  g.setUsed(3);
  g.setUsed(6);
  g.setKilled(2);
  g.setKilled(4);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo8_before.dot", 0);
  g.drawBCM("demo8_after.dot", 1);
}

// Original Paper Demo
void test9() {
  FlowGraph g(18);

  g.addEdge(0, 1); // entry node 's edges
  g.addEdge(1, 2);
  g.addEdge(1, 4);
  g.addEdge(2, 3);
  g.addEdge(3, 5);
  g.addEdge(4, 5);
  g.addEdge(5, 6);
  g.addEdge(5, 7);
  g.addEdge(6, 8);
  g.addEdge(6, 9);
  g.addEdge(7, 18);
  g.addEdge(8, 11);
  g.addEdge(9, 12);
  g.addEdge(10, 11);
  g.addEdge(11, 10);
  g.addEdge(11, 14);
  g.addEdge(12, 13);
  g.addEdge(12, 15);
  g.addEdge(12, 17);
  g.addEdge(13, 12);
  g.addEdge(14, 16);
  g.addEdge(15, 16);
  g.addEdge(16, 18);
  g.addEdge(17, 18);
  g.addEdge(18, 19); // exit node 's edges

  g.setUsed(3);
  g.setUsed(10);
  g.setUsed(15);
  g.setUsed(16);
  g.setUsed(17);
  g.setKilled(2);

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  g.printVector(g.getDownSafety());

  std::cout << "\nStep 2: Compute Earliestness\n";
  FlowGraph::Earliestness early{g};
  early.compute();
  std::cout << "[Earliestness Result]: ";
  g.printVector(g.getEarliestness());

  g.getPlacementBCM();
  g.drawBCM("demo9_bcm_before.dot", 0);
  g.drawBCM("demo9_bcm_after.dot", 1);

  std::cout << "\nStep 3: Compute Delay & Latest\n";
  FlowGraph::DelayLatest delay{g};
  delay.compute();
  std::cout << "[Delay Result]: ";
  g.printVector(g.getDelay());
  std::cout << "[Latest Result]: ";
  g.printVector(g.getLatest());

  g.drawALCM("demo9_alcm_after.dot", 1);

  std::cout << "\nStep 4: Compute Isolated\n";
  FlowGraph::Isolated isolated{g};
  isolated.compute();
  std::cout << "[Isolated Result]: ";
  g.printVector(g.getIsolated());

  std::cout << "\n";
  g.getPlacementLCM();
  g.drawLCM("demo9_lcm.dot");
}

int main() {
  std::cout << "Lazy-Code-Motion implemented By zhaosiying12138@LiuYueCity "
               "Academy of Sciences!\n";
  test9();

  return 0;
}