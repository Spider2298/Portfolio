from order import Order

class OrderManager:

    def __init__(self):
        self._orders = dict()
        self._up_to_id = 0
    
    def create_new_order(self):
        order = Order(str(self._up_to_id))
        self._orders[str(order.id)] = order
        self._up_to_id += 1
        return str(order.id)
    
    def id_exists(self, id):
        try:
            self._check_id_valid(id)
        except (TypeError, ValueError):
            return False
        else:
            return id in self._orders

    def get_order_by_id(self, id):
        self._check_id_valid(id)
        return self._orders[id]

    def _check_id_valid(self, id):

        if type(id) != str: 
            raise TypeError("id must be string of int")
        
        try:
            int(id)
        except ValueError:
            raise TypeError("id must be string of int")

    @property
    def orders(self):
        return [pair[1] for pair in self._orders.items()]