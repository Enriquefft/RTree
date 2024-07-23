#include "Rtree.h"
#include <algorithm>
#include <optional>

using std::cout;
using std::optional;
using std::pair;

template <std::floating_point T> RNode<T>::~RNode() {

  if (isLeaf) {
    return;
  }
  for (auto child : children) {

    delete child;
  }
}

template <std::floating_point T>
auto RNode<T>::chooseSubtree(const Point<T> &point) -> RNode<T> * {
  // Choose the subtree that requires the least expansion to include the new
  // point
  RNode *bestChild = nullptr;
  NType leastExpansion = std::numeric_limits<float>::max();

  for (auto child : children) {
    NType expansionCost =
        child->boundingBox.calculateExpansionCost(MBB<T>(point, point));
    if (expansionCost < leastExpansion) {
      leastExpansion = expansionCost;
      bestChild = child;
    }
  }
  return bestChild;
}

template <std::floating_point T>
auto RNode<T>::split() -> std::pair<RNode<T> *, RNode<T> *> {

  // Create two new nodes
  auto *newNode1 = new RNode<T>(minChildren, maxChildren, isLeaf);
  auto *newNode2 = new RNode<T>(minChildren, maxChildren, isLeaf);

  // Set the parent of the new nodes
  newNode1->parent = this->parent;
  newNode2->parent = this->parent;

  if (isLeaf) {

    // Leaf node splitting
    std::pair<Point<T>, Point<T>> seeds = pickSeedsQuadratic();

    // Add the seeds to the new nodes
    newNode1->points.push_back(seeds.first);
    newNode2->points.push_back(seeds.second);

    std::cout << "pre-seeding points: " << points.size() << "\n";

    // remove seeds from points
    this->points.erase(
        std::remove(this->points.begin(), this->points.end(), seeds.first),
        this->points.end());
    this->points.erase(
        std::remove(this->points.begin(), this->points.end(), seeds.second),
        this->points.end());

    std::cout << "Remaining points: " << points.size() << "\n";

    // Distribute the remaining points
    while (!points.empty()) {
      auto entry = points.back();
      points.pop_back();

      if (newNode1->points.size() + points.size() + 1 == minChildren) {
        newNode1->points.push_back(entry);
      } else if (newNode2->points.size() + points.size() + 1 == minChildren) {
        newNode2->points.push_back(entry);
      } else {
        // Calculate the cost of adding this point to each of the new nodes
        auto cost1 =
            newNode1->boundingBox.calculateExpansionCost(MBB<T>(entry, entry));
        auto cost2 =
            newNode2->boundingBox.calculateExpansionCost(MBB<T>(entry, entry));

        // Add the point to the node that has the least cost increase
        if (cost1 <= cost2) {
          newNode1->points.push_back(entry);
        } else {
          newNode2->points.push_back(entry);
        }
      }
    }
  } else {

    std::cout << "Splitting internal node with " << children.size()
              << " entries\n";

    // Inner node splitting
    std::pair<RNode<T> *, RNode<T> *> seeds = pickInternalSeedsQuadratic();

    // Add the seeds to the new nodes
    newNode1->children.push_back(seeds.first);
    newNode2->children.push_back(seeds.second);

    // Remove the seeds from the current node's children
    this->children.erase(
        std::remove(this->children.begin(), this->children.end(), seeds.first),
        this->children.end());
    this->children.erase(
        std::remove(this->children.begin(), this->children.end(), seeds.second),
        this->children.end());

    // Distribute the remaining children
    while (!children.empty()) {
      auto *entry = children.back();
      children.pop_back();

      if (newNode1->children.size() + children.size() + 1 == minChildren) {
        newNode1->children.push_back(entry);
      } else if (newNode2->children.size() + children.size() + 1 ==
                 minChildren) {
        newNode2->children.push_back(entry);
      } else {
        auto cost1 =
            newNode1->boundingBox.calculateExpansionCost(entry->boundingBox);
        auto cost2 =
            newNode2->boundingBox.calculateExpansionCost(entry->boundingBox);

        if (cost1 <= cost2) {
          newNode1->children.push_back(entry);
        } else {
          newNode2->children.push_back(entry);
        }
      }
    }
  }

  // Update bounding boxes
  updateBoundingBox();
  newNode1->updateBoundingBox();
  newNode2->updateBoundingBox();

  // Optionally print debug information
  std::cout << "Splitting into: "
            << (isLeaf ? newNode1->points.size() : newNode1->children.size())
            << " and "
            << (isLeaf ? newNode2->points.size() : newNode2->children.size())
            << " entries\n";
  if (isLeaf) {
    cout << "Leaf node split\n";
    for (const auto &point : newNode1->points) {
      std::cout << "Point: " << point << '\n';
    }
    cout << "Right node split\n";
    for (const auto &point : newNode2->points) {
      std::cout << "Point: " << point << '\n';
    }
  }
  delete this;
  return {newNode1, newNode2};
}

template <std::floating_point T>
auto RNode<T>::pickInternalSeedsQuadratic()
    -> std::pair<RNode<T> *, RNode<T> *> {

  return {children[0], children[1]};

  NType maxWastedArea = -1;
  std::pair<RNode<T> *, RNode<T> *> seeds = {nullptr, nullptr};

  for (size_t i = 0; i < children.size() - 1; ++i) {
    for (size_t j = i + 1; j < children.size(); ++j) {
      MBB<T> m = children.at(i)->boundingBox;
      m.expand(children[j]->boundingBox);
      NType wastedArea = m.area() - (children[i]->boundingBox.area() +
                                     children[j]->boundingBox.area());

      if (wastedArea > maxWastedArea) {
        maxWastedArea = wastedArea;
        seeds = std::make_pair(children[i], children[j]);
      }
    }
  }

  return seeds;
}
template <std::floating_point T>
auto RNode<T>::pickSeedsQuadratic() -> std::pair<Point<T>, Point<T>> {
  NType maxWastedArea = -1;
  std::pair<Point<T>, Point<T>> seeds;

  for (size_t i = 0; i < points.size() - 1; ++i) {
    for (size_t j = i + 1; j < points.size(); ++j) {
      MBB<T> m(points[i], points[j]);
      if (m.area() > maxWastedArea) {
        maxWastedArea = m.area();
        seeds = std::make_pair(points[i], points[j]);
      }
    }
  }

  return seeds;
}

template <std::floating_point T> void RNode<T>::updateBoundingBox() {
  if (isLeaf) {
    if (points.empty()) {
      return;
    }
    boundingBox = MBB<T>(points[0], points[0]);
    for (const auto &point : points) {
      boundingBox.expand(MBB<T>(point, point));
    }
  } else {
    if (children.empty()) {
      return;
    }
    boundingBox = children[0]->boundingBox;
    for (const auto &child : children) {
      boundingBox.expand(child->boundingBox);
    }
  }
}

template <std::floating_point T>
auto RNode<T>::search(const Point<T> &point) -> bool {
  if (isLeaf) {

    return std::find(points.begin(), points.end(), point) != points.end();
  } else {
    for (auto child : children) {
      if (child->boundingBox.contains(point)) {

        if (child->search(point)) {
          return true;
        }
      }
    }
  }
  return false;
}

template <std::floating_point T>
auto RNode<T>::insert(const Point<T> &point)
    -> optional<pair<RNode<T> *, RNode<T> *>> {

  if (isLeaf) {

    points.push_back(point);
    updateBoundingBox();
    if (points.size() > maxChildren) {
      return split();
    }
    return std::nullopt;
  } else {

    RNode *child = chooseSubtree(point);

    // handle overflow
    auto newChildren = child->insert(point);

    if (newChildren.has_value()) {
      this->children.erase(
          std::remove(this->children.begin(), this->children.end(), child),
          this->children.end());

      this->children.push_back(newChildren->first);
      this->children.push_back(newChildren->second);
      if (this->children.size() > maxChildren) {
        return split();
      }
    }
    updateBoundingBox();
    return std::nullopt;
  }
}

template <std::floating_point T>
auto RNode<T>::query(const QueryBox<T> &q) -> std::vector<Point<T>> {
  std::vector<Point<T>> result;
  if (isLeaf) {
    for (const auto &point : points) {
      if (q.contains(point)) {
        result.push_back(point);
      }
    }
  } else {
    for (const auto &child : children) {
      if (q.intersects(child->boundingBox)) {
        auto childResult = child->query(q);
        result.insert(result.end(), childResult.begin(), childResult.end());
      }
    }
  }
  return result;
}

template <std::floating_point T>
auto RNode<T>::findLeaf(const Point<T> &point) -> RNode<T> * {
  if (isLeaf) {
    auto it = std::find(points.begin(), points.end(), point);
    if (it != points.end()) {
      return this;
    }
  } else {
    for (auto child : children) {
      if (child->boundingBox.contains(point)) {
        RNode *leaf = child->findLeaf(point);
        if (leaf != nullptr) {
          return leaf;
        }
      }
    }
  }
  return nullptr;
}

template <std::floating_point T>
void RNode<T>::remove(const Point<T> &point,
                      std::vector<RNode<T> *> &eliminated) {
  RNode *leaf = findLeaf(point);
  if (leaf != nullptr) {

    leaf->points.erase(
        std::remove(leaf->points.begin(), leaf->points.end(), point),
        leaf->points.end());

    leaf->updateBoundingBox();
    if (leaf->points.size() < minChildren) {
      condenseTree(leaf, eliminated);
    } else {
      adjustTree(leaf, eliminated);
    }
  }
}

template <std::floating_point T>
void RNode<T>::adjustTree(RNode<T> *n, std::vector<RNode<T> *> &eliminated) {
  while (n != nullptr) {
    n->updateBoundingBox();
    if (n->parent == nullptr) {
      break;
    }
    if (n->points.size() < minChildren || n->children.size() < minChildren) {
      eliminated.push_back(n);

      n->parent->children.erase(std::remove(n->parent->children.begin(),
                                            n->parent->children.end(), n),
                                n->parent->children.end());
    }
    n = n->parent;
  }
}

template <std::floating_point T>
void RNode<T>::condenseTree(RNode<T> *n, std::vector<RNode<T> *> &eliminated) {
  adjustTree(n, eliminated);
  for (auto node : eliminated) {
    if (node->isLeaf) {
      for (const auto &point : node->points) {
        insert(point);
      }
    } else {
      for (auto child : node->children) {
        children.push_back(child);
        child->parent = this;
      }
    }
    delete node;
  }
}

template <std::floating_point T> void RNode<T>::print(size_t depth) const {

  std::string indent(depth * 2, ' ');
  std::cout << indent << "Node at depth " << depth
            << (isLeaf ? " (Leaf)" : " (Internal)") << '\n';

  if (isLeaf) {
    for (const auto &point : getPoints()) {
      std::cout << indent << "  Point: " << point << std::endl;
    }
  } else {
    for (const auto &child : getChildren()) {
      child->print(depth + 1);
    }
  }
}

template class RNode<float>;
