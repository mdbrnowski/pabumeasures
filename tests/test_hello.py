import pytest
from pabutools.election.instance import Instance
from pabutools.election.profile import ApprovalProfile

from pabumeasures import Measure, hello, mes_apr_measures


def test_hello():
    assert hello() == "Hello from pabumeasures!"


def test_mes_apr_measures_raises():
    instance = Instance()
    profile = ApprovalProfile()

    with pytest.raises(NotImplementedError):
        mes_apr_measures(instance, profile, [Measure.COST_REDUCTION, Measure.ADD_APPROVAL_OPTIMIST])
