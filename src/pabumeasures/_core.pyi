import enum
from typing import overload

# ========== project enums ==========

class Comparator(enum.Enum):
    COST: Comparator
    VOTES: Comparator
    LEXICOGRAPHIC: Comparator

class Ordering(enum.Enum):
    ASCENDING: Ordering
    DESCENDING: Ordering

# ========== project classes ==========

class ProjectEmbedding:
    def __init__(self, cost: int, name: str = ..., approvers: list[int] = ...) -> None: ...
    @property
    def cost(self) -> int: ...
    @property
    def name(self) -> str: ...
    @property
    def approvers(self) -> list[int]: ...

class ProjectComparator:
    ByCostAsc: ProjectComparator
    ByVotesDesc: ProjectComparator
    ByCostAscThenVotesDesc: ProjectComparator

    @overload
    def __init__(self, criteria: list[tuple[Comparator, Ordering]]) -> None: ...
    @overload
    def __init__(self, comparator: Comparator, ordering: Ordering) -> None: ...
    def __init__(self, *args, **kwargs) -> None: ...
    def __call__(self, lhs: ProjectEmbedding, rhs: ProjectEmbedding) -> bool: ...

# ========== rules ==========

def greedy(
    total_budget: int, projects: list[ProjectEmbedding], tie_breaking: ProjectComparator
) -> list[ProjectEmbedding]: ...
def greedy_over_cost(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]]
) -> list[int]: ...

# ========== optimist-add ==========

def optimist_add_for_greedy(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...
def optimist_add_for_greedy_over_cost(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...

# ========== pessimist-add ==========

def pessimist_add_for_greedy(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...
def pessimist_add_for_greedy_over_cost(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...
