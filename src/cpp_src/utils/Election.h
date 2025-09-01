#pragma once
#include "ProjectEmbedding.h"
#include <vector>

class Election {
  public:
    template <typename ProjectsT>
    Election(int budget, int numVoters, ProjectsT &&projects)
        : budget_(budget), numVoters_(numVoters), projects_(std::forward<ProjectsT>(projects)) {}
    int budget() const { return budget_; }
    int numVoters() const { return numVoters_; }
    const std::vector<ProjectEmbedding> &projects() const { return projects_; };

  private:
    int budget_;
    int numVoters_;
    std::vector<ProjectEmbedding> projects_;
};
