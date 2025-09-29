#pragma once
#include "ProjectEmbedding.h"
#include <vector>

class Election {
  public:
    template <typename ProjectsT>
    Election(long long budget, int num_of_voters, ProjectsT &&projects)
        : budget_(budget), num_of_voters_(num_of_voters), projects_(std::forward<ProjectsT>(projects)) {}
    long long budget() const { return budget_; }
    int num_of_voters() const { return num_of_voters_; }
    const std::vector<ProjectEmbedding> &projects() const { return projects_; };

  private:
    long long budget_;
    int num_of_voters_;
    std::vector<ProjectEmbedding> projects_;
};
