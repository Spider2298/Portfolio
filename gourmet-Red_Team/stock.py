from gourmet_burger_system import GourmetBurgerSystem


system = GourmetBurgerSystem()

updated_stock_levels = {}

for key in system.stock_levels:
    updated_stock_levels[key] = 100000

system.update_stock_levels(updated_stock_levels)
system.save()