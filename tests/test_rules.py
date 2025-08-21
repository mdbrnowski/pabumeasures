import glob
import random

import pytest
from pabutools.election import Cardinality_Sat, Cost_Sat, parse_pabulib
from pabutools.rules import greedy_utilitarian_welfare
from pabutools.tiebreaking import min_cost_tie_breaking
from utils import get_random_approval_profile, get_random_instance

import pabumeasures

test_files = glob.glob("./data/*.pb")
NUMBER_OF_TIMES = 100


@pytest.mark.parametrize("file", test_files)
def test_greedy(file):
    instance, profile = parse_pabulib(file)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cost_Sat, tie_breaking=min_cost_tie_breaking
    )
    result = pabumeasures.greedy(instance, profile)

    assert sorted(pabutools_result) == sorted(result)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_random(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cost_Sat, tie_breaking=min_cost_tie_breaking
    )
    result = pabumeasures.greedy(instance, profile)

    assert sorted(pabutools_result) == sorted(result)


@pytest.mark.parametrize("file", test_files)
def test_greedy_over_cost(file):
    instance, profile = parse_pabulib(file)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cardinality_Sat, tie_breaking=min_cost_tie_breaking
    )
    result = pabumeasures.greedy_over_cost(instance, profile)

    assert sorted(pabutools_result) == sorted(result)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_over_cost_random(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cardinality_Sat, tie_breaking=min_cost_tie_breaking
    )
    result = pabumeasures.greedy_over_cost(instance, profile)

    assert sorted(pabutools_result) == sorted(result)
