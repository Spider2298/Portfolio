from gourmet_burger_system import GourmetBurgerSystem
from errors import InvalidOrderError, StockSetterError, OutOfStockError
from item import Item, initialise_buns, initialise_ingredients, initialise_drinks, initialise_sides
from wrap import Wrap
from burger import Burger
import sys
import pytest
sys.path.append('../')


@pytest.fixture(scope='class')
def system():
    system = GourmetBurgerSystem()
    for (key, (value, units)) in system._stock_levels.items():
        system._stock_levels[key] = [10000, units]
    return system


@pytest.fixture(scope='class')
def standard_burger(system):
    bun_string_list = ['Muffin bun']*2
    buns = [system.get_object_by_name(bun) for bun in bun_string_list]
    filling_string_list = ['Beef Patty', 'Cheddar Cheese', 'Tomato Sauce']
    fillings = [system.get_object_by_name(
        filling) for filling in filling_string_list]
    burger = system.create_burger(fillings, buns)
    return burger


@pytest.fixture(scope='class')
def standard_order(system, standard_burger):
    id = system.create_new_order()
    order = system.get_order_by_id(id)
    order.burger = standard_burger
    GourmetBurgerSystem.checkout(order)
    return order


class TestSS2:

    def test_set_order_status(self, system, standard_burger, standard_order):
        standard_order.status = "being prepared"
        assert (standard_order.status == "being prepared")
        standard_order.status = "ready"
        assert (standard_order.status == "ready")
        standard_order.status = "collected"
        assert (standard_order.status == "collected")

    def test_set_wrong_order_status(self, system, standard_burger, standard_order):
        with pytest.raises(ValueError) as e:
            standard_order.status = "not right"
        assert "Status must be one of 'selection', 'being prepared', 'ready', or 'collected'." in e.value.args


class TestSS3:

    def test_get_stock_levels(self, system):
        stock_levels = system.stock_levels
        assert (stock_levels['Fries'])
        assert ('grams' in stock_levels['Fries'])
        assert (stock_levels['Nuggets'])
        assert ('pcs' in stock_levels['Nuggets'])
        assert (stock_levels['Orange Juice'])
        assert ('mL' in stock_levels['Orange Juice'])
        assert (stock_levels['600ml Coke'])
        assert ('bottles' in stock_levels['600ml Coke'])
        assert (stock_levels['Vegetarian Patty'])
        assert ('servings' in stock_levels['Vegetarian Patty'])

   
class TestSS4:

    def test_set_stock_levels(self, system):
        stock_levels = system.stock_levels
        assert (stock_levels['Nuggets'])
        messages = system.update_stock_levels({"Nuggets": 5})
        stock_levels = system.stock_levels
        assert(5 in stock_levels["Nuggets"])
        assert (messages['Nuggets'] == 'Stock level updated succesfully')

    def test_set_negative_stock_levels(self, system):
        stock_levels = system.stock_levels
        assert (stock_levels['Nuggets'])
        with pytest.raises(StockSetterError) as e:
            system.update_stock_levels({"Nuggets": -5})
        assert ({'Nuggets': 'Please enter a stock level greater than 0.'} == e.value.errors)

    def test_set_bad_stock_levels(self, system):
        stock_levels = system.stock_levels
        assert (stock_levels['Nuggets'])
        with pytest.raises(StockSetterError) as e:
            system.update_stock_levels({"Nuggets": "So many"})
        assert ({'Nuggets': 'Please enter an integer.'} == e.value.errors)