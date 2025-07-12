import glob

import pytest
from pabutools.election import Cost_Sat, parse_pabulib
from pabutools.rules import greedy_utilitarian_welfare
from pabutools.tiebreaking import min_cost_tie_breaking

import pabumeasures

test_files = glob.glob("./data/*.pb")


@pytest.mark.parametrize("file", test_files)
def test_greedy(file):
    instance, profile = parse_pabulib(file)

    pabutools_result = greedy_utilitarian_welfare(
        instance, profile, sat_class=Cost_Sat, tie_breaking=min_cost_tie_breaking
    )
    result = pabumeasures.greedy(instance, profile)

    assert sorted(pabutools_result) == sorted(result)
