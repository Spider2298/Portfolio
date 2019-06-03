from burger import Burger 
from wrap import Wrap 
from item import Item
from errors import OutOfStockError

class Order:

    def __init__(self, id):
        self._id = id
        self._burgers = ()
        self._wraps = ()
        self._sides = ()
        self._drinks = ()
        self._status = "selection"
        
    @property
    def id(self):
        return self._id

    @property
    def burgers(self):
        return self._burgers
    def clear_burgers(self):
        self._burger = ()
    def set_burgers(self, burgers):
        if type(burgers) != list:
            raise TypeError("Please input a list of burgers")
        self._burgers = tuple(burgers)

    @property
    def wraps(self):
        return self._wraps
    def clear_wrap(self):
        self._wrap = None
    def set_wraps(self, wraps):
        if type(wraps) != list:
            raise TypeError("Please input a list of wraps")
        self._wraps = tuple(wraps)

    @property
    def sides(self):
        return self._sides
    def clear_sides(self):
        self._sides = ()
    def set_sides(self, sides):
        if type(sides) != list:
            raise TypeError("Please input a list of sides")
        self._sides = tuple(sides)
    
    @property
    def drinks(self):
        return self._drinks
    def clear_drinks(self):
        self._drinks = ()
    def set_drinks(self, drinks):
        if type(drinks) != list:
            raise TypeError("Please input a list of drinks")
        self._drinks = tuple(drinks)

    @property
    def price(self):
        price = 0
        for burger in self._burgers:
            price += burger.price
        for wrap in self._wraps:
            price += wrap.price
        for side in self._sides:
            price += side.price
        for drink in self._drinks:
            price += drink.price
        return price

    @property
    def status(self):
        return self._status
    @status.setter
    def status(self, status):
        if status not in ("selection", "being prepared", "ready", "collected"):
            raise ValueError("Status must be one of 'selection', 'being prepared', 'ready', or 'collected'.")
        self._status = status
    
    def list_all(self):
        items = []
        for burger in self._burgers:
            items += [burger.bun]*burger.number_of_buns
            items += list(burger.ingredients)
        for wrap in self._wraps:
            items += list(wrap.ingredients)
        items += list(self._sides)
        items += list(self._drinks)
        return items

    def __repr__(self):
        lines = []
        lines.append(type(self).__name__)
        lines.append("id: {}".format(self._id))
        lines.append("status: {}".format(self.status))
        lines.append("Burger: {}".format(self._burgers))
        lines.append("Wrap: {}".format(self._wraps))
        lines.append("Sides: {}".format(self._sides))
        lines.append("Drinks: {}".format(self._drinks))
        lines.append("Price: ${:.2f}".format(self.price))
        return '\n'.join(lines)
