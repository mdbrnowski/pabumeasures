import glob
import random

import pytest
from pabutools.election import ApprovalProfile, Cardinality_Sat, Cost_Sat, parse_pabulib
from pabutools.rules import greedy_utilitarian_welfare, sequential_phragmen
from pabutools.tiebreaking import TieBreakingRule
from utils import get_random_approval_profile, get_random_instance

import pabumeasures

# redefined pabutools tie-breaking rules to include project name as a secondary criterion
min_cost_tie_breaking = TieBreakingRule(lambda inst, prof, proj: (proj.cost, proj.name))
max_cost_tie_breaking = TieBreakingRule(lambda inst, prof, proj: (-proj.cost, proj.name))

test_files = glob.glob("./data/*.pb")
NUMBER_OF_TIMES = 500


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


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_random_different_comparator(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cost_Sat, tie_breaking=max_cost_tie_breaking
    )
    result = pabumeasures.greedy(
        instance,
        profile,
        pabumeasures.ProjectComparator(pabumeasures.Comparator.COST, pabumeasures.Ordering.DESCENDING),
    )

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


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_greedy_over_cost_random_different_comparator(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cardinality_Sat, tie_breaking=max_cost_tie_breaking
    )
    result = pabumeasures.greedy_over_cost(
        instance,
        profile,
        pabumeasures.ProjectComparator(pabumeasures.Comparator.COST, pabumeasures.Ordering.DESCENDING),
    )

    assert sorted(pabutools_result) == sorted(result)


@pytest.mark.parametrize("file", test_files)
def test_phragmen(file):
    instance, profile = parse_pabulib(file)
    assert isinstance(profile, ApprovalProfile)  # for type checking
    pabutools_result = sequential_phragmen(instance, profile, tie_breaking=min_cost_tie_breaking)
    result = pabumeasures.phragmen(instance, profile)

    assert sorted(pabutools_result) == sorted(result)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
def test_phragmen_random(seed):
    random.seed(seed)
    instance, projects = get_random_instance(3, 1, 10)
    profile = get_random_approval_profile(instance, projects, 10)
    pabutools_result = sequential_phragmen(instance, profile, tie_breaking=min_cost_tie_breaking)
    result = pabumeasures.phragmen(instance, profile)

    assert sorted(pabutools_result) == sorted(result)
