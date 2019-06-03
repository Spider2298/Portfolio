from item import Item
from errors import InvalidOrderError

class Burger:

    def __init__(self, ingredients, buns):
        self._number_of_buns = len(buns)

        if self._number_of_buns < 2:
            raise InvalidOrderError("You must have at least two buns.")

        if len(set(buns)) > 1:
            raise InvalidOrderError("Cannot mix bun types.  Why would you even try this you monster.")

        self._bun = buns[0]

        self._ingredients = tuple(ingredients)
        self._number_of_patties = 0
        for item in self._ingredients:
            if item.name in ("Chicken Patty", "Vegetarian Patty", "Beef Patty"):
                self._number_of_patties += 1
        
        if self._number_of_buns > self._number_of_patties + 1:
            raise InvalidOrderError("You can have at most one more bun than patties.")
        elif self._number_of_patties > 10:
            raise InvalidOrderError("You can't have more than 10 patties.")

        self._items = (*ingredients, *buns)
    
    @property
    def ingredients(self):
        return self._ingredients

    @property
    def bun(self):
        return self._bun

    @property
    def number_of_buns(self):
        return self._number_of_buns

    @property
    def items(self):
        return self._items

    @property
    def price(self):
        return sum([item.price for item in self._items])
