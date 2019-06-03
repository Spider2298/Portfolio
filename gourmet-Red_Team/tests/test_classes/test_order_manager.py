import sys
sys.path.append('../')

from order_manager import OrderManager
from order import Order

import pytest

class Test:

    def test_init(self):
        self.x = OrderManager()

        assert type(self.x._orders) == dict
        assert self.x._up_to_id == 0
    
    def test_setters(self):
        self.x = OrderManager()

        id1 = self.x.create_new_order()

        assert len(self.x.orders) == 1
        assert type(self.x.orders[0]) == Order
        assert type(self.x.get_order_by_id(id1)) == Order

        id2 = self.x.create_new_order()

        assert len(self.x.orders) == 2

    def test_checker(self):
        #we expect the first order created to have an id of 0
        self.x = OrderManager()

        assert self.x.id_exists('0') == False

        self.x.create_new_order()

        assert self.x.id_exists('0') == True

    def test_invalid_cases(self):
        self.x = OrderManager()

        with pytest.raises(TypeError):
            self.x.get_order_by_id('1.1')
        
        with pytest.raises(TypeError):
            self.x.get_order_by_id(1.1)
        
        with pytest.raises(KeyError):
            self.x.get_order_by_id('1')
        
        assert self.x.id_exists('a') == False