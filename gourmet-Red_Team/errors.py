class OutOfStockError(Exception):
    def __init__(self, msg):
        self.msg = msg

class InvalidOrderError(Exception):
    def __init__(self, msg):
        self.msg = msg

class StockSetterError(Exception):
    def __init__(self, errors, messages):
        self.errors = errors
        self.messages = messages