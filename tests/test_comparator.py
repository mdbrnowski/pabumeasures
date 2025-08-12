import random
import string

from pabumeasures._core import ASCENDING, COST, DESCENDING, LEXICOGRAPHIC, VOTES, Project, ProjectComparator


def random_project(min_cost, max_cost, name_length=5, max_approvers=3):
    cost = random.randint(min_cost, max_cost)
    name = "".join(random.choices(string.ascii_letters, k=name_length))
    approvers = random.sample(range(max_approvers), random.randint(0, max_approvers))
    return Project(cost, name, approvers)


def test_project_comparator_basic():
    projects = [random_project(1, 3) for _ in range(200)]

    test_cases = [
        # (key function, reverse flag, comparator instance)
        (lambda p: p.cost, False, ProjectComparator.ByCostAsc),
        (lambda p: p.cost, False, ProjectComparator(COST, ASCENDING)),
        (lambda p: p.cost, True, ProjectComparator(COST, DESCENDING)),
        (lambda p: len(p.approvers), True, ProjectComparator.ByVotesDesc),
        (lambda p: len(p.approvers), True, ProjectComparator(VOTES, DESCENDING)),
        (lambda p: len(p.approvers), False, ProjectComparator(VOTES, ASCENDING)),
        (lambda p: (p.cost, -len(p.approvers)), False, ProjectComparator.ByCostAscThenVotesDesc),
        (lambda p: (p.cost, -len(p.approvers)), False, ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING)])),
        (
            lambda p: (p.cost, -len(p.approvers), p.name),
            False,
            ProjectComparator([(COST, ASCENDING), (VOTES, DESCENDING), (LEXICOGRAPHIC, ASCENDING)]),
        ),
    ]

    for key_func, reverse, comparator in test_cases:
        projects.sort(key=key_func, reverse=reverse)

        for i in range(len(projects)):
            for j in range(i + 1, len(projects)):
                same_key = key_func(projects[i]) == key_func(projects[j])

                if same_key:
                    assert not comparator(projects[i], projects[j])
                    assert not comparator(projects[j], projects[i])
                else:
                    assert comparator(projects[i], projects[j]) != comparator(projects[j], projects[i])
                    assert comparator(projects[i], projects[j]) or comparator(projects[j], projects[i])
