# This module contains functions from pabutools the were modified to generate random instances and profiles in a
# deterministic way. For documentation of the original functions, please refer to the pabutools documentation.

import random
from math import ceil

from pabutools.election import ApprovalProfile, Instance, Project, get_random_approval_ballot


def get_random_election(
    num_projects: int = 3, min_cost: int = 1, max_cost: int = 4, num_agents: int = 5
) -> tuple[Instance, ApprovalProfile]:
    """Generates and returns a random election (Instance and ApprovalProfile) with the given parameters."""
    instance = get_random_instance(num_projects, min_cost, max_cost)
    profile = get_random_approval_profile(instance, num_agents)
    return instance, profile


def get_random_project(instance: Instance) -> Project:
    """Selects and returns a random Project from the given Instance."""
    return random.choice(sorted(instance))


def get_random_instance(num_projects: int, min_cost: int, max_cost: int) -> Instance:
    """
    Generates a random instance.

    Function adapted from pabutools, disallows not feasible projects.
    """
    inst = Instance()
    inst.update(
        Project(
            name=str(p),
            cost=random.randint(round(min_cost), round(max_cost)),
        )
        for p in range(round(num_projects))
    )
    inst.budget_limit = random.randint(ceil(max(p.cost for p in inst)), ceil(sum(p.cost for p in inst)))
    return inst


def get_random_approval_profile(instance: Instance, num_agents: int) -> ApprovalProfile:
    """
    Generates a random approval profile in which approval ballots are such that each project is approved with
    probability 0.5.

    Function adapted from pabutools, we can remove it when pabutools#48 is released.
    """
    profile = ApprovalProfile(instance=instance)
    sorted_instance: list[Project] = sorted(instance)
    for i in range(num_agents):
        profile.append(get_random_approval_ballot(sorted_instance, name=f"RandomAppBallot {i}"))
    return profile
