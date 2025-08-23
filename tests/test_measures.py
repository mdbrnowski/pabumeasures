import random

import pytest
from utils import get_random_approval_profile, get_random_instance

import pabumeasures

NUMBER_OF_TIMES = 100


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_measure(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    allocation = pabumeasures.greedy(instance, profile)
    project = random.choice(projects)
    result = pabumeasures.greedy_measure(instance, profile, project, pabumeasures.Measure.ADD_APPROVAL_OPTIMIST)
    # todo: needs more extensive assertions
    if project in allocation:
        assert result == 0
    elif project.cost > instance.budget_limit:
        assert result is None
    else:
        assert result is None or result >= 1


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_over_cost_measure(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    allocation = pabumeasures.greedy_over_cost(instance, profile)
    project = random.choice(projects)
    result = pabumeasures.greedy_over_cost_measure(
        instance, profile, project, pabumeasures.Measure.ADD_APPROVAL_OPTIMIST
    )
    print(instance, profile, allocation, project, result, sep="\n")
    # todo: needs more extensive assertions
    if project in allocation:
        assert result == 0
    elif project.cost > instance.budget_limit:
        assert result is None
    else:
        assert result is None or result >= 0
