import random
import string

import pytest

from pabumeasures._core import Comparator, Ordering, ProjectEmbedding, ProjectComparator


def random_project(id: int, min_cost: int, max_cost: int, name_length: int = 5, max_approvers: int = 3):
    cost = random.randint(min_cost, max_cost)
    name = "".join(random.choices(string.ascii_letters, k=name_length))
    approvers = random.sample(range(max_approvers), random.randint(0, max_approvers))
    return ProjectEmbedding(cost, id, name, approvers)


projects = [random_project(i, 1, 3) for i in range(200)]

test_cases = [
    (lambda p: p.cost, False, ProjectComparator.ByCostAsc, "ByCostAsc"),
    (lambda p: p.cost, False, ProjectComparator(Comparator.COST, Ordering.ASCENDING), "ByCostAsc_explicit"),
    (lambda p: p.cost, True, ProjectComparator(Comparator.COST, Ordering.DESCENDING), "ByCostDesc"),
    (lambda p: p.id, False, ProjectComparator(Comparator.ID, Ordering.ASCENDING), "ByIdAsc"),
    (lambda p: p.id, True, ProjectComparator(Comparator.ID, Ordering.DESCENDING), "ByIdDesc"),
    (lambda p: len(p.approvers), True, ProjectComparator.ByVotesDesc, "ByVotesDesc"),
    (
        lambda p: len(p.approvers),
        True,
        ProjectComparator(Comparator.VOTES, Ordering.DESCENDING),
        "ByVotesDesc_explicit",
    ),
    (lambda p: len(p.approvers), False, ProjectComparator(Comparator.VOTES, Ordering.ASCENDING), "ByVotesAsc"),
    (lambda p: (p.cost, -len(p.approvers)), False, ProjectComparator.ByCostAscThenVotesDesc, "ByCostAscThenVotesDesc"),
    (
        lambda p: (p.cost, -len(p.approvers)),
        False,
        ProjectComparator([(Comparator.COST, Ordering.ASCENDING), (Comparator.VOTES, Ordering.DESCENDING)]),
        "ByCostAscThenVotesDesc_explicit",
    ),
    (
        lambda p: (p.cost, -len(p.approvers), p.name),
        False,
        ProjectComparator(
            [
                (Comparator.COST, Ordering.ASCENDING),
                (Comparator.VOTES, Ordering.DESCENDING),
                (Comparator.LEXICOGRAPHIC, Ordering.ASCENDING),
            ]
        ),
        "ByCostVotesLex",
    ),
    (
        lambda p: (p.cost, p.id),
        False,
        ProjectComparator.ByCostAscThenIdAsc,
        "ByCostId",
    ),
    (
        lambda p: (p.cost, p.id),
        False,
        ProjectComparator(
            [
                (Comparator.COST, Ordering.ASCENDING),
                (Comparator.ID, Ordering.ASCENDING),
            ]
        ),
        "ByCostId_explicit",
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
