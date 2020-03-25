import fileinput
import argparse
try:
	from prettytable import PrettyTable
except:
	print('pro lepsi zobrazeni -> sudo pip install prettytable | sudo pip3 install prettytable')	
import copy

parser = argparse.ArgumentParser(description='generator DU do IZU 3. cvičení pro sudoku o velikosti 4', usage='python3 %(prog)s [options]')
parser.add_argument('--src',metavar='file',help='vstupni soubor pro zadani pevnych hodnot formát: XY<hodnota>\\n (tj 113 znamená x=1 y=1 s hodnotou 3) pokud neni zadano tak čte z STDIN indexace od 1')
parser.add_argument('--blok', action='store_true',help='pro zapnutí detekce bloku 2x2')
parser.add_argument('--iter',metavar='iterace',help='pocet iteraci default: 1')
parser.add_argument('--uhlir_index', action='store_true',help='styl abecedniho serazeni defaultne: sustek')
parser.add_argument('--all', action='store_true',help='vypise vsechny iterace froward trackingu')
args = parser.parse_args()

global BOARD_SIZE
BOARD_SIZE = 4

class cell:
	def __init__(self, x, y):
		self.x = x
		self.y = y
		self.number = '0'
		self.possible_numbers = []
		self.square_id = ' '
		self.alpha_id = None
		self.step = []
	
	def __str__(self):
		return self.number
		
	def search_back(self,array,blok):
		if not self.number in ['1','2','3','4']:
			ar = set()
			if blok:
				for lists in array:
					for cells in lists:
						if cells.square_id == self.square_id:
							ar.add(cells.number)
			for i in range(1,BOARD_SIZE):
				if 0 <= self.y+i < BOARD_SIZE:
					ar.add(array[self.x][self.y+i].number)
				if 0 <= self.y-i < BOARD_SIZE:
					ar.add(array[self.x][self.y-i].number)
				if 0 <= self.x+i < BOARD_SIZE:
					ar.add(array[self.x+i][self.y].number)
				if 0 <= self.x-i < BOARD_SIZE:
					ar.add(array[self.x-i][self.y].number)
			ar = list(ar)
			self.possible_numbers = []
			for i in range(1,BOARD_SIZE+1):
				if str(i) not in ar:
					self.possible_numbers.append(i)
			if len(self.possible_numbers) == 0:
				self.number = None
			self.step.append(self.possible_numbers)
			
		elif self.alpha_id:
			if self.step[-1][0] == '>':
				s = list(self.step[-1])
				s[0] = '-'
				self.step[-1] = "".join(s)
				return
			self.step.append(' ')
	
	def check_validity(self,array,blok):
		ar = []
		if blok:
			for lists in array:
				for cell in lists:
					if cell.square_id == self.square_id and cell.x != self.x and cell.y != self.y:
						self.validate(cell)		
		for i in range(1,BOARD_SIZE):
			if 0 <= self.y+i < BOARD_SIZE:
				self.validate(field[self.x][self.y+i])
			if 0 <= self.y-i < BOARD_SIZE:
				self.validate(field[self.x][self.y-i])
			if 0 <= self.x+i < BOARD_SIZE:
				self.validate(field[self.x+i][self.y])
			if 0 <= self.x-i < BOARD_SIZE:
				self.validate(field[self.x-i][self.y])
	def validate(self,other):
		#print(self.number,other.number,'  ',end='')
		if self.number == other.number:
			self.number = None
	
	def min_conflict(self,array,blok):
		if self.alpha_id:
			init_val = self.number
			ar = [0,0,0,0]
			if blok:
				for lists in array:
					for cell in lists:
						if cell.square_id == self.square_id and cell.x != self.x and cell.y != self.y:
							ar[cell.number-1] += 1
			for i in range(1,BOARD_SIZE):
				if 0 <= self.y+i < BOARD_SIZE:
					ar[array[self.x][self.y+i].number-1] += 1
				if 0 <= self.y-i < BOARD_SIZE:
					ar[array[self.x][self.y-i].number-1] += 1
				if 0 <= self.x+i < BOARD_SIZE:
					ar[array[self.x+i][self.y].number-1] += 1
				if 0 <= self.x-i < BOARD_SIZE:
					ar[array[self.x-i][self.y].number-1] += 1
			min_indexs = self.conflict(ar)
			if len(min_indexs) == 1:
				self.number = min_indexs[0]+1
			else:
				for i in range(self.number,self.number+4):
					if i%4 in min_indexs:
						change = i%4
						break
				self.number = change +1
			self.step.append([ar,self.number])
			if init_val != self.number:
				return True
			
	def conflict(self,array):
		min_index = []
		min_value = 8
		for i in range(len(array)):
			if array[i] < min_value:
				min_value = array[i]
				min_index = []
				min_index.append(i)
			elif array[i] == min_value:
				min_index.append(i)
		return min_index
	
class backup_field(object):
	def __init__(self, array,iteration,x,y,possibility):
		self.array = copy.deepcopy(array)
		self.iteration = iteration
		self.x = x
		self.y = y
		self.possibility = possibility
						

def print_array(array):
	for line in array:
		for cell in line:
			print(cell.number,'',end='')
		print()

def print_step(field):
	print('------------------------------')
	for row in field:
		for cell in row:
			if cell.alpha_id:
				print(cell.alpha_id,cell.number,'\t',cell.possible_numbers)
	print('------------------------------')
	
def light_copy(field,array):
	for x in range(BOARD_SIZE):
		for y in range(BOARD_SIZE):
			field[x][y].number = array[x][y].number
			field[x][y].possible_numbers = array[x][y].possible_numbers
					
field = []
for i in range(BOARD_SIZE):
	line = []
	for j in range(BOARD_SIZE):
		line.append(cell(i,j))
	field.append(line)
					
try:
	f = open(args.src,"r")
except:
	print("cteni z STDIN: formát: XY<hodnota>\\n tj. 113 znamená x=1 y=1 s hodnotou 3 x a y v rozmezí 1-4")
	f = fileinput.input()

for line in f:
	x = int(line[0])-1
	y = int(line[1])-1
	value = line[2]
	field[x][y].number = value

for i in range(BOARD_SIZE):
	for j in range(BOARD_SIZE):
		field[i][j].square_id = int(((i//2)*(BOARD_SIZE/2))+(j//2))

conflict_field = copy.deepcopy(field)

output = []
abc = 'A'
for line in field:
	for column in line:
		if column.number == '0':
			column.number = abc
			column.alpha_id = abc
			output.append(column)
			if args.uhlir_index != True:
				abc = chr(ord(abc)+1)
		if args.uhlir_index == True:
			abc = chr(ord(abc)+1)
print('input:')
for line in field:
	for cell in line:
		print(cell.number,' ',end='')
	print()

if args.iter:
	count = int(args.iter)
else:
	count = 1

#forward checking...
backups = []
out_head = []
out_head.append('')
i = 0
while i != count:
	for row in field:
		for cell in row:
			cell.search_back(field,args.blok)
	changed = 0
	minimal = None
	for row in field:
		for cell in row:
			if len(cell.possible_numbers) == 1:
				cell.number = str(cell.possible_numbers[0])
				cell.check_validity(field,args.blok)
				cell.possible_numbers = []
				changed += 1
			elif len(cell.possible_numbers) > 1 and minimal == None:
				minimal = cell

	if changed == 0 and minimal != None:
		for row in field:
			for cell in row:
				if len(cell.possible_numbers) < len(minimal.possible_numbers) and len(cell.possible_numbers) > 1:
					minimal = cell
		backups.append(backup_field(field,i,minimal.x,minimal.y,1))
		minimal.number = str(minimal.possible_numbers[0])
		minimal.step.append('>> '+str(minimal.possible_numbers[0]))
		minimal.possible_numbers = []
	if 'iter '+str(i+1) in out_head:
		out_head.append('iter '+str(i+1)+'_2')
	else:
		out_head.append('iter '+str(i+1))
		
	i += 1
	br = False
	for row in field:
		for cell in row:
			if cell.number == None and br == False:
				backup = backups[-1]
				backups.pop()
				light_copy(field,backup.array)
				field[backup.x][backup.y].number = str(field[backup.x][backup.y].possible_numbers[backup.possibility])
				field[backup.x][backup.y].step.append('>>'+str(field[backup.x][backup.y].possible_numbers[backup.possibility]))
				field[backup.x][backup.y].possible_numbers = []
				i = backup.iteration+1
				backups.append(backup_field(field,i,backup.x,backup.y,backup.possibility+1))
				br = True
	if args.all == True:
		print('\niter',i)
		print_array(field)
				
print("Forward checking (most-constrained-variable heuristic)")


try:
	t = PrettyTable(out_head)
	for cell in output:
		out = []
		out.append(cell.alpha_id)
		for step in cell.step:
			out.append(step)
		if len(out) != len(out_head):
			for i in range(len(out_head)-len(out)):
				out.append('')
		t.add_row(out)
	print(t)
except Exception as e:
	#print(e)
	for i in range(count):
		print('\titer',i+1,end='')
	print()
	for cell in output:
		print(cell.alpha_id,'  ',end='')
		for step in cell.step:
			print(step,'\t',end='')
		print()	
	print()

for line in field:
	for cell in line:
		print(cell.number,'',end='')
	print()
print()

# min-conflict
output = []
abc = 'A'
j = range(1,5)
i = 0
for line in conflict_field:
	for column in line:
		if column.number == '0':	
			column.number = j[i%4]
			i += 1
			column.alpha_id = abc
			if args.uhlir_index != True:
				abc = chr(ord(abc)+1)
			output.append(column)
		else:
			column.number = int(column.number)
		column.step.append(column.number)
		if args.uhlir_index == True:
			abc = chr(ord(abc)+1)
			
change = ['changed:',0]
for i in range(count-1):
	changed = 0
	for line in conflict_field:
		for cell in line:
			if cell.min_conflict(conflict_field,args.blok) == True:
				changed += 1
	change.append(changed)
			
			

print("min-conflict")
out_head = ['']
for i in range(count):
	out_head.append('iter '+str(i+1))
try:
	t = PrettyTable(out_head)
	for cell in output:
		out = []
		out.append(cell.alpha_id)
		for step in cell.step:
			if step in [1,2,3,4,' ']:
				out.append(step)
			else:
				out.append(str(step[0])+' | '+str(step[1]))
		if len(out) != len(out_head):
			for i in range(len(out_head)-len(out)):
				out.append('')
		t.add_row(out)
	t.add_row(change)
	print(t)
except:
	for i in range(count):
		print('\t iter',i+1,'\t',end='')
	print()
	for cell in output:
		print(cell.alpha_id,'  ',end='')
		for step in cell.step:
			if step in [1,2,3,4,' ' ]:
				print('\t\t',step,'\t  ',end='')
			else:
				print(str(step[0])+' | '+str(step[1]),'   ',end='')
		print()	
	for c in change:
		print(c,'\t\t   ',end='')
	print()
	
	

for line in conflict_field:
	for cell in line:
		print(cell.number,'',end='')
	print()
print()
