class Item:

    def __init__(self, name, price, quantity_per_serving, quantity_units, stock_levels_dict, stock_levels_key=None):
        self._name = name
        self._price = price
        self._quantity_per_serving = quantity_per_serving
        self._quantity_units = quantity_units
        self._stock_levels_dict = stock_levels_dict
        if stock_levels_key:
            self._stock_levels_key = stock_levels_key
        else:
            self._stock_levels_key = name
        
        if self._stock_levels_key not in self._stock_levels_dict:
            self._stock_levels_dict[self._stock_levels_key] = [0,self._quantity_units]

    @property
    def name(self):
        return self._name
    
    @property
    def price(self):
        return self._price
    
    @property
    def quantity_per_serving(self):
        return self._quantity_per_serving

    @property
    def quantity_units(self):
        return self._quantity_units

    @property
    def stock(self):
        return self._stock_levels_dict[self._stock_levels_key][0]
    @stock.setter
    def stock(self, amount):
        if amount < 0:
            raise ValueError("Can't have negative stock.")
        self._stock_levels_dict[self._stock_levels_key][0] = amount
    def increment(self, amount):
        self._stock_levels_dict[self._stock_levels_key][0] += amount

    def __repr__(self):
        return type(self).__name__ + " <name: {}, price: ${:.2f}, quantity_per_serving: {}{}>".format(self.name,self.price, self.quantity_per_serving, self.quantity_units)



def initialise_drinks(stock_levels_dict):
    return [
        Item("Large Orange Juice", 7, 650, "mL", stock_levels_dict,"Orange Juice"),
        Item("Medium Orange Juice", 5.5, 450, "mL", stock_levels_dict,"Orange Juice"),
        Item("Small Orange Juice", 4, 250, "mL", stock_levels_dict,"Orange Juice"),

        Item("Large Apple Juice", 7, 650, "mL", stock_levels_dict,"Apple Juice"),
        Item("Medium Apple Juice", 5.5, 450, "mL", stock_levels_dict,"Apple Juice"),
        Item("Small Apple Juice", 4, 250, "mL", stock_levels_dict,"Apple Juice"),

        Item("Large Cranberry Juice", 7, 650, "mL", stock_levels_dict, "Cranberry Juice"),
        Item("Medium Cranberry Juice", 5.5, 450, "mL", stock_levels_dict, "Cranberry Juice"),
        Item("Small Cranberry Juice", 4, 250, "mL", stock_levels_dict, "Cranberry Juice"),

        Item("Large Pomegranate Juice", 7, 650, "mL", stock_levels_dict, "Pomegranate Juice"),
        Item("Medium Pomegranate Juice", 5.5, 450, "mL", stock_levels_dict, "Pomegranate Juice"),
        Item("Small Pomegranate Juice", 4, 250, "mL", stock_levels_dict, "Pomegranate Juice"),

        Item("600ml Coke", 4.5, 1, "bottles", stock_levels_dict),
        Item("350ml Coke", 3, 1, "cans", stock_levels_dict),
        Item("600ml Sprite", 4.5, 1, "bottles", stock_levels_dict),
        Item("350ml Sprite", 3, 1, "cans", stock_levels_dict),
        Item("600ml Lift", 4.5, 1, "bottles", stock_levels_dict),
        Item("350ml Lift", 3, 1, "cans", stock_levels_dict),
    ]
        

def initialise_sides(stock_levels_dict):

    return [
        Item("Large Fries", 3, 175, "grams", stock_levels_dict, "Fries"),
        Item("Medium Fries", 3, 125, "grams", stock_levels_dict, "Fries"),
        Item("Small Fries", 3, 75, "grams", stock_levels_dict, "Fries"),

        Item("20-Pack Nuggets", 5, 20, "pcs", stock_levels_dict, "Nuggets"),
        Item("10-Pack Nuggets", 3, 10, "pcs", stock_levels_dict, "Nuggets"),
        Item("6-Pack Nuggets", 2, 6, "pcs", stock_levels_dict, "Nuggets"),
        Item("3-Pack Nuggets", 1, 3, "pcs", stock_levels_dict, "Nuggets"),

        Item("Large Chocolate Sundae", 3, 1, "servings", stock_levels_dict),
        Item("Medium Chocolate Sundae", 2, 1, "servings", stock_levels_dict),
        Item("Small Chocolate Sundae", 1, 1, "servings", stock_levels_dict),

        Item("Large Strawberry Sundae", 3, 1, "servings", stock_levels_dict),
        Item("Medium Strawberry Sundae", 2, 1, "servings", stock_levels_dict),
        Item("Small Strawberry Sundae", 1, 1, "servings", stock_levels_dict),

    ]


def initialise_buns(stock_levels_dict):

    return [
        Item("Sesame bun", 0.5, 1, "pcs", stock_levels_dict),
        Item("Muffin bun", 0.5, 1, "pcs", stock_levels_dict),
    ]

def initialise_ingredients(stock_levels_dict):

    return [
        Item("Chicken Patty", 2, 1, "servings", stock_levels_dict),
        Item("Vegetarian Patty", 2, 1, "servings", stock_levels_dict),
        Item("Beef Patty", 2, 1, "servings", stock_levels_dict),

        Item("Tomato", 1, 1, "portions", stock_levels_dict),
        Item("Lettuce", 1, 1, "portions", stock_levels_dict),
        Item("Cheddar Cheese", 1, 1, "portions", stock_levels_dict),
        Item("Swiss Cheese", 1, 1, "portions", stock_levels_dict),
        Item("Tomato Sauce", 1, 1, "portions", stock_levels_dict),
    ]
