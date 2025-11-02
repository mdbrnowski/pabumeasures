from pabutools.election import ApprovalBallot, ApprovalProfile, Instance, Project

from pabumeasures import Measure, mes_cost, mes_cost_measure


def test_example():
    p1 = Project("p1", 1)
    p2 = Project("p2", 1)
    p3 = Project("p3", 3)

    b1 = ApprovalBallot([p1, p2])
    b2 = ApprovalBallot([p1, p2, p3])
    b3 = ApprovalBallot([p3])

    instance = Instance([p1, p2, p3], budget_limit=3)
    profile = ApprovalProfile([b1, b2, b3])

    assert mes_cost(instance, profile) == [p1, p2]
    assert mes_cost_measure(instance, profile, p3, Measure.ADD_APPROVAL_OPTIMIST) == 1
