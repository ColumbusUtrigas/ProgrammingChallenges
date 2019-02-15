import random
import sys

def print_help():
	print('[script name] length=[number] digits=[y/n] letters=[y/n] capital[y/n]')
	sys.exit()

def is_accept(string):
	return string == 'yes' or string == 'y' or string == 'true'

alphabet = 'abcdefghijklmnopqrstuvwxyz'
numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
letters = [c for c in alphabet]
capital_letters = [c for c in alphabet.upper()]

length = 10
allow_digits = True
allow_letters = True
allow_capital = True

print()
print('Password generator')
print()

for arg in sys.argv:
	spl = arg.lower().split('=')

	if (arg == 'help' or arg == '--help' or arg == '-h'): print_help()

	if (len(spl) == 2):
		if (spl[0] == 'length'): length = int(spl[1])
		if (spl[0] == 'digits'): allow_digits = is_accept(spl[1])
		if (spl[0] == 'letters'): allow_letters = is_accept(spl[1])
		if (spl[0] == 'capital'): allow_capital = is_accept(spl[1])

print('length =', length)
print('digits =', allow_digits)
print('letters =', allow_letters)
print('capital =', allow_capital)

choice = []
password = str()

if (allow_digits): choice.append(1)
if (allow_letters): choice.append(2)
if (allow_capital): choice.append(3)

if len(choice) != 0:
	for i in range(0, length):
		num = random.choice(choice)
		if num == 1: password += str(random.choice(numbers))
		if num == 2: password += str(random.choice(letters))
		if num == 3: password += str(random.choice(capital_letters))

print()
print(password)
print()


