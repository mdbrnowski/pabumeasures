from enum import Enum, auto

from pabutools.election.instance import Instance
from pabutools.election.profile import Profile
from pabutools.rules import BudgetAllocation

from pabumeasures._core import hello_from_bin


class Measure(Enum):
    COST_REDUCTION = auto()
    ADD_APPROVAL_OPTIMIST = auto()
    ADD_APPROVAL_PESSIMIST = auto()
    ADD_SINGLETON = auto()


def hello() -> str:
    return hello_from_bin()


def greedy(instance: Instance, profile: Profile) -> BudgetAllocation:
    raise NotImplementedError()


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
