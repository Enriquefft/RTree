#ifndef RTREE_H
#define RTREE_H

#include "MBB.h"
#include <optional>
#include <vector>

template <std::floating_point T> class RTree;

template <std::floating_point T = float> class RNode {
private:
  MBB<T> boundingBox;
  std::vector<Point<T>> points;  // Only used if it is a leaf node
  std::vector<RNode *> children; // Only used if it is not a leaf node
  RNode *parent;
  size_t minChildren;
  size_t maxChildren;
  ~RNode();
  // copy,copy assignment, move, move assignment
  // RNode(const RNode &other);
  // auto operator=(const RNode &other) -> RNode &;
  // RNode(RNode &&other) noexcept;
  // auto operator=(RNode &&other) noexcept -> RNode &;

  auto chooseSubtree(const Point<T> &point) -> RNode *;
  auto split() -> std::pair<RNode<T> *, RNode<T> *>;
  auto pickSeedsQuadratic() -> std::pair<Point<T>, Point<T>>;
  auto pickInternalSeedsQuadratic() -> std::pair<RNode *, RNode *>;
  auto calculateLinearCost(Point<T>, std::vector<Point<T>> &)
      -> std::pair<NType, NType>;
  auto calculateQuadraticCost(RNode *, std::vector<RNode *> &)
      -> std::pair<NType, NType>;

  void updateBoundingBox();

  void adjustTree(RNode<T> *n, std::vector<RNode<T> *> &eliminated);
  auto findLeaf(const Point<T> &point) -> RNode<T> *;
  void condenseTree(RNode<T> *n, std::vector<RNode<T> *> &eliminated);

public:
  friend class RTree<T>;
  bool isLeaf;

  explicit RNode(size_t _minChildren = 3, size_t _maxChildren = 2,
                 bool _isLeaf = true)
      : parent(nullptr), minChildren(_minChildren), maxChildren(_maxChildren),
        isLeaf(_isLeaf) {}

  RNode() : parent(nullptr), minChildren(2), maxChildren(3), isLeaf(true) {}

  auto search(const Point<T> &point) -> bool;
  auto insert(const Point<T> &point)
      -> std::optional<std::pair<RNode<T> *, RNode<T> *>>;
  auto query(const QueryBox<T> &q) -> std::vector<Point<T>>;
  void remove(const Point<T> &point, std::vector<RNode *> &eliminated);

  [[nodiscard]] auto geChild(size_t i) const -> RNode * { return children[i]; }
  [[nodiscard]] auto getPoint(size_t i) const -> Point<T> { return points[i]; }

  [[nodiscard]] auto getChildren() const -> std::vector<RNode *> {
    return children;
  }
  [[nodiscard]] auto getPoints() const -> std::vector<Point<T>> {
    return points;
  }
  [[nodiscard]] auto getParent() const -> RNode * { return parent; }
  [[nodiscard]] auto getBoundingBox() const -> MBB<T> { return boundingBox; }
  void print(size_t depth) const;
};

template <std::floating_point T = float> class RTree {
private:
  RNode<T> *root;
  uint minChildren;
  uint maxChildren;

public:
  RTree(uint _minChildren, uint _maxChildren)
      : root(new RNode<T>(_minChildren, _maxChildren, true)),
        minChildren(_minChildren), maxChildren(_maxChildren) {}

  RTree() : root(new RNode<T>(2, 3, true)), minChildren(2), maxChildren(3) {}
  ~RTree() { delete root; }

  auto search(const Point<T> &point) -> bool;
  void insert(const Point<T> &point);
  void remove(const Point<T> &point);
  auto query(const QueryBox<T> &q) -> std::vector<Point<T>>;

  [[nodiscard]] auto getRoot() const -> RNode<T> * { return root; }
  void print() const;
};

extern template class RNode<float>;
extern template class RTree<float>;

#endif // RTREE_H
