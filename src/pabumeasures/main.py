from enum import Enum, auto

from pabutools.election.ballot import Ballot, FrozenBallot
from pabutools.election.instance import Instance, Project
from pabutools.election.profile import ApprovalProfile, Profile
from pabutools.rules import BudgetAllocation

from pabumeasures import _core


class Measure(Enum):
    COST_REDUCTION = auto()
    ADD_APPROVAL_OPTIMIST = auto()
    ADD_APPROVAL_PESSIMIST = auto()
    ADD_SINGLETON = auto()


def _translate_input_format_temp(
    instance: Instance, profile: Profile
) -> tuple[list[Project], list[Ballot], int, list[int], list[list[int]]]:
    if not isinstance(instance, Instance):
        raise TypeError("Instance must be of type Instance")
    if not isinstance(profile, ApprovalProfile):
        raise TypeError("Profile must be of type ApprovalProfile")

    projects: list[Project] = sorted(instance)
    _project_to_id = {project: i for i, project in enumerate(projects)}
    ballots: list[Ballot] = [ballot for ballot in profile]
    frozen_ballots: list[FrozenBallot] = [ballot.frozen() for ballot in profile]
    _ballot_to_id = {ballot: i for i, ballot in enumerate(frozen_ballots)}
    total_budget = int(instance.budget_limit)  # todo: remove int() if budget_limit can be float/mpq
    cost: list[int] = [int(project.cost) for project in projects]  # todo: remove int() if cost can be float/mpq
    approvers: list[list[int]] = [[] for _ in range(len(projects))]
    for ballot in profile:
        ballot_id = _ballot_to_id[ballot.frozen()]
        for project in ballot:
            approvers[_project_to_id[project]].append(ballot_id)
    return projects, ballots, total_budget, cost, approvers


def _translate_input_format(
    instance: Instance, profile: Profile
) -> tuple[int, dict[str, Project], list[_core.ProjectEmbedding]]:
    if not isinstance(instance, Instance):
        raise TypeError("Instance must be of type Instance")
    if not isinstance(profile, ApprovalProfile):
        raise TypeError("Profile must be of type ApprovalProfile")
    if len([project.name for project in instance]) != len({project.name for project in instance}):
        raise ValueError("Project names must be unique in the instance")
    if any(project.cost <= 0 for project in instance):
        raise ValueError("Project costs must be positive")
    if any(project.cost > instance.budget_limit for project in instance):
        raise ValueError("Project costs must not exceed the budget limit")
    if instance.budget_limit > 1_000_000_000:
        raise ValueError("Budget limit must not exceed 1 billion")

    projects: list[Project] = sorted(instance)
    frozen_ballots: list[tuple[int, FrozenBallot]] = [(i, ballot.frozen()) for i, ballot in enumerate(profile)]
    total_budget = int(
        instance.budget_limit
    )  # todo: remove int() (and type in ProjectEmbedding) if budget_limit can be float/mpq
    approvers: dict[str, list[int]] = {project.name: [] for project in projects}
    for i, frozen_ballot in frozen_ballots:
        for project in frozen_ballot:
            approvers[project.name].append(i)
    project_embeddings: list[_core.ProjectEmbedding] = [
        _core.ProjectEmbedding(int(project.cost), project.name, approvers[project.name]) for project in projects
    ]  # todo: remove int() (and type in ProjectEmbedding) if budget_limit can be float/mpq
    name_to_project: dict[str, Project] = {project.name: project for project in projects}
    return total_budget, name_to_project, project_embeddings


def greedy(instance: Instance, profile: Profile) -> BudgetAllocation:
    total_budget, name_to_project, project_embeddings = _translate_input_format(instance, profile)
    result = _core.greedy(total_budget, project_embeddings)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def greedy_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    projects, ballots, total_budget, cost, approvers = _translate_input_format_temp(instance, profile)
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
    total_budget, name_to_project, project_embeddings = _translate_input_format(instance, profile)
    result = _core.greedy_over_cost(total_budget, project_embeddings)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def greedy_over_cost_measure(instance: Instance, profile: Profile, project: Project, measure: Measure) -> int | None:
    projects, ballots, total_budget, cost, approvers = _translate_input_format_temp(instance, profile)
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
