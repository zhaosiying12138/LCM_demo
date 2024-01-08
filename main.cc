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
  }

  ~FlowGraph() {
    delete _edges;
    delete _used;
    delete _killed;
    delete _downsafety;
    delete _earliestness;
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

  void printVector(int *vec) const {
    for (int i = 1; i < _size - 1; ++i) {
      if (vec[i] == 1) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";
  }

  void getPlacement() {
    std::cout << "[Get Placement]: ";
    for (int i = 0; i < _size - 1; ++i) {
      if ((_downsafety[i] == 1) && (_earliestness[i] == 1)) {
        std::cout << i << ", ";
      }
    }
    std::cout << "\n";
  }

  class DownSafety {
  public:
    DownSafety(FlowGraph &g) : _g(g), _size(g._size), _result(g._downsafety) {}

    ~DownSafety() {}

    void compute() {
      for (int i = 0; i < _size - 1; ++i) {
        _result[i] = -1;
      }
      _result[_size - 1] = 0;

      std::queue<int> worklist{};
      for (auto tmp_u : _g.getPrecessors(_size - 1)) {
        worklist.push(tmp_u);
      }

      while (!worklist.empty()) {
        int tmp_u = worklist.front();
        worklist.pop();
        int tmp_res_u = !_g._killed[tmp_u];

        for (auto tmp_v : _g.getSuccessors(tmp_u)) {
          if (_result[tmp_v] == -1) {
            continue;
          }
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
        _result[i] = -1;
      }
      _result[0] = 1;

      std::queue<int> worklist{};
      for (auto tmp_v : _g.getSuccessors(0)) {
        worklist.push(tmp_v);
      }

      while (!worklist.empty()) {
        int tmp_v = worklist.front();
        worklist.pop();
        int tmp_res_v = 0;

        for (auto tmp_u : _g.getPrecessors(tmp_v)) {
          if (_result[tmp_u] == -1) {
            continue;
          }
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

private:
  int _size;
  int *_edges;
  int *_used;
  int *_killed;
  int *_downsafety;
  int *_earliestness;
};

int main() {
  std::cout << "Busy-Code-Motion implemented By zhaosiying12138@LiuYueCity "
               "Academy of Sciences!\n";

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

  g.getPlacement();

  return 0;
}