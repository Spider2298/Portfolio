import sys
sys.path.append('../')

from item import Item

import pytest

class TestItem:
    


    def test_init(self):
        self.x = Item("name", 3, 6, "unit",dict())
        assert self.x.name == "name"
        assert self.x.price == 3
        assert self.x.quantity_per_serving == 6
        assert self.x.quantity_units == "unit"
        assert self.x.stock == 0
    
    def test_setters_fail(self):
        #none of these can be set through the interface
        self.x = Item("name", 3, 6, "unit",dict())
        with pytest.raises(AttributeError):
            self.x.name = 'hello'
        with pytest.raises(AttributeError):
            self.x.price = 3
        with pytest.raises(AttributeError):
            self.x.quantity_per_serving = 3
        with pytest.raises(AttributeError):
            self.x.quantity_units = 'unit2'
        
    def test_stock_setters(self):
        self.x = Item("name", 3, 6, "unit",dict())

        with pytest.raises(TypeError):
            self.x.stock = 'string'
        with pytest.raises(ValueError):
            self.x.stock = -5
        
        self.x.stock = 10
        assert self.x.stock == 10

        self.x.increment(5)
        assert self.x.stock == 15
        self.x.increment(-5)
        assert self.x.stock == 10
    

    def test__multi_stock_setters(self):
        stock_levels_dict = dict()
        self.x = Item("3-Pack Nuggets", 3, 3, "pcs", stock_levels_dict, "Nuggets")
        self.y = Item("6-Pack Nuggets", 6, 6, "pcs", stock_levels_dict, "Nuggets")
        self.z = Item("Small Fries", 3, 100, "grams", stock_levels_dict, "Fries")

        self.x.stock = 10
        self.z.stock = 100
        assert self.y.stock == 10
        assert self.z.stock == 100

    def test_repr(self):
        self.x = Item("Nuggets", 3, 6, "pcs",dict())
        self.x.stock = 5
        assert str(self.x) == "Item <name: Nuggets, price: $3.00, quantity_per_serving: 6pcs>"