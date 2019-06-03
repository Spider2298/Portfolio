import sys
import pytest
sys.path.append('../')
from burger import Burger
from wrap import Wrap
from item import Item, initialise_buns, initialise_ingredients, initialise_drinks, initialise_sides
from errors import InvalidOrderError, StockSetterError, OutOfStockError
from gourmet_burger_system import GourmetBurgerSystem



@pytest.fixture(scope='class')
def system():
    system = GourmetBurgerSystem()
    for (key, (value, units)) in system._stock_levels.items():
        system._stock_levels[key] = [10000,units]
    return system

@pytest.fixture(scope='class')
def standard_burger(system):
    bun_string_list = ['Muffin bun']*2
    buns = [system.get_object_by_name(bun) for bun in bun_string_list]
    filling_string_list = ['Beef Patty','Cheddar Cheese', 'Tomato Sauce']
    fillings = [system.get_object_by_name(filling) for filling in filling_string_list]
    burger = system.create_burger(fillings, buns)
    return burger

@pytest.fixture(scope='class')
def standard_order(system, standard_burger):
    id = system.create_new_order()
    order = system.get_order_by_id(id)
    order.burger = standard_burger
    GourmetBurgerSystem.checkout(order)
    return order

class TestCS1:
    
    def test_too_few_buns(self, system):
        bun_string_list = ['Sesame bun']
        buns = [system.get_object_by_name(bun) for bun in bun_string_list]
        print(buns)
        with pytest.raises(InvalidOrderError) as e:
            system.create_burger([], buns)
        assert "You must have at least two buns" in e.value.msg

    def test_too_many_buns(self, system):
        bun_string_list = ['Sesame bun']*3
        buns = [system.get_object_by_name(bun) for bun in bun_string_list]
        patty_string_list = ['Beef Patty']
        patties = [system.get_object_by_name(patty) for patty in patty_string_list]
        print(buns, patties)
        with pytest.raises(InvalidOrderError) as e:
            system.create_burger(patties, buns)
        assert "You can have at most one more bun than patties." in e.value.msg

    def test_too_many_patties(self, system):
        bun_string_list = ['Sesame bun']*3
        buns = [system.get_object_by_name(bun) for bun in bun_string_list]
        patty_string_list = ['Beef Patty']*11
        patties = [system.get_object_by_name(patty) for patty in patty_string_list]
        print(buns, patties)
        with pytest.raises(InvalidOrderError) as e:
            system.create_burger(patties, buns)
        assert "You can't have more than 10 patties." in e.value.msg
    
    def test_out_of_stock(self):
        empty_system = GourmetBurgerSystem()
        bun_string_list = ['Sesame bun']
        buns = [empty_system.get_object_by_name(bun) for bun in bun_string_list]

        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.check_item_list(buns)
        assert "Sesame bun" in e.value.msg
    
    def test_successfully_order_everything(self, system):
        bun_string_list = ['Muffin bun']*2
        buns = [system.get_object_by_name(bun) for bun in bun_string_list]
        fillings = system.ingredients

        burger = system.create_burger(fillings, buns)

        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_burgers([burger])

        GourmetBurgerSystem.checkout(order)

        assert order.price == burger.price



    








class TestCS2:

    def test_too_many_patties(self, system):
        patty_string_list = ['Beef Patty']*11
        patties = [system.get_object_by_name(patty) for patty in patty_string_list]
        print(patties)
        with pytest.raises(InvalidOrderError) as e:
            system.create_wrap(patties)
        assert "You can't have more than 10 patties." in e.value.msg
    
    #def test_out_of_stock(self):
        #demonstrated in TestUS1
    
    def test_successfully_order_everything(self, system):
        fillings = system.ingredients

        wrap = system.create_wrap(fillings)

        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_wraps([wrap])

        GourmetBurgerSystem.checkout(order)

        assert order.price == wrap.price
    
    def test_order_buns_and_wraps(self, system):
        fillings = system.ingredients
        sesame_bun = system.buns[0]

        wrap1 = system.create_wrap(fillings)
        wrap2 = system.create_wrap(fillings[:-1])

        burger1 = system.create_burger(fillings, [sesame_bun]*2)
        burger2 = system.create_burger(fillings[:-1], [sesame_bun]*2)

        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_wraps([wrap1, wrap2])
        order.set_burgers([burger1, burger2])


        GourmetBurgerSystem.checkout(order)

        assert order.price == wrap1.price + wrap2.price + burger1.price + burger2.price





class TestCS3_CS10:

    def test_order_everything(self, system, standard_burger):
        sides = system.sides
        
        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_sides(sides)
        order.set_burgers([standard_burger])

        GourmetBurgerSystem.checkout(order)

        assert order.price == sum([side.price for side in system.sides]) + standard_burger.price
    
    def test_out_of_stock(self, standard_burger):
        empty_system = GourmetBurgerSystem()

        sides = empty_system.sides
        
        id = empty_system.create_new_order()
        order = empty_system.get_order_by_id(id)
        order.set_sides(sides)
        order.burger = standard_burger

        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.checkout(order)
        assert e.value.msg == [item.name for item in empty_system.sides]
    
    #def test_order_without_sides(self):
        #demonstrated in TestUS1





class TestCS4:

    def test_order_everything(self, system, standard_burger):
        drinks = system.drinks
        
        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_drinks(drinks)
        order.set_burgers([standard_burger])

        GourmetBurgerSystem.checkout(order)

        assert order.price == sum([drink.price for drink in system.drinks]) + standard_burger.price
        
    
    def test_out_of_stock(self, standard_burger):
        empty_system = GourmetBurgerSystem()

        drinks = empty_system.drinks
        
        id = empty_system.create_new_order()
        order = empty_system.get_order_by_id(id)
        order.set_drinks(drinks)
        order.set_burgers([standard_burger])

        with pytest.raises(OutOfStockError) as e:
            GourmetBurgerSystem.checkout(order)
        assert e.value.msg == [item.name for item in empty_system.drinks]

        
    #def test_order_without_drinks(self):
        #demonstrated in TestUS1





class TestCS5:

    #def display_total_price(self):
        #demonstrated in TestUS1, TestUS2, TestUS3, TestUS4
        
    def test_decrement_stock(self, system):
        drinks = system.drinks
        sides = system.sides
        
        id = system.create_new_order()
        order = system.get_order_by_id(id)
        order.set_drinks(drinks)
        order.set_sides(sides)

        bun_string_list = ['Muffin bun']*2
        buns = [system.get_object_by_name(bun) for bun in bun_string_list]
        fillings = system.ingredients

        burger = system.create_burger(fillings, buns)

        order.set_burgers([burger])

        GourmetBurgerSystem.checkout(order)
        
        #test burger stock decremented correctly
        assert buns[0].stock == 10000 - 2*buns[0].quantity_per_serving
        for filling in fillings:
            assert filling.stock == 10000 - filling.quantity_per_serving

        #test whether sides decremented correctly
        (lf, mf, sf, n20, n10, n6, n3, lcs, mcs, scs, lss, msm, sss) = system.sides
        assert lf.stock == mf.stock == sf.stock
        assert sf.stock == 10000 - lf.quantity_per_serving - mf.quantity_per_serving - sf.quantity_per_serving
        
        assert n20.stock == n10.stock == n6.stock == n3.stock
        assert n3.stock == 10000 - n20.quantity_per_serving - n10.quantity_per_serving - n6.quantity_per_serving - n3.quantity_per_serving

        assert lcs.stock == 10000 - lcs.quantity_per_serving
        assert mcs.stock == 10000 - mcs.quantity_per_serving
        assert scs.stock == 10000 - scs.quantity_per_serving
        assert lss.stock == 10000 - lss.quantity_per_serving
        assert msm.stock == 10000 - msm.quantity_per_serving
        assert sss.stock == 10000 - sss.quantity_per_serving


        #test whether drinks decremented correctly
        (loj, moj, soj, laj, maj, saj, lcj, mcj, scj, lpj, mpj, spj, c600, c350, s600, s350, l600, l350) = system.drinks

        assert loj.stock == moj.stock == soj.stock
        assert soj.stock == 10000 - loj.quantity_per_serving - moj.quantity_per_serving - soj.quantity_per_serving

        assert laj.stock == maj.stock == saj.stock
        assert saj.stock == 10000 - laj.quantity_per_serving - maj.quantity_per_serving - saj.quantity_per_serving

        assert lcj.stock == mcj.stock == scj.stock
        assert scj.stock == 10000 - lcj.quantity_per_serving - mcj.quantity_per_serving - scj.quantity_per_serving

        assert lpj.stock == mpj.stock == spj.stock
        assert spj.stock == 10000 - lpj.quantity_per_serving - mpj.quantity_per_serving - spj.quantity_per_serving

        assert c600.stock == 10000 - c600.quantity_per_serving
        assert c350.stock == 10000 - c350.quantity_per_serving

        assert s600.stock == 10000 - s600.quantity_per_serving
        assert s350.stock == 10000 - s350.quantity_per_serving

        assert l600.stock == 10000 - l600.quantity_per_serving
        assert l350.stock == 10000 - l350.quantity_per_serving
        

class TestCS6:

    def test_customer_check(self, system, standard_order):
        standard_order.status = "ready"

        assert standard_order.status == "ready"

class TestCS8_US9:
    
    def test_standard_burger_items(self, system):
        expected_item_names = ("Beef Patty","Cheddar Cheese","Tomato Sauce","Sesame bun","Sesame bun")

        item_list = []
        system.add_standard_burger_items(item_list)
        assert item_list == [system.get_object_by_name(name) for name in expected_item_names]
    
    def test_standard_wrap_items(self, system):
        expected_item_names = ("Chicken Patty", "Tomato", "Lettuce")

        item_list = []
        system.add_standard_wrap_items(item_list)
        assert item_list == [system.get_object_by_name(name) for name in expected_item_names]