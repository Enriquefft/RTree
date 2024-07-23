#include "Rtree.h"

template <std::floating_point T>
auto RTree<T>::search(const Point<T> &point) -> bool {

  return root->search(point);
}

template <std::floating_point T> void RTree<T>::insert(const Point<T> &point) {

  auto newNodes = root->insert(point);

  if (newNodes.has_value()) {
    // root was split
        std::cout << "Root was split\n";


    auto *newRoot = new RNode<T>(minChildren, maxChildren, false);
    newRoot->children.push_back(newNodes.value().first);
    newRoot->children.push_back(newNodes.value().second);

    newNodes.value().first->parent = newRoot;
    newNodes.value().second->parent = newRoot;

    newRoot->updateBoundingBox();

    root = newRoot;
  }
}

template <std::floating_point T> void RTree<T>::remove(const Point<T> &point) {

  std::vector<RNode<T> *> removed;
  root->remove(point, removed);
}

template <std::floating_point T>
auto RTree<T>::query(const QueryBox<T> &q) -> std::vector<Point<T>> {
  return root->query(q);
}

template <std::floating_point T> void RTree<T>::print() const {
  root->print(0);
}

template class RTree<float>;
