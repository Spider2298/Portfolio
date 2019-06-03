

import sys
import pytest
sys.path.append('../')
from wrap import Wrap
from item import Item, initialise_ingredients
from errors import InvalidOrderError


@pytest.fixture(scope="module")
def test_ingredients():
    return list(initialise_ingredients({}))

def test_wrap_constructor(test_ingredients):
    test_wrap = Wrap(test_ingredients)
    assert(test_wrap.ingredients == tuple(test_ingredients))
    assert(test_wrap.price == 11)


