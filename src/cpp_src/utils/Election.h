#pragma once
#include "ProjectEmbedding.h"
#include <vector>

class Election {
  public:
    template <typename ProjectsT>
    Election(long long budget, int numVoters, ProjectsT &&projects)
        : budget_(budget), numVoters_(numVoters), projects_(std::forward<ProjectsT>(projects)) {}
    long long budget() const { return budget_; }
    int numVoters() const { return numVoters_; }
    const std::vector<ProjectEmbedding> &projects() const { return projects_; };

  private:
    long long budget_;
    int numVoters_;
    std::vector<ProjectEmbedding> projects_;
};
