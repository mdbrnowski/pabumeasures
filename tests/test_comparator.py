import random
import string
from dataclasses import dataclass

import pytest

from pabumeasures._core import ASCENDING, COST, DESCENDING, LEXICOGRAPHIC, VOTES, Project, ProjectComparator


def random_project(min_cost, max_cost, name_length=5, max_approvers=3):
    cost = random.randint(min_cost, max_cost)
    name = "".join(random.choices(string.ascii_letters, k=name_length))
    approvers = random.sample(range(max_approvers), random.randint(0, max_approvers))
    return Project(cost, name, approvers)


projects = [random_project(1, 3) for _ in range(200)]


@dataclass(frozen=True)
class ComparatorTestCase:
    key_func: callable
    reverse: bool
    comparator: object
    id: str


test_cases = [
    ComparatorTestCase(lambda p: p.cost, False, ProjectComparator.ByCostAsc, "ByCostAsc"),
    ComparatorTestCase(lambda p: p.cost, False, ProjectComparator(COST, ASCENDING), "ByCostAsc_explicit"),
    ComparatorTestCase(lambda p: p.cost, True, ProjectComparator(COST, DESCENDING), "ByCostDesc"),
    ComparatorTestCase(lambda p: len(p.approvers), True, ProjectComparator.ByVotesDesc, "ByVotesDesc"),
    ComparatorTestCase(lambda p: len(p.approvers), True, ProjectComparator(VOTES, DESCENDING), "ByVotesDesc_explicit"),
    ComparatorTestCase(lambda p: len(p.approvers), False, ProjectComparator(VOTES, ASCENDING), "ByVotesAsc"),
    ComparatorTestCase(
        lambda p: (p.cost, -len(p.approvers)), False, ProjectComparator.ByCostAscThenVotesDesc, "ByCostAscThenVotesDesc"
    ),
    ComparatorTestCase(
        lambda p: (p.cost, -len(p.approvers)),
        False,
        ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING)]),
        "ByCostAscThenVotesDesc_explicit",
    ),
    ComparatorTestCase(
        lambda p: (p.cost, -len(p.approvers), p.name),
        False,
        ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING), (LEXICOGRAPHIC, ASCENDING)]),
        "ByCostVotesLex",
    ),
]


@pytest.mark.parametrize("case", test_cases, ids=lambda c: c.id)
def test_project_comparator_basic(case: ComparatorTestCase):
    sorted_projects = list(projects)
    sorted_projects.sort(key=case.key_func, reverse=case.reverse)

    for i in range(len(sorted_projects)):
        for j in range(i + 1, len(sorted_projects)):
            same_key = case.key_func(sorted_projects[i]) == case.key_func(sorted_projects[j])

            if same_key:
                assert not case.comparator(sorted_projects[i], sorted_projects[j])
                assert not case.comparator(sorted_projects[j], sorted_projects[i])
            else:
                assert case.comparator(sorted_projects[i], sorted_projects[j]) != case.comparator(
                    sorted_projects[j], sorted_projects[i]
                )
                assert case.comparator(sorted_projects[i], sorted_projects[j])
