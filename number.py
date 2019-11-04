from decimal import *

getcontext().prec = 100


init: Decimal = Decimal(1)
prev: Decimal = Decimal(0)

while prev != init:
	prev = init
	init = (init / (init + 1)) ** init
	count += 1

	
print (init)
