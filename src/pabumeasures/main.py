from enum import Enum, auto

from pabutools.election.ballot import Ballot, FrozenBallot
from pabutools.election.instance import Instance, Project
from pabutools.election.profile import Profile
from pabutools.rules import BudgetAllocation

from pabumeasures import _core


class Measure(Enum):
    COST_REDUCTION = auto()
    ADD_APPROVAL_OPTIMIST = auto()
    ADD_APPROVAL_PESSIMIST = auto()
    ADD_SINGLETON = auto()


def _translate_input_format(
    instance: Instance, profile: Profile
) -> tuple[list[Project], list[Ballot], int, list[int], list[list[int]]]:
    if not isinstance(instance, Instance):
        raise TypeError("Instance must be of type Instance")
    if not isinstance(profile, Profile):
        raise TypeError("Profile must be of type Profile")

    projects: list[Project] = [project for project in instance]
    _project_to_id = {project: i for i, project in enumerate(projects)}
    ballots: list[Ballot] = [ballot for ballot in profile]
    frozen_ballots: list[FrozenBallot] = [ballot.frozen() for ballot in profile]
    _ballot_to_id = {ballot: i for i, ballot in enumerate(frozen_ballots)}
    total_budget = int(instance.budget_limit)  # todo: remove int() if budget_limit can be float/mpq
    cost: list[int] = [int(project.cost) for project in instance]  # todo: remove int() if cost can be float/mpq
    approvers: list[list[int]] = [[] for _ in range(len(projects))]
    for ballot in profile:
        ballot_id = _ballot_to_id[ballot.frozen()]
        for project in ballot:
            approvers[_project_to_id[project]].append(ballot_id)
    return projects, ballots, total_budget, cost, approvers


def greedy(instance: Instance, profile: Profile) -> BudgetAllocation:
    projects, ballots, total_budget, cost, approvers = _translate_input_format(instance, profile)
    result = _core.greedy(len(projects), len(ballots), total_budget, cost, approvers)
    return BudgetAllocation(projects[i] for i in result)


def greedy_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    projects, ballots, total_budget, cost, approvers = _translate_input_format(instance, profile)
    p = projects.index(project)
    match measure:
        case Measure.COST_REDUCTION:
            raise NotImplementedError()
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_greedy(len(projects), len(ballots), total_budget, cost, approvers, p)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_greedy(len(projects), len(ballots), total_budget, cost, approvers, p)
        case Measure.ADD_SINGLETON:
            raise NotImplementedError()


def greedy_over_cost(instance: Instance, profile: Profile) -> BudgetAllocation:
    projects, ballots, total_budget, cost, approvers = _translate_input_format(instance, profile)
    result = _core.greedy_over_cost(len(projects), len(ballots), total_budget, cost, approvers)
    return BudgetAllocation(projects[i] for i in result)


def greedy_over_cost_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    projects, ballots, total_budget, cost, approvers = _translate_input_format(instance, profile)
    p = projects.index(project)
    match measure:
        case Measure.COST_REDUCTION:
            raise NotImplementedError()
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_greedy_over_cost(
                len(projects), len(ballots), total_budget, cost, approvers, p
            )
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_greedy_over_cost(
                len(projects), len(ballots), total_budget, cost, approvers, p
            )
        case Measure.ADD_SINGLETON:
            raise NotImplementedError()


def mes_apr(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def mes_apr_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    raise NotImplementedError()


def mes_cost(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def mes_cost_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    raise NotImplementedError()


def phragmen(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def phragmen_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    raise NotImplementedError()
