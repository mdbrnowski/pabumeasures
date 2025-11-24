import pytest
from pabutools.election import ApprovalBallot, ApprovalProfile, Instance, Project

import pabumeasures


def test_error_on_empty_instance():
    instance = Instance([], 10)
    profile = ApprovalProfile(
        [
            ApprovalBallot([]),
        ]
    )

    with pytest.raises(ValueError, match=r".+ at least one project"):
        pabumeasures.greedy(instance, profile)


def test_error_on_empty_profile():
    p1 = Project("p1", 2)
    p2 = Project("p2", 1)
    instance = Instance([p1, p2], 10)
    profile = ApprovalProfile([])

    with pytest.raises(ValueError, match=r".+ at least one ballot"):
        pabumeasures.greedy(instance, profile)


def test_error_on_same_names():
    p1 = Project("p", 2)
    p2 = Project("p", 1)
    instance = Instance([p1, p2], 2)
    if len(instance) != 2:
        # pabutools removes duplicates
        assert len(instance) == 1
    else:
        # if pabutools does not remove duplicates (for some reason), we should raise an error
        profile = ApprovalProfile(
            [
                ApprovalBallot([p1]),
                ApprovalBallot([p2]),
                ApprovalBallot([p1, p2]),
            ]
        )

        with pytest.raises(ValueError, match=r"names .+ unique"):
            pabumeasures.greedy(instance, profile)


def test_error_on_negative_cost():
    p1 = Project("p1", -2)
    p2 = Project("p2", 1)
    instance = Instance([p1, p2], 2)
    profile = ApprovalProfile(
        [
            ApprovalBallot([p1]),
            ApprovalBallot([p2]),
            ApprovalBallot([p1, p2]),
        ]
    )

    with pytest.raises(ValueError, match=r"costs .+ positive"):
        pabumeasures.greedy(instance, profile)


def test_error_on_cost_larger_than_budget():
    p1 = Project("p1", 11)
    p2 = Project("p2", 1)
    instance = Instance([p1, p2], 10)
    profile = ApprovalProfile(
        [
            ApprovalBallot([p1]),
            ApprovalBallot([p2]),
        ]
    )

    with pytest.raises(ValueError, match=r"costs .+ exceed"):
        pabumeasures.greedy(instance, profile)


def test_error_on_huge_budget():
    p1 = Project("p1", 2)
    p2 = Project("p2", 1)
    instance = Instance([p1, p2], int(2e9))
    profile = ApprovalProfile(
        [
            ApprovalBallot([p1]),
            ApprovalBallot([p2]),
        ]
    )

    with pytest.raises(ValueError, match=r"[Bb]udget limit .+ exceed"):
        pabumeasures.greedy(instance, profile)
