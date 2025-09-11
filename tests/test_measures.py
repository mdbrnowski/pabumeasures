import random

import pytest
from pabutools.election import ApprovalBallot
from utils import get_random_election, get_random_project

import pabumeasures
from pabumeasures import Measure

NUMBER_OF_TIMES = 500


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize("measure", [Measure.ADD_APPROVAL_OPTIMIST, Measure.ADD_APPROVAL_PESSIMIST])
def test_greedy_measure(seed, measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = pabumeasures.greedy(instance, profile)
    result = pabumeasures.greedy_measure(instance, profile, project, measure)
    if project in allocation:
        assert result == 0
    else:
        non_approvers = [ballot for ballot in profile if project not in ballot]
        if result is None:
            for na in non_approvers:
                na.add(project)
            assert project not in pabumeasures.greedy(instance, profile)
        else:
            assert 1 <= result <= len(non_approvers)
            for na in non_approvers[:result]:
                na.add(project)
            assert project in pabumeasures.greedy(instance, profile)
            non_approvers[0].remove(project)
            assert project not in pabumeasures.greedy(instance, profile)


@pytest.mark.parametrize("seed", list(range(NUMBER_OF_TIMES)))
@pytest.mark.parametrize("measure", [Measure.ADD_APPROVAL_OPTIMIST, Measure.ADD_APPROVAL_PESSIMIST])
def test_greedy_over_cost_measure(seed, measure):
    random.seed(seed)
    instance, profile = get_random_election()
    project = get_random_project(instance)
    allocation = pabumeasures.greedy_over_cost(instance, profile)
    result = pabumeasures.greedy_over_cost_measure(instance, profile, project, measure)
    if project in allocation:
        assert result == 0
    else:
        non_approvers = [ballot for ballot in profile if project not in ballot]
        if result is None:
            for na in non_approvers:
                na.add(project)
            assert project not in pabumeasures.greedy_over_cost(instance, profile)
        else:
            assert 1 <= result <= len(non_approvers)
            for na in non_approvers[:result]:
                na.add(project)
            assert project in pabumeasures.greedy_over_cost(instance, profile)
            non_approvers[0].remove(project)
            assert project not in pabumeasures.greedy_over_cost(instance, profile)


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
