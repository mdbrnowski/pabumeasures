#pragma once
#include <compare>
#include <string>
#include <utility>
#include <vector>

class ProjectComparator;

class ProjectEmbedding {
  public:
    template <typename StringT, typename VectorT>
    ProjectEmbedding(int cost, int id, StringT &&name, VectorT &&approvers)
        : cost_(cost), id_(id), name_(std::forward<StringT>(name)), approvers_(std::forward<VectorT>(approvers)) {}

    template <typename StringT> ProjectEmbedding(int cost, int id, StringT &&name) : cost_(cost), id_(id), name_(std::forward<StringT>(name)) {}

    ProjectEmbedding(int cost, int id) : cost_(cost), id_(id) {}

    ProjectEmbedding(int cost) : cost_(cost) {}

    int cost() const { return cost_; }
    int id() const { return id_; }
    const std::string &name() const { return name_; }
    const std::vector<int> &approvers() const { return approvers_; }

    friend class ProjectComparator;

  private:
    int cost_;
    int id_;
    std::string name_;
    std::vector<int> approvers_;
};