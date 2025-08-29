import random
import string

import pytest

from pabumeasures._core import Comparator, Ordering, ProjectComparator, ProjectEmbedding


def random_project_embedding(
    min_cost: int, max_cost: int, name_length: int = 5, max_approvers: int = 3
) -> ProjectEmbedding:
    cost = random.randint(min_cost, max_cost)
    name = "".join(random.choices(string.ascii_letters, k=name_length))
    approvers = random.sample(range(max_approvers), random.randint(0, max_approvers))
    return ProjectEmbedding(cost, name, approvers)


random.seed(42)
projects = [random_project_embedding(1, 3) for _ in range(200)]

test_cases = [
    (lambda p: (p.cost, p.name), ProjectComparator.ByCostAsc, "ByCostAsc"),
    (lambda p: (p.cost, p.name), ProjectComparator(Comparator.COST, Ordering.ASCENDING), "ByCostAsc_explicit"),
    (lambda p: (-p.cost, p.name), ProjectComparator.ByCostDesc, "ByCostDesc"),
    (lambda p: (-p.cost, p.name), ProjectComparator(Comparator.COST, Ordering.DESCENDING), "ByCostDesc_explicit"),
    (lambda p: (p.name), ProjectComparator.ByNameAsc, "ByNameAsc"),
    (lambda p: (p.name), ProjectComparator([]), "ByNameAsc_implicit"),
    (lambda p: (p.name), ProjectComparator(Comparator.LEXICOGRAPHIC, Ordering.ASCENDING), "ByNameAsc_explicit"),
    (lambda p: "".join(chr(255 - ord(c)) for c in p.name), ProjectComparator.ByNameDesc, "ByNameDesc"),
    (
        lambda p: "".join(chr(255 - ord(c)) for c in p.name),
        ProjectComparator(Comparator.LEXICOGRAPHIC, Ordering.DESCENDING),
        "ByNameDesc_explicit",
    ),
    (
        lambda p: (len(p.approvers), p.name),
        ProjectComparator(Comparator.VOTES, Ordering.ASCENDING),
        "ByVotesAsc_explicit",
    ),
    (lambda p: (-len(p.approvers), p.name), ProjectComparator.ByVotesDesc, "ByVotesDesc"),
    (
        lambda p: (-len(p.approvers), p.name),
        ProjectComparator(Comparator.VOTES, Ordering.DESCENDING),
        "ByVotesDesc_explicit",
    ),
    (lambda p: (p.cost, -len(p.approvers), p.name), ProjectComparator.ByCostAscThenVotesDesc, "ByCostAscThenVotesDesc"),
    (
        lambda p: (p.cost, -len(p.approvers), p.name),
        ProjectComparator([(Comparator.COST, Ordering.ASCENDING), (Comparator.VOTES, Ordering.DESCENDING)]),
        "ByCostAscThenVotesDesc_explicit",
    ),
    (
        lambda p: (-p.cost, -len(p.approvers), p.name),
        ProjectComparator.ByCostDescThenVotesDesc,
        "ByCostDescThenVotesDesc",
    ),
    (
        lambda p: (-p.cost, -len(p.approvers), p.name),
        ProjectComparator([(Comparator.COST, Ordering.DESCENDING), (Comparator.VOTES, Ordering.DESCENDING)]),
        "ByCostDescThenVotesDesc_explicit",
    ),
    (
        lambda p: (p.cost, -len(p.approvers), p.name),
        ProjectComparator(
            [
                (Comparator.COST, Ordering.ASCENDING),
                (Comparator.VOTES, Ordering.DESCENDING),
                (Comparator.LEXICOGRAPHIC, Ordering.ASCENDING),
            ]
        ),
        "ByCostVotesLex",
    ),
]


@pytest.mark.parametrize("key_func,comparator,name", test_cases)
def test_project_comparator_basic(key_func, comparator, name):
    sorted_projects = list(projects)
    sorted_projects.sort(key=key_func)

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
