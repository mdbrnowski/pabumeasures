from enum import Enum, auto

from pabutools.election.ballot import FrozenBallot
from pabutools.election.instance import Instance, Project
from pabutools.election.profile import ApprovalProfile, Profile
from pabutools.rules import BudgetAllocation

from pabumeasures import ProjectComparator, _core


class Measure(Enum):
    COST_REDUCTION = auto()
    ADD_APPROVAL_OPTIMIST = auto()
    ADD_APPROVAL_PESSIMIST = auto()
    ADD_SINGLETON = auto()


def _translate_input_format(instance: Instance, profile: Profile) -> tuple[_core.Election, dict[str, Project]]:
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
    total_budget = int(instance.budget_limit)
    approvers: dict[str, list[int]] = {project.name: [] for project in projects}
    for i, frozen_ballot in frozen_ballots:
        for project in frozen_ballot:
            approvers[project.name].append(i)
    project_embeddings: list[_core.ProjectEmbedding] = [
        _core.ProjectEmbedding(int(project.cost), project.name, approvers[project.name]) for project in projects
    ]
    name_to_project: dict[str, Project] = {project.name: project for project in projects}
    return _core.Election(total_budget, len(profile), project_embeddings), name_to_project


def greedy(
    instance: Instance, profile: Profile, tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc
) -> BudgetAllocation:
    election, name_to_project = _translate_input_format(instance, profile)
    result = _core.greedy(election, tie_breaking)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def greedy_measure(
    instance: Instance,
    profile: Profile,
    project: Project,
    measure: Measure,
    tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc,
) -> int | None:
    election, _ = _translate_input_format(instance, profile)
    p = sorted(instance).index(project)
    match measure:
        case Measure.COST_REDUCTION:
            return _core.cost_reduction_for_greedy(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_greedy(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_greedy(election, p, tie_breaking)
        case Measure.ADD_SINGLETON:
            return _core.singleton_add_for_greedy(election, p, tie_breaking)


def greedy_over_cost(
    instance: Instance, profile: Profile, tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc
) -> BudgetAllocation:
    election, name_to_project = _translate_input_format(instance, profile)
    result = _core.greedy_over_cost(election, tie_breaking)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def greedy_over_cost_measure(
    instance: Instance,
    profile: Profile,
    project: Project,
    measure: Measure,
    tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc,
) -> int | None:
    election, _ = _translate_input_format(instance, profile)
    p = sorted(instance).index(project)
    match measure:
        case Measure.COST_REDUCTION:
            return _core.cost_reduction_for_greedy_over_cost(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_greedy_over_cost(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_greedy_over_cost(election, p, tie_breaking)
        case Measure.ADD_SINGLETON:
            return _core.singleton_add_for_greedy_over_cost(election, p, tie_breaking)


def mes_apr(
    instance: Instance, profile: Profile, tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc
) -> BudgetAllocation:
    election, name_to_project = _translate_input_format(instance, profile)
    result = _core.mes_apr(election, tie_breaking)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def mes_apr_measure(
    instance: Instance,
    profile: Profile,
    project: Project,
    measure: Measure,
    tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc,
) -> int | None:
    election, _ = _translate_input_format(instance, profile)
    p = sorted(instance).index(project)
    match measure:
        case Measure.COST_REDUCTION:
            return _core.cost_reduction_for_mes_apr(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_mes_apr(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_mes_apr(election, p, tie_breaking)
        case Measure.ADD_SINGLETON:
            return _core.singleton_add_for_mes_apr(election, p, tie_breaking)


def mes_cost(
    instance: Instance, profile: Profile, tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc
) -> BudgetAllocation:
    election, name_to_project = _translate_input_format(instance, profile)
    result = _core.mes_cost(election, tie_breaking)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def mes_cost_measure(
    instance: Instance,
    profile: Profile,
    project: Project,
    measure: Measure,
    tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc,
) -> int | None:
    election, _ = _translate_input_format(instance, profile)
    p = sorted(instance).index(project)
    match measure:
        case Measure.COST_REDUCTION:
            return _core.cost_reduction_for_mes_cost(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_mes_cost(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_mes_cost(election, p, tie_breaking)
        case Measure.ADD_SINGLETON:
            return _core.singleton_add_for_mes_cost(election, p, tie_breaking)


def phragmen(
    instance: Instance, profile: Profile, tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc
) -> BudgetAllocation:
    election, name_to_project = _translate_input_format(instance, profile)
    result = _core.phragmen(election, tie_breaking)
    return BudgetAllocation(name_to_project[project_embeding.name] for project_embeding in result)


def phragmen_measure(
    instance: Instance,
    profile: Profile,
    project: Project,
    measure: Measure,
    tie_breaking: ProjectComparator = ProjectComparator.ByCostAsc,
) -> int | None:
    election, _ = _translate_input_format(instance, profile)
    p = sorted(instance).index(project)
    match measure:
        case Measure.COST_REDUCTION:
            return _core.cost_reduction_for_phragmen(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_OPTIMIST:
            return _core.optimist_add_for_phragmen(election, p, tie_breaking)
        case Measure.ADD_APPROVAL_PESSIMIST:
            return _core.pessimist_add_for_phragmen(election, p, tie_breaking)
        case Measure.ADD_SINGLETON:
            return _core.singleton_add_for_phragmen(election, p, tie_breaking)
