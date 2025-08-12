import random
import string

import pytest

from pabumeasures._core import ASCENDING, COST, DESCENDING, LEXICOGRAPHIC, VOTES, Project, ProjectComparator


def random_project(min_cost, max_cost, name_length=5, max_approvers=3):
    cost = random.randint(min_cost, max_cost)
    name = "".join(random.choices(string.ascii_letters, k=name_length))
    approvers = random.sample(range(max_approvers), random.randint(0, max_approvers))
    return Project(cost, name, approvers)


projects = [random_project(1, 3) for _ in range(200)]

test_cases = [
    (lambda p: p.cost, False, ProjectComparator.ByCostAsc, "ByCostAsc"),
    (lambda p: p.cost, False, ProjectComparator(COST, ASCENDING), "ByCostAsc_explicit"),
    (lambda p: p.cost, True, ProjectComparator(COST, DESCENDING), "ByCostDesc"),
    (lambda p: len(p.approvers), True, ProjectComparator.ByVotesDesc, "ByVotesDesc"),
    (lambda p: len(p.approvers), True, ProjectComparator(VOTES, DESCENDING), "ByVotesDesc_explicit"),
    (lambda p: len(p.approvers), False, ProjectComparator(VOTES, ASCENDING), "ByVotesAsc"),
    (lambda p: (p.cost, -len(p.approvers)), False, ProjectComparator.ByCostAscThenVotesDesc, "ByCostAscThenVotesDesc"),
    (
        lambda p: (p.cost, -len(p.approvers)),
        False,
        ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING)]),
        "ByCostAscThenVotesDesc_explicit",
    ),
    (
        lambda p: (p.cost, -len(p.approvers), p.name),
        False,
        ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING), (LEXICOGRAPHIC, ASCENDING)]),
        "ByCostVotesLex",
    ),
]


@pytest.mark.parametrize("key_func,reverse,comparator,name", test_cases)
def test_project_comparator_basic(key_func, reverse, comparator, name):
    sorted_projects = list(projects)
    sorted_projects.sort(key=key_func, reverse=reverse)

    for i in range(len(sorted_projects)):
        for j in range(i + 1, len(sorted_projects)):
            same_key = key_func(sorted_projects[i]) == key_func(sorted_projects[j])

            if same_key:
                assert not comparator(sorted_projects[i], sorted_projects[j])
                assert not comparator(sorted_projects[j], sorted_projects[i])
            else:
                assert comparator(sorted_projects[i], sorted_projects[j]) != comparator(
                    sorted_projects[j], sorted_projects[i]
                )
                assert comparator(sorted_projects[i], sorted_projects[j])
