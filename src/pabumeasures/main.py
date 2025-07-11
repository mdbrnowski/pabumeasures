from enum import Enum, auto

from pabutools.election.instance import Instance, Project
from pabutools.election.profile import Profile
from pabutools.rules import BudgetAllocation

from pabumeasures import _core


class Measure(Enum):
    COST_REDUCTION = auto()
    ADD_APPROVAL_OPTIMIST = auto()
    ADD_APPROVAL_PESSIMIST = auto()
    ADD_SINGLETON = auto()


def greedy(instance: Instance, profile: Profile) -> BudgetAllocation:
    return BudgetAllocation(Project(str(i), 1) for i in _core.greedy())


def greedy_measure(instance: Instance, profile: Profile, measure: Measure):
    raise NotImplementedError()


def greedy_measures(instance: Instance, profile: Profile, measures: list[Measure]):
    raise NotImplementedError()


def greedy_over_cost(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def greedy_over_cost_measure(instance: Instance, profile: Profile, measure: Measure):
    raise NotImplementedError()


def greedy_over_cost_measures(instance: Instance, profile: Profile, measures: list[Measure]):
    raise NotImplementedError()


def mes_apr(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def mes_apr_measure(instance: Instance, profile: Profile, measure: Measure):
    raise NotImplementedError()


def mes_apr_measures(instance: Instance, profile: Profile, measures: list[Measure]):
    raise NotImplementedError()


def mes_cost(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def mes_cost_measure(instance: Instance, profile: Profile, measure: Measure):
    raise NotImplementedError()


def mes_cost_measures(instance: Instance, profile: Profile, measures: list[Measure]):
    raise NotImplementedError()


def phragmen(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


def phragmen_measure(instance: Instance, profile: Profile, measure: Measure):
    raise NotImplementedError()


def phragmen_measures(instance: Instance, profile: Profile, measures: list[Measure]):
    raise NotImplementedError()
