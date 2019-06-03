import item
import pickle
from order_manager import OrderManager
from errors import StockSetterError, OutOfStockError
from wrap import Wrap
from burger import Burger


class GourmetBurgerSystem:

    def __init__(self):
        self._stock_levels = {}
        self._sides = item.initialise_sides(self._stock_levels)
        self._drinks = item.initialise_drinks(self._stock_levels)
        self._ingredients = item.initialise_ingredients(self._stock_levels)
        self._buns = item.initialise_buns(self._stock_levels)
        self._order_manager = OrderManager()
        self._lookup = {}
        obj_list = (*self._sides, *self._drinks, *
                    self._ingredients, *self._buns)
        for obj in obj_list:
            self._lookup[obj.name] = obj
        

        standard_burger_buns = [self.get_object_by_name("Sesame bun")]*2
        standard_burger_fillings = [self.get_object_by_name(name) for name in ("Beef Patty", "Cheddar Cheese", "Tomato Sauce")]
        standard_wrap_fillings = [self.get_object_by_name(name) for name in ("Chicken Patty", "Tomato", "Lettuce")]

        self._standard_burger = Burger(standard_burger_fillings, standard_burger_buns)
        self._standard_wrap = Wrap(standard_wrap_fillings)

    @property
    def sides(self):
        return list(self._sides)

    def get_object_by_name(self, name):
        return self._lookup[name]

    @property
    def drinks(self):
        return list(self._drinks)

    @property
    def ingredients(self):
        return list(self._ingredients)

    @property
    def buns(self):
        return list(self._buns)

    @property
    def stock_levels(self):
        return dict(self._stock_levels)

    @property
    def orders(self):
        return self._order_manager.orders

    @property
    def standard_burger(self):
        return self._standard_burger

    @property
    def standard_wrap(self):
        return self._standard_wrap

    def update_stock_levels(self, stock_levels):

        processed_stock_levels = {}
        errors = {}
        messages = {}

        for (key, value) in stock_levels.items():
            if key not in self._stock_levels:
                continue
            try:
                new_value = int(value)
            except ValueError:
                errors[key] = 'Please enter an integer.'
            else:
                if new_value < 0:
                    errors[key] = 'Please enter a stock level greater than 0.'
                elif new_value != self._stock_levels[key][0]:
                    processed_stock_levels[key] = [new_value, self._stock_levels[key][1]]
                    messages[key] = 'Stock level updated succesfully'

        if errors:
            self._stock_levels.update(processed_stock_levels)
            raise StockSetterError(errors, messages)
        else:
            self._stock_levels.update(processed_stock_levels)
            return messages

    def create_new_order(self):
        return self._order_manager.create_new_order()

    def id_exists(self, id):
        return self._order_manager.id_exists(id)

    def get_order_by_id(self, id):
        return self._order_manager.get_order_by_id(id)

    def print_order(self, id):
        print(self._order_manager.get_order_by_id(id))

    @classmethod
    def load(cls):
        try:
            with open("database", "rb") as file:
                system = pickle.load(file)
        except FileNotFoundError:
            system = GourmetBurgerSystem()
        return system

    def save(self):
        with open("database", "wb") as file:
            pickle.dump(self, file)

    @classmethod
    def checkout(cls, order):

        errors = []

        items = order.list_all()

        cls._increment_all(-1, items)

        items_no_duplicates = list(dict.fromkeys(items))

        errors += cls._list_of_names_with_less_than_zero(items_no_duplicates)

        if errors:
            cls._increment_all(1, items)

            raise OutOfStockError(errors)
        else:
            order.status = "being prepared"

    @classmethod
    def check_item_list(cls, items):
        cls._increment_all(-1, items)

        errors = cls._list_of_names_with_less_than_zero(items)

        cls._increment_all(1, items)

        errors = list(dict.fromkeys(errors))

        if errors:
            raise OutOfStockError(errors)

    @classmethod
    def _list_of_names_with_less_than_zero(cls, items):
        shortages = []
        for item in items:
            if item.stock < 0:
                shortages.append(item.name)
        return shortages

    @classmethod
    def _increment_all(cls, sign, items):
        for item in items:
            item.increment(sign*item.quantity_per_serving)

    def create_burger(self, ingredients, buns):
        return Burger(ingredients, buns)
    
    def create_wrap(self, ingredients):
        return Wrap(ingredients)

    def add_standard_burger_items(self, item_list):
        for item in self.standard_burger.ingredients:
            item_list.append(item)
        item_list += [self.standard_burger.bun]*self.standard_burger.number_of_buns

    def add_standard_wrap_items(self, item_list):
        for item in self.standard_wrap.ingredients:
            item_list.append(item)
