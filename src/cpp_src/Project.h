#pragma once
#include <compare>
#include <string>
#include <utility>
#include <vector>

class ProjectComparator;

class Project {
  public:
    template <typename StringT, typename VectorT>
    Project(int cost, StringT &&name, VectorT &&approvers)
        : cost_(cost), name_(std::forward<StringT>(name)), approvers_(std::forward<VectorT>(approvers)) {}

    template <typename StringT> Project(int cost, StringT &&name) : cost_(cost), name_(std::forward<StringT>(name)) {}

    Project(int cost) : cost_(cost) {}

    friend class ProjectComparator;

  private:
    int cost_;
    std::string name_;
    std::vector<int> approvers_;
};