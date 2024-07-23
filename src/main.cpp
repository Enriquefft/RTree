#include "Rtree.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

constexpr size_t POINTS = 100;
constexpr auto RANGE = 10;

namespace {
using std::cout;

auto testPointsStored(RTree<float> &tree,
                      const std::vector<Point<float>> &points) -> bool {
  for (const auto &point : points) {
    if (!tree.search(point)) {
      std::cout << "Point not found in RTree: " << point << '\n';
      return false;
    }
  }
  return true;
}

void collectLeafHeights(RNode<float> *node, int currentHeight,
                        std::vector<int> &heights) {
  if (node->isLeaf) {
    heights.push_back(currentHeight);
  } else {
    for (auto *child : node->getChildren()) {
      collectLeafHeights(child, currentHeight + 1, heights);
    }
  }
}
auto testTreeBalance(RTree<float> &tree) -> bool {
  if (!tree.getRoot()) {
    return true;
  }

  std::vector<int> leafHeights;
  collectLeafHeights(tree.getRoot(), 0, leafHeights);

  if (!leafHeights.empty()) {
    int firstHeight = leafHeights[0];
    for (int height : leafHeights) {
      if (height != firstHeight) {
        std::cout
            << "Tree is not balanced: Not all leaves are at the same height."
            << '\n';
        return false;
      }
    }
  }
  return true;
}

auto checkNodeCapacity(const RNode<float> *node, uint minChildren,
                       uint maxChildren) -> bool {
  if (node->isLeaf) {
    if (node->getPoints().size() < minChildren ||
        node->getPoints().size() > maxChildren) {
      if (node->getParent() != nullptr || node->getPoints().size() != 0) {

        cout << "has parent: " << (node->getParent() != nullptr) << '\n';
        cout << "points size: " << node->getPoints().size() << '\n';

        std::cout << "Leaf node capacity violation. Contains "
                  << node->getPoints().size() << " points." << '\n';
        return false;
      }
    }
  } else {
    if (node->getChildren().size() < minChildren ||
        node->getChildren().size() > maxChildren) {
      if (node->getParent() != nullptr || node->getChildren().size() != 0) {
        std::cout << "Internal node capacity violation. Contains "
                  << node->getChildren().size() << " children." << '\n';
        return false;
      }
    }
    for (const auto *child : node->getChildren()) {
      if (!checkNodeCapacity(child, minChildren, maxChildren)) {
        return false;
      }
    }
  }
  return true;
}
auto testCapacity(RTree<float> &tree) -> bool {

  if (!tree.getRoot()) {
    std::cout << "The tree is empty." << '\n';
    return true;
  }
  return checkNodeCapacity(tree.getRoot(), 2, 3);
}

auto checkPointsInBoundingBox(const RNode<float> *node) -> bool {
  if (node->isLeaf) {
    for (const auto &point : node->getPoints()) {
      if (!node->getBoundingBox().contains(point)) {
        std::cout << "Point " << point
                  << " is not contained in its node's bounding box." << '\n';
        return false;
      }
    }
  } else {
    for (const auto *child : node->getChildren()) {
      if (!checkPointsInBoundingBox(child)) {
        return false;
      }
    }
  }
  return true;
}
auto testPointsInBoundingBox(RTree<float> &tree) -> bool {
  if (!tree.getRoot()) {
    std::cout << "The tree is empty, no points to check." << '\n';
    return true;
  }
  return checkPointsInBoundingBox(tree.getRoot());
}

auto isContained(const MBB<float> &inner, const MBB<float> &outer) -> bool {
  bool containedInX = (inner.lowerLeft.getX() >= outer.lowerLeft.getX()) &&
                      (inner.upperRight.getX() <= outer.upperRight.getX());
  bool containedInY = (inner.lowerLeft.getY() >= outer.lowerLeft.getY()) &&
                      (inner.upperRight.getY() <= outer.upperRight.getY());
  return containedInX && containedInY;
}
auto checkChildrenInParentMBB(const RNode<float> *node) -> bool {
  if (!node->isLeaf) {
    auto parentMBB = node->getBoundingBox();
    std::vector<RNode<float> *> children = node->getChildren();
    for (const auto *child : children) {
      auto childMBB = child->getBoundingBox();
      if (!isContained(childMBB, parentMBB)) {
        std::cout << "Child MBB is not contained in its parent's MBB." << '\n';
        return false;
      }
      if (!checkChildrenInParentMBB(child)) {
        return false;
      }
    }
  }
  return true;
}
auto testChildrenInParentMBB(RTree<float> &tree) -> bool {
  auto *root = tree.getRoot();
  if (!root) {
    std::cout
        << "The tree is empty, no parent-child MBB relationships to check."
        << '\n';
    return true;
  }
  return checkChildrenInParentMBB(root);
}
} // namespace

auto main() -> int {
  RTree<float> tree;
  std::vector<Point<float>> points;

  // Insertar puntos aleatorios
  std::mt19937 rng(static_cast<uint64_t>(
      std::chrono::steady_clock::now().time_since_epoch().count()));
  std::uniform_real_distribution<float> dist(0.0F, static_cast<float>(RANGE));

  // Insert random points
  for (size_t i = 0; i < POINTS; ++i) {
    float x = dist(rng);
    float y = dist(rng);
    Point<float> p(x, y);
    points.push_back(p);
    std::cout << "Inserting point: " << p << '\n';
    tree.insert(p);
  tree.print();
  }

  // Ejecutar pruebas
  if (testPointsStored(tree, points)) {
    std::cout << "Test Points Stored: Passed\n";
  } else {
    std::cout << "Test Points Stored: Failed\n";
  }

  if (testTreeBalance(tree)) {
    std::cout << "Test Tree Balance: Passed\n";
  } else {
    std::cout << "Test Tree Balance: Failed\n";
  }

  if (testCapacity(tree)) {
    std::cout << "Test Capacity: Passed\n";
  } else {
    std::cout << "Test Capacity: Failed\n";
  }

  if (testPointsInBoundingBox(tree)) {
    std::cout << "Test Points In BoundingBox: Passed\n";
  } else {
    std::cout << "Test Points In BoundingBox: Failed\n";
  }

  if (testChildrenInParentMBB(tree)) {
    std::cout << "Test Children In Parent MBB: Passed\n";
  } else {
    std::cout << "Test Children In Parent MBB: Failed\n";
  }

  return 0;
}
