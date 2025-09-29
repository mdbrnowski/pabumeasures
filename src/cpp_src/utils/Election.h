#pragma once
#include "ProjectEmbedding.h"
#include <vector>

class Election {
  public:
    template <typename ProjectsT>
    Election(long long budget, int num_voters, ProjectsT &&projects)
        : budget_(budget), num_voters_(num_voters), projects_(std::forward<ProjectsT>(projects)) {}
    long long budget() const { return budget_; }
    int num_voters() const { return num_voters_; }
    const std::vector<ProjectEmbedding> &projects() const { return projects_; };

  private:
    long long budget_;
    int num_voters_;
    std::vector<ProjectEmbedding> projects_;
};
