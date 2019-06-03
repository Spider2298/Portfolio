import sys
sys.path.append('../')

import pytest
import item
from errors import OutOfStockError, StockSetterError
from wrap import Wrap
from burger import Burger
from order import Order
from gourmet_burger_system import GourmetBurgerSystem

class TestSystem:

    def setup_method(self):
        self.system = GourmetBurgerSystem()
        assert self.system != None
        assert self.system.drinks != None
        assert self.system.sides != None
        assert self.system.ingredients != None
        assert self.system.buns != None

    def test_create_new_order(self):
        self.order_id = self.system.create_new_order()
        assert self.order_id != None
        assert self.order_id == '0'

    def test_order_exists(self):
        self.order_id = self.system.create_new_order()
        assert self.system.id_exists(self.order_id)
        assert self.system.id_exists("100") == False

    def test_successful_order(self):
        # making an order of every item on the menu
        items = []
        items += self.system.sides
        items += self.system.drinks

        for item in items:
            item.increment(item.quantity_per_serving)

        # creating the order
        self.order_id = self.system.create_new_order()

        order = self.system.get_order_by_id(self.order_id)
        print(self.system._stock_levels)
        order.set_sides(self.system.sides)
        order.set_drinks(self.system.drinks)

        assert order.price == sum([item.price for item in items])
        assert order.status == "selection"
        assert order.sides == tuple(self.system.sides)
        assert order.drinks == tuple(self.system.drinks)

        GourmetBurgerSystem.checkout(order)

        assert order.status == "being prepared"

        #checking the stock levels after placing order
        for (key, (value, units)) in self.system._stock_levels.items():
            assert value == 0
    
    def test_staff_update_order(self):
        self.order_id = self.system.create_new_order()
        order = self.system.get_order_by_id(self.order_id)
        assert order.status == "selection"
        order.status = "ready"

        assert order.status == "ready"

    def test_customer_check_order(self):
        self.order_id = self.system.create_new_order()
        order = self.system.get_order_by_id(self.order_id)

        print(order)

    def test_stock_shortage(self):
        # set stock levels to the minimum ammount required to order all sides and drinks on the menu
        items = []
        items += self.system.sides
        items += self.system.drinks

        for item in items:
            item.increment(1)

        for item in items:
            if item.name == "Large Fries":
                item.stock = item.stock - 1
            if item.name == "Large Orange Juice":
                item.stock = item.stock - 1

        self.order_id = self.system.create_new_order()

        order = self.system.get_order_by_id(self.order_id)

        try:
            order.set_sides(self.system.sides)
        except OutOfStockError as e:
            assert "Large Fries" in e.msg

        try:
            order.set_drinks(self.system.drinks)
        except OutOfStockError as e:
            assert "Large Orange Juice" in e.msg

    def test_two_orders_with_stock(self):
        items = []
        items += self.system.sides
        items += self.system.drinks

        for item in items:
            item.increment(item.quantity_per_serving)
            item.increment(item.quantity_per_serving)

        self.order_id = self.system.create_new_order()
        order1 = self.system.get_order_by_id(self.order_id)
        self.order_id = self.system.create_new_order()
        order2 = self.system.get_order_by_id(self.order_id)
        assert order1 != order2

        # placing order 1 - valid
        order1.set_sides(self.system.sides)
        order1.set_drinks(self.system.drinks)
        assert order1.price == sum([item.price for item in items])
        assert order1.status == "selection"
        assert order1.sides == tuple(self.system.sides)
        assert order1.drinks == tuple(self.system.drinks)
        GourmetBurgerSystem.checkout(order1)
        assert order1.status == "being prepared"

        # placing order 2 - valid
        order2.set_sides(self.system.sides)
        order2.set_drinks(self.system.drinks)
        assert order2.price == sum([item.price for item in items])
        assert order2.status == "selection"
        assert order2.sides == tuple(self.system.sides)
        assert order2.drinks == tuple(self.system.drinks)
        GourmetBurgerSystem.checkout(order2)
        assert order2.status == "being prepared"

    def test_two_orders_when_stock_runs_out(self):
        items = []
        items += self.system.sides
        items += self.system.drinks

        for item in items:
            item.increment(item.quantity_per_serving)

        self.order_id = self.system.create_new_order()
        order1 = self.system.get_order_by_id(self.order_id)
        self.order_id = self.system.create_new_order()
        order2 = self.system.get_order_by_id(self.order_id)
        assert order1 != order2

        # placing order 1 - valid
        order1.set_sides(self.system.sides)
        order1.set_drinks(self.system.drinks)
        assert order1.price == sum([item.price for item in items])
        assert order1.status == "selection"
        assert order1.sides == tuple(self.system.sides)
        assert order1.drinks == tuple(self.system.drinks)
        GourmetBurgerSystem.checkout(order1)
        assert order1.status == "being prepared"

        # placing order 2 - not valid
        try:
            order2.set_sides(self.system.sides)
        except OutOfStockError as e:
            assert "Large Fries" in e.msg

        try:
            order2.set_drinks(self.system.drinks)
        except OutOfStockError as e:
            assert "Large Orange Juice" in e.msg

        assert order2.status == "selection"

    def test_confirmation_print_out(self):
        items = []
        items += self.system.sides
        for item in items:
            item.increment(item.quantity_per_serving)
        self.order_id = self.system.create_new_order()
        order = self.system.get_order_by_id(self.order_id)
        # print(self.system._stock_levels)
        order.set_sides(self.system.sides)
        GourmetBurgerSystem.checkout(order)
        self.system.print_order(self.order_id)

    def test_stock_levels_set(self):

        # Check stock levels setting

        self.system = GourmetBurgerSystem()

        dictionary = {'Nuggets': '3'}

        self.system.update_stock_levels(dictionary)

        assert self.system.stock_levels['Nuggets'] == [3,'pcs']

        dictionary['Nuggets'] = 4

        assert self.system.stock_levels['Nuggets'] == [3,'pcs']

        # test attempted insertion of a new key
        dictionary = {'new_key': 0}

        self.system.update_stock_levels(dictionary)

        print(self.system.stock_levels)

        assert 'new_key' not in self.system.stock_levels

        # test invalid alphabet character
        dictionary = {'Nuggets': 'a'}

        with pytest.raises(StockSetterError) as e:
            self.system.update_stock_levels(dictionary)
        assert e.value.errors['Nuggets'] == 'Please enter an integer.'
        assert self.system.stock_levels['Nuggets'] != ['a','pcs']
        assert self.system.stock_levels['Nuggets'] == [3,'pcs']

        # test negative stock level
        dictionary = {'Nuggets': '-1'}

        with pytest.raises(StockSetterError) as e:
            self.system.update_stock_levels(dictionary)
        assert e.value.errors['Nuggets'] == 'Please enter a stock level greater than 0.'
        assert self.system.stock_levels['Nuggets'] != ['-1','pcs']
        assert self.system.stock_levels['Nuggets'] == [3,'pcs']

        # test valid and invalid input
        dictionary = {
            'Nuggets': '-1',
            'Fries': 5,
        }

        with pytest.raises(StockSetterError) as e:
            self.system.update_stock_levels(dictionary)
        assert e.value.errors['Nuggets'] == 'Please enter a stock level greater than 0.'
        assert self.system.stock_levels['Nuggets'] != ['-1','pcs']
        assert self.system.stock_levels['Fries'] == [5,'grams']

    def test_get_orders(self):
        self.system = GourmetBurgerSystem()

        self.system.create_new_order()
        self.system.create_new_order()

        assert self.system.orders == self.system._order_manager.orders

    def test_lookups(self):

        self.system = GourmetBurgerSystem()

        items = (*self.system.sides, *self.system.drinks, *
                 self.system.ingredients, *self.system.buns)

        for item in items:
            assert item == self.system.get_object_by_name(item.name)


class TestOrderCheckouts:

    def test_creation_ordering_success(self):

        ingredients = item.initialise_ingredients(dict())
        bun = item.initialise_buns(dict())[0]

        self.x = Order(1)

        for ingredient in ingredients:
            ingredient.increment(ingredient.quantity_per_serving*2)

        bun.increment(bun.quantity_per_serving*2)

        burger = Burger(ingredients, [bun]*2)

        wrap = Wrap(ingredients)

        self.x.set_wraps([wrap])
        self.x.set_burgers([burger])

        GourmetBurgerSystem.checkout(self.x)

        assert self.x.status == "being prepared"

        for ingredient in ingredients:
            assert ingredient.stock == 0

        assert bun.stock == 0

        assert self.x.price == wrap.price + burger.price

    def test_creation_out_of_stock(self):

        ingredients = item.initialise_ingredients(dict())
        bun = item.initialise_buns(dict())[0]

        self.x = Order(1)

        for ingredient in ingredients:
            ingredient.increment(ingredient.quantity_per_serving)

        bun.increment(bun.quantity_per_serving)

        burger = Burger(ingredients, [bun]*2)

        wrap = Wrap(ingredients)

        self.x.set_wraps([wrap])
        self.x.set_burgers([burger])

        # check that checking out fails
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.checkout(self.x)

        assert len(e.value.msg) == len(ingredients) + 1

        for ingredient in ingredients:
            assert ingredient.name in e.value.msg

        assert bun.name in e.value.msg

        #check that checking the item list fails
        item_list = list(wrap.ingredients) + list(burger.ingredients) + burger.number_of_buns*[burger.bun]
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.check_item_list(item_list)
        
        assert len(e.value.msg) == len(ingredients) + 1

        for ingredient in ingredients:
            assert ingredient.name in e.value.msg

        assert bun.name in e.value.msg

    def test_sides_out_of_stock(self):
        self.x = Order(1)

        side1 = item.initialise_sides(dict())[0]
        side1.stock = side1.quantity_per_serving - 1

        self.x.set_sides([side1])

        # check that checkout out fails
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.checkout(self.x)

        assert len(e.value.msg) == 1

        assert side1.name in e.value.msg

        # check that checking the item list fails
        item_list = [side1]
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.check_item_list(item_list)

        assert len(e.value.msg) == 1

        assert side1.name in e.value.msg

        assert self.x.sides == (side1,)
        assert self.x.price == side1.price

    def test_drinks_out_of_stock(self):
        self.x = Order(1)

        drink1 = item.initialise_drinks(dict())[0]
        drink1.stock = drink1.quantity_per_serving - 1

        self.x.set_drinks([drink1])

        # check that checkout out fails
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.checkout(self.x)

        assert len(e.value.msg) == 1

        assert drink1.name in e.value.msg

        # check that checking the item list fails
        item_list = [drink1]
        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.check_item_list(item_list)

        assert len(e.value.msg) == 1

        assert drink1.name in e.value.msg

        assert self.x.drinks == (drink1,)
        assert self.x.price == drink1.price
