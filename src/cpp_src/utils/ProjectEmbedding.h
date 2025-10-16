#pragma once
#include <compare>
#include <string>
#include <utility>
#include <vector>

class ProjectComparator;

class ProjectEmbedding {
  public:
    template <typename StringT, typename VectorT>
    ProjectEmbedding(long long cost, StringT &&name, VectorT &&approvers)
        : cost_(cost), name_(std::forward<StringT>(name)), approvers_(std::forward<VectorT>(approvers)) {}

    bool operator==(const ProjectEmbedding &other) const { return name_ == other.name_; }
    long long cost() const { return cost_; }
    const std::string &name() const { return name_; }
    const std::vector<int> &approvers() const { return approvers_; }
    int num_of_approvers() const { return approvers_.size(); }

    friend class ProjectComparator;

  private:
    long long cost_;
    std::string name_;
    std::vector<int> approvers_;
};
