from flask import Flask, render_template, request, redirect, url_for, Markup, session

from server import system, app

#to do
#refactor code to make it a lot nicer

from gourmet_burger_system import GourmetBurgerSystem

from errors import StockSetterError, OutOfStockError, InvalidOrderError

from item import *

class SessionManager:

    _id = 0
    orders = {}

    @classmethod
    def fetch_items(cls):
        if 'order_id' not in session or session['order_id'] not in cls.orders:
            session['order_id'] = cls._id
            cls.orders[cls._id] = []
            cls._id += 1
        
        id = session['order_id']
        return cls.orders[id]
    
    @classmethod
    def start_new_session(cls):
        session['order_id'] = cls._id
        cls.orders[cls._id] = []
        cls._id += 1
        


def display(object, **kwargs):
    object_name = type(object).__name__.lower()
    context = {
        'display':display,
        object_name: object,
    }
    return Markup(render_template(object_name+'_display.html', **context, **kwargs))

@app.route('/', methods=["GET", "POST"])
def index():
    return render_template('index.html')

@app.route('/order/', methods=["GET", "POST"])
def order():
    if (request.method == 'POST'):
        SessionManager.start_new_session()
        return redirect(url_for('select'))
    return render_template('order.html')

def clean_input(input):
    if input in ('', 'Confirm Order', 'Check Order', 'Cancel', None):
        return 0
    value = int(input)
    if value < 0: raise ValueError
    return value

def sort_items(request):
    request_dict = request.form.to_dict()
    selection = request.form['main']
    input_errors = {}

    items_dict = {
        'sides_list' : [],
        'drinks_list' : [],
        'ingredients_list' : [],
        'buns_list' : [],
        'all' : []
    }

    if "standard_burger" in selection:
        system.add_standard_burger_items(items_dict['all'])
    elif "standard_wrap" in selection:
        system.add_standard_wrap_items(items_dict['all'])


    checklist = (
        (system.sides, 'sides_list'),
        (system.drinks, 'drinks_list'),
        (system.ingredients, 'ingredients_list'),
        (system.buns, 'buns_list')
    )

    for item_obj_list, list_name in checklist:
        for obj in item_obj_list:
            try:
                number = clean_input(request_dict.get(obj.name))
                items_dict[list_name] += [obj]*number
                items_dict['all'] += [obj]*number
            except ValueError:
                input_errors[obj.name] = "Invalid input for {}".format(obj.name)

    return items_dict, input_errors

def check_errors(items_dict, mains, action, adding_main):
    errors = {}
    items_required = []
    for main in mains:
        items_required += main.items

    if "Add Main" == action:
        if adding_main == 'custom_burger':
            items_required += items_dict['ingredients_list'] + items_dict['buns_list']
            try:
                system.create_burger(items_dict['ingredients_list'], items_dict['buns_list'])
            except InvalidOrderError as e:
                errors['head'] = 'Invalid burger: ' +e.msg
        elif adding_main == 'custom_wrap':
            items_required += items_dict['ingredients_list']
            try:
                system.create_wrap(items_dict['ingredients_list'])
            except InvalidOrderError as e:
                errors['head'] = 'Invalid wrap: ' +e.msg
        elif adding_main == 'standard_burger':
            items_required += system.standard_burger.items
        elif adding_main == 'standard_wrap':
            items_required += system.standard_wrap.items

    else:
        items_required += items_dict['drinks_list']+items_dict['sides_list']


    try:
        GourmetBurgerSystem.check_item_list(items_required)
    except OutOfStockError as e:
        for item in e.msg:
            errors[item] = item + ' out of stock.'

    
    
    return errors

def create_order(items_dict):
    mains = SessionManager.fetch_items()
    id = system.create_new_order()
    order = system.get_order_by_id(id)
    burgers = [w for w in mains if type(w).__name__ == 'burger']
    wraps = [w for w in mains if not type(w).__name__ == 'burger']
    order.set_burgers(burgers)
    order.set_wraps(wraps)
    order.set_sides(items_dict['sides_list'])
    order.set_drinks(items_dict['drinks_list'])
    GourmetBurgerSystem.checkout(order)
    return id

def create_main(items_dict, adding_main):
    if adding_main == 'custom_burger':
        return system.create_burger(items_dict['ingredients_list'], items_dict['buns_list'])
    elif adding_main == 'custom_wrap':
        return system.create_wrap(items_dict['ingredients_list'])
    elif adding_main == 'standard_burger':
        return system.standard_burger
    else:
        return system.standard_wrap


@app.route('/select/', methods=["GET", "POST"])
def select():
    
    mains = SessionManager.fetch_items()

    if request.method == 'POST':
        adding_main = request.form['main']
        action = request.form['action']
        current_order = request.form.to_dict()

        if "remove" in action:
            index = int(action[6:])
            mains.remove(mains[index])
            return render_template('select.html', selection=adding_main, system=system,
                current_order=current_order, display=display, mains=list(enumerate(mains)))

        errors = {}
        
        items_dict, input_errors = sort_items(request)
        errors.update(input_errors)
        
        price = 0
        for item in items_dict['drinks_list']:
            price += item.price
        for item in items_dict['sides_list']:
            price += item.price
        for main in mains:
            price += main.price

        
        errors.update(check_errors(items_dict, mains, action, adding_main))

        if errors:
            return render_template('select.html', selection=adding_main, system=system, errors=errors, 
                current_order=current_order, display=display, mains=list(enumerate(mains)))
        if "Check Order" == action:
            return render_template('select.html', selection=adding_main, system=system, price=price, 
                current_order=current_order, display=display, mains=list(enumerate(mains)))
        elif "Add Main" == action:
            mains += [create_main(items_dict, adding_main)]
            return render_template('select.html', selection=adding_main, system=system, 
                current_order=current_order, display=display, mains=list(enumerate(mains)))
        elif "Confirm Order" == action:
            id = create_order(items_dict)
            return redirect(url_for('check_order',id=id))
        else:
            return redirect(url_for('order'))

    return render_template('select.html', system=system, display=display, mains=list(enumerate(mains)))

@app.route('/restock/', methods=["GET", "POST"])
def restock():
    if request.method == 'POST':

        new_stock_levels = request.form.to_dict()
        try:
            messages = system.update_stock_levels(new_stock_levels)
        except StockSetterError as e:
            stock_levels = system.stock_levels
            key_value_pairs = [(key, (value, stock_levels[key][1])) for (key, value) in list(request.form.items())]
            return render_template('restock.html', stock_key_value_pairs = key_value_pairs, errors=e.errors, messages=e.messages)
        else:
            key_value_pairs = system.stock_levels.items()
            return render_template('restock.html', stock_key_value_pairs = key_value_pairs, messages=messages)
            
    else:
        key_value_pairs = system.stock_levels.items()
        return render_template('restock.html', stock_key_value_pairs = key_value_pairs)


@app.route("/check_order/", methods=["GET", "POST"])
def get_id():
    if request.method == 'POST':
        id = request.form["id"]
        return redirect(url_for("check_order", id=id))

    return render_template('order_status.html', get_number=True)

@app.route('/check_order/<id>', methods=["POST", "GET"])
def check_order(id):
    if request.method == 'POST':
        system.get_order_by_id(request.form['bt']).status = 'collected'

    if system.id_exists(id):
        order = system.get_order_by_id(id)
    else:
        order = None
    
    return render_template('order_status.html', display = display, order=order)

@app.route('/order_list/', methods=["POST", "GET"])
def order_list():
    if request.method == 'POST':
        system.get_order_by_id(request.form['bt']).status = 'ready'

        return render_template('order_list.html', display = display, order_list = system.orders)
        
    else:

        return render_template('order_list.html', display = display, order_list = system.orders)


