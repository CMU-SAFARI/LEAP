import sys

skip_fails_in = open(sys.argv[1]) 
nw_pass_in = open(sys.argv[2]) 

skip_fails_array = [] 

for element in skip_fails_in: 
    if "fail read_num" in element: 
        skip_fails_array.append(int(element.split(":")[1])) 

row_num = -1 
cur_read_num = 0 
seq1 = ""
seq2 = "" 
for element in nw_pass_in: 
    row_num = row_num + 1 
    if (row_num % 3 == 0): 
        cur_read_num = int(element.split(":")[1])
    elif (row_num % 3 == 1): 
        seq1 = element 
    elif (row_num % 3 == 2): 
        seq2 = element 
        if (cur_read_num in skip_fails_array): 
            sys.stdout.write(seq1)
            sys.stdout.write(seq2)

