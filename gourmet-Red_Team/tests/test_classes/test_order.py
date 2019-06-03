import sys
sys.path.append('../')

from order import Order
from errors import OutOfStockError
from burger import Burger
from wrap import Wrap

import pytest
import item


class TestOrder:

    def test_init(self):
        self.x = Order(1)

        assert self.x.id == 1

    def test_setters_fail(self):
        self.x = Order(1)
        with pytest.raises(AttributeError):
            self.x.id = 2
        with pytest.raises(TypeError):
            self.x.set_burgers(2)
        with pytest.raises(TypeError):
            self.x.set_wraps(2)
        with pytest.raises(AttributeError):
            self.x.sides = 2
        with pytest.raises(AttributeError):
            self.x.drinks = 2
        with pytest.raises(ValueError):
            self.x.status = 1
        with pytest.raises(ValueError):
            self.x.status = "hello"

    def test_setters_success(self):
        self.x = Order(1)
        sides = item.initialise_sides(dict())
        drinks = item.initialise_drinks(dict())

        side1 = sides[0]
        side1.stock = 10000

        self.x.set_sides([side1])

        assert self.x.sides == (side1,)
        assert self.x.price == side1.price
        
        side2 = sides[1]
        side2.stock = 10000

        self.x.set_sides([side1, side2])

        assert self.x.sides == (side1, side2)
        assert self.x.price == side1.price + side2.price


        drink1 = drinks[0]
        drink1.stock = 10000

        self.x.set_drinks([drink1])

        assert self.x.drinks == (drink1,)
        assert self.x.price == drink1.price + side1.price + side2.price

        drink2 = drinks[1]
        drink2.stock = 10000

        self.x.set_drinks([drink1, drink2])

        assert self.x.drinks == (drink1, drink2)
        assert self.x.price == drink1.price + drink2.price + side1.price + side2.price

        self.x.clear_sides()

        assert self.x.sides == ()
        assert self.x.drinks == (drink1, drink2)
        assert self.x.price == drink1.price + drink2.price

        self.x.clear_drinks()

        assert self.x.sides == ()
        assert self.x.drinks == ()
        assert self.x.price == 0

        self.x.status = "selection"
        assert self.x.status == "selection"
        self.x.status = "being prepared"
        assert self.x.status == "being prepared"
        self.x.status = "ready"
        assert self.x.status == "ready"
        self.x.status = "collected"
        assert self.x.status == "collected"
    
    def test_repr(self):
        self.x = Order(1)
        lines = []
        lines.append("Order")
        lines.append("id: 1")
        lines.append("status: selection")
        lines.append("Burger: ()")
        lines.append("Wrap: ()")
        lines.append("Sides: ()")
        lines.append("Drinks: ()")
        lines.append("Price: $0.00")
        
        assert str(self.x) == '\n'.join(lines)

        side1 = item.initialise_sides(dict())[0]
        side1.stock = 10000
        self.x.set_sides([side1])
        
        lines[5] = "Sides: {}".format(tuple([side1]))
        lines[7] = "Price: ${:.2f}".format(side1.price)
        
        assert str(self.x) == '\n'.join(lines)