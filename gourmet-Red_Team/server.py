from flask import Flask
from gourmet_burger_system import GourmetBurgerSystem

system = GourmetBurgerSystem.load()

app = Flask(__name__)
app.secret_key = 'very-secret-123'  # Used to add entropy