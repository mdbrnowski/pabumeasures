# This module contains functions from pabutools the were modified to generate random instances and profiles in a
# deterministic way. For documentation of the original functions, please refer to the pabutools documentation.

import random
from math import ceil

from pabutools.election import ApprovalBallot, ApprovalProfile, Instance, Project


def get_random_instance(num_projects: int, min_cost: int, max_cost: int) -> tuple[Instance, list[Project]]:
    inst = Instance()
    projects = [
        Project(
            name=str(p),
            cost=random.randint(round(min_cost), round(max_cost)),
        )
        for p in range(round(num_projects))
    ]
    inst.update(projects)
    inst.budget_limit = random.randint(ceil(max(p.cost for p in inst)), ceil(sum(p.cost for p in inst)))
    return inst, projects


def get_random_approval_profile(instance: Instance, projects: list[Project], num_agents: int) -> ApprovalProfile:
    profile = ApprovalProfile(instance=instance)
    for i in range(num_agents):
        profile.append(get_random_approval_ballot(projects, name=f"RandomAppBallot {i}"))
    return profile


def get_random_approval_ballot(projects: list[Project], name: str) -> ApprovalBallot:
    ballot = ApprovalBallot(name=name)
    for p in projects:
        if random.random() > 0.5:
            ballot.add(p)
    return ballot
