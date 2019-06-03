import sys
import pytest
sys.path.append('../')
from burger import Burger
from item import Item, initialise_buns, initialise_ingredients
from errors import InvalidOrderError

@pytest.fixture(scope="module")
def test_buns():
    return list(initialise_buns({}))


@pytest.fixture(scope="module")
def test_ingredients():
    return list(initialise_ingredients({}))

def test_burger_constructor(test_buns, test_ingredients):
    test_burger = Burger(test_ingredients, [test_buns[0]]*2)
    assert(test_burger.ingredients == tuple(test_ingredients))
    assert(test_burger.bun == test_buns[0])
    assert(test_burger._number_of_buns == 2)
    assert(test_burger.price == 12)


def test_add_ingredient(test_buns, test_ingredients):
    test_burger = Burger(test_ingredients, [test_buns[0]]*2)
    test_ingredient = test_ingredients[0]
    assert(test_ingredient in test_burger.ingredients)
    assert(test_burger._number_of_patties == 3)

def test_set_num_buns(test_buns, test_ingredients):
    test_burger = Burger(test_ingredients, [test_buns[0]]*3)
    assert(test_burger._number_of_buns == 3)
    assert(test_burger.price == 12.5)

def test_add_incorrect_bun_type(test_buns, test_ingredients):
    
    try:
        test_burger = Burger(test_ingredients, test_buns)
    except InvalidOrderError as e:
        assert("mix bun" in e.msg)

def test_add_too_many_buns(test_buns, test_ingredients):
    
    try:
        test_burger = Burger(test_ingredients, [test_buns[0]]*5)
    except InvalidOrderError as e:
        assert("You can have at most one more bun than patties." in e.msg)

def test_add_too_many_patties(test_buns, test_ingredients):
    
    try:
        test_burger = Burger([test_ingredients[0]]*11, [test_buns[0]]*2)
            
    except InvalidOrderError as e:
        assert ("You can't have more than 10 patties." == e.msg)


    