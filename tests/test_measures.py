import random
from itertools import chain, combinations

import pytest
from pabutools.election import ApprovalBallot
from utils import get_random_election, get_random_project

import pabumeasures
from pabumeasures import Measure


def _powerset(iterable):
    # powerset([1,2,3]) → () (1,) (2,) (3,) (1,2) (1,3) (2,3) (1,2,3)
    s = list(iterable)
    return chain.from_iterable(combinations(s, r) for r in range(len(s) + 1))


NUMBER_OF_TIMES = 500


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize(
    "rule,rule_measure",
    [
        (pabumeasures.greedy, pabumeasures.greedy_measure),
        (pabumeasures.greedy_over_cost, pabumeasures.greedy_over_cost_measure),
        (pabumeasures.mes_apr, pabumeasures.mes_apr_measure),
        (pabumeasures.mes_cost, pabumeasures.mes_cost_measure),
        (pabumeasures.phragmen, pabumeasures.phragmen_measure),
    ],
    ids=["greedy", "greedy_over_cost", "mes_apr", "mes_cost", "phragmen"],
)
def test_optimist_add_measure(seed, rule, rule_measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = rule(instance, profile)
    result = rule_measure(instance, profile, project, Measure.ADD_APPROVAL_OPTIMIST)
    if project in allocation:
        assert result == 0
    else:
        non_approvers = [ballot for ballot in profile if project not in ballot]
        if result is None:
            for na in non_approvers:
                na.add(project)
            assert project not in rule(instance, profile), "result should not be None"
        else:
            assert 1 <= result <= len(non_approvers)
            for new_approvers in _powerset(non_approvers):
                for na in new_approvers:
                    na.add(project)
                if project in rule(instance, profile):
                    # here we use the fact that subsets are generated in increasing order of size
                    assert result == len(new_approvers)
                    return
                for na in new_approvers:
                    na.remove(project)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize(
    "rule,rule_measure",
    [
        (pabumeasures.greedy, pabumeasures.greedy_measure),
        (pabumeasures.greedy_over_cost, pabumeasures.greedy_over_cost_measure),
        (pabumeasures.mes_apr, pabumeasures.mes_apr_measure),
        (pabumeasures.mes_cost, pabumeasures.mes_cost_measure),
        (pabumeasures.phragmen, pabumeasures.phragmen_measure),
    ],
    ids=["greedy", "greedy_over_cost", "mes_apr", "mes_cost", "phragmen"],
)
def test_pessimist_add_measure(seed, rule, rule_measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = rule(instance, profile)
    result = rule_measure(instance, profile, project, Measure.ADD_APPROVAL_PESSIMIST)
    if project in allocation:
        assert result == 0
    else:
        non_approvers = [ballot for ballot in profile if project not in ballot]
        for expected_result in range(1, len(non_approvers) + 1):
            ok = True
            for new_approvers in combinations(non_approvers, expected_result):
                for na in new_approvers:
                    na.add(project)
                if project not in rule(instance, profile):
                    ok = False
                for na in new_approvers:
                    na.remove(project)
            if ok:
                assert result == expected_result
                return
        assert result is None


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize(
    "rule,rule_measure",
    [
        (pabumeasures.greedy, pabumeasures.greedy_measure),
        (pabumeasures.greedy_over_cost, pabumeasures.greedy_over_cost_measure),
        (pabumeasures.mes_apr, pabumeasures.mes_apr_measure),
        (pabumeasures.mes_cost, pabumeasures.mes_cost_measure),
        (pabumeasures.phragmen, pabumeasures.phragmen_measure),
    ],
    ids=["greedy", "greedy_over_cost", "mes_apr", "mes_cost", "phragmen"],
)
def test_singleton_add_measure(seed, rule, rule_measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = rule(instance, profile)
    result = rule_measure(instance, profile, project, Measure.ADD_SINGLETON)

    if rule in [pabumeasures.mes_apr, pabumeasures.mes_cost] and result is None:
        assert instance.budget_limit == project.cost
    else:
        assert result is not None

        if project in allocation:
            assert result == 0
        else:
            assert result >= 1
            for i in range(len(profile), len(profile) + result):
                profile.append(ApprovalBallot({project}, name=f"SingletonAppBallot {i}"))
            assert project in rule(instance, profile)
            profile.pop()
            assert project not in rule(instance, profile)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize(
    "rule,rule_measure",
    [
        (pabumeasures.greedy, pabumeasures.greedy_measure),
        (pabumeasures.greedy_over_cost, pabumeasures.greedy_over_cost_measure),
        (pabumeasures.mes_apr, pabumeasures.mes_apr_measure),
        (pabumeasures.mes_cost, pabumeasures.mes_cost_measure),
        (pabumeasures.phragmen, pabumeasures.phragmen_measure),
    ],
    ids=["greedy", "greedy_over_cost", "mes_apr", "mes_cost", "phragmen"],
)
def test_cost_reduction_measure(seed, rule, rule_measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = rule(instance, profile)
    result = rule_measure(instance, profile, project, Measure.COST_REDUCTION)

    if project in allocation:
        assert result == project.cost
    else:
        if result > 0:
            project.cost = result
            assert project in rule(instance, profile)
        else:
            assert result == 0

        project.cost = result + 1
        assert project not in rule(instance, profile)
