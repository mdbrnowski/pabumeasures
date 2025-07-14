# ========== rules ==========

def greedy(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]]
) -> list[int]: ...
def greedy_over_cost(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]]
) -> list[int]: ...

# ========== optimist-add ==========

def optimist_add_for_greedy(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...

# ========== pessimist-add ==========

def pessimist_add_for_greedy(
    num_projects: int, num_voters: int, total_budget: int, cost: list[int], approvers: list[list[int]], p: int
) -> int | None: ...
