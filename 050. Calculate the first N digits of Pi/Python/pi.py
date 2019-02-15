import sys
from decimal import *

digits = input('Enter the number of digits: ')
n = int(0)
pi = Decimal(0)

# if user inputs not a string of numbers, conversion to integer will cause exception
try:
	n = int(digits)
except Exception as e:
	print('Error: enter digits')
	sys.exit()

choose = input('Print pi to the screen (s) or to the file pi.txt (f): ')
if choose != 's' and choose != 'f':
	print('Error: enter print mode')
	sys.exit()

getcontext().prec = n
counter = int(1)

# Gregory-Leibniz series
# π = (4/1) - (4/3) + (4/5) - (4/7) + (4/9) - (4/11) + (4/13) - (4/15) ...
four = Decimal(4)
for i in range(0, n):
	pi += four / Decimal(counter) - four / Decimal(counter + 2)
	counter += 4

if choose == 's':
	print("Calculated pi:", pi)

if choose == 'f':
	with open('pi.txt', 'wt') as file:
	    file.write(str(pi))


