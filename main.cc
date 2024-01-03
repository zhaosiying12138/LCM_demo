#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

class FlowGraph {
public:
  FlowGraph(int size) : _size(size + 2) {
    _edges = new int[_size * _size]();
    addEdge(0, 1);
    addEdge(size, size + 1);
  }

  ~FlowGraph() { delete _edges; }

  void addEdge(int u, int v) { _edges[u * _size + v] = 1; }

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

  std::vector<int> getDownSafety() const { return _downsafety; }

  static void printVector(std::vector<int> vec) {
    for (auto i : vec) {
      std::cout << i << ", ";
    }
    std::cout << "\n";
  }

  class DownSafety {
  public:
    DownSafety(FlowGraph &g) : _g(g), _size(g._size) {
      _used = new int[_size]();
      _killed = new int[_size]();
      _result = new int[_size];
    }

    ~DownSafety() {
      delete _used;
      delete _killed;
    }

    void setUsed(int u) { _used[u] = 1; }

    void setKilled(int u) { _killed[u] = 1; }

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
        int tmp_res_u = !_killed[tmp_u];

        for (auto tmp_v : _g.getSuccessors(tmp_u)) {
          if (_result[tmp_v] == -1) {
            continue;
          }
          tmp_res_u = tmp_res_u && _result[tmp_v];
        }

        tmp_res_u = tmp_res_u || _used[tmp_u];
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

      for (int i = 1; i < _size - 1; ++i) {
        if (_result[i] == 1) {
          _g._downsafety.push_back(i);
        }
      }
    }

  private:
    FlowGraph &_g;
    int _size;
    int *_used;
    int *_killed;
    int *_result;
  };

private:
  int _size;
  int *_edges;
  std::vector<int> _downsafety{};
};

int main() {
  std::cout << "Busy-Code-Motion implemented By zhaosiying12138@LiuYueCity "
               "Academy of Science!\n";

  FlowGraph g(18);
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

  std::cout << "Step 1: Compute Down-Safety\n";
  FlowGraph::DownSafety d_safe{g};
  d_safe.setUsed(3);
  d_safe.setUsed(10);
  d_safe.setUsed(15);
  d_safe.setUsed(16);
  d_safe.setUsed(17);
  d_safe.setKilled(2);
  d_safe.compute();
  std::cout << "[D-Safety Result]: ";
  FlowGraph::printVector(g.getDownSafety());

  return 0;
}