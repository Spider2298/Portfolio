from item import Item
from errors import InvalidOrderError

class Wrap:

    def __init__(self, ingredients):

        self._ingredients = tuple(ingredients)
        self._number_of_patties = 0
        for item in self._ingredients:
            if item.name in ("Chicken Patty", "Vegetarian Patty", "Beef Patty"):
                self._number_of_patties += 1
        
        if self._number_of_patties > 10:
            raise InvalidOrderError("You can't have more than 10 patties.")
    
    @property
    def ingredients(self):
        return self._ingredients

    @property
    def items(self):
        return self._ingredients

    @property
    def price(self):
        return sum([item.price for item in self.items])