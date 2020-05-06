import re

def open_file(path):
    file = open(path, 'r')
    return file

directives = ['data', 'code', 'model', 'END']
instructions = ['std', 'jnge', 'jmp', 'push', 'pop', 'idiv', 'add', 'adc', 'in', 'or']
registers32 = ['eax', 'ebx', 'ecx', 'edx', 'ebp', 'esi', 'edi', 'esp']
registers8 = ['cl', 'ch', 'al', 'ah', 'bl', 'bh', 'dh', 'dl']
models = ['small', 'tiny']
data_types = ['db', 'dd']

splitters = ['.', ':', ' ', '[', ']', '\n', '\t', ',']

def type_detector(string_token):

    if string_token in directives:
        return token_type.directive
    elif string_token in registers32:
        return token_type.register32
    if string_token in registers8:
        return token_type.register8
    if string_token in models:
        return token_type.model
    if string_token in data_types:
        return token_type.data_type
    if string_token in instructions:
        return token_type.instructions
    if string_token in splitters:
        return token_type.splitter
    pattern = re.compile(r'[0-9]+')
    res = re.fullmatch(pattern, string_token)
    if res:
        return token_type.dec_number
    pattern = re.compile(r'[0-9]+b')
    res = re.fullmatch(pattern,string_token)
    if res:
        return token_type.bin_number
    else:
        return token_type.user_type

class token_type:
    directive = 1
    register32 = 2
    model = 3
    data_type = 4
    instructions = 5
    splitter = 6
    user_type = 7
    dec_number = 8
    bin_number = 9
    register8 = 10

TYPES = {
    token_type.directive: 'directive',
    token_type.register32: 'register32',
    token_type.register8: 'register8',
    token_type.model: 'model',
    token_type.data_type: 'directive',
    token_type.instructions: 'instruction',
    token_type.splitter: 'splitter',
    token_type.dec_number: 'dec number',
    token_type.bin_number: 'bin number',
    token_type.user_type: 'user_defined'
}

def token_print(string_token, array):
    if string_token.strip() != "":

        if TYPES[type_detector(string_token)] != 'splitter':
            print("(" + "\"" + string_token.strip() + '"' + ' : ' + TYPES[type_detector(string_token)] + ' : ' + str(len(token)) + ")", end = ' ')
        else :
            print("(" + "\"" + string_token.strip() + '"' + ' : ' + TYPES[type_detector(string_token)] + ' : ' + '1' ")", end = ' ')
        array.append(string_token)

def sentence_analysis(token_array, position, labels_array, position_array):
    coma_flag = 0
    print('|SENTENCE :|', end='')
    if len(token_array) >= 2 and token_array[1] == ':' :
            print("| 0 |")
            labels_array.append(token_array[0])
            position_array.append(position)
    else:
        print('|--|', end='')
        if token_array[0] == '.' :
            print(' 0  1 | 1 1 |', end ='')
            if len(token_array) > 2 :
                print(" 2 1 |")
        else:
            print('0 1 |',end= '')
            for i in range(0, len(token_array)):
                if token_array[i] == ',':
                    coma_flag = 1
                    coma_pos = i
            if coma_flag :
                for j in range (1, len(token_array)):
                    if j == 1:
                        print('|' + str(j) + ' ' + str(coma_pos - j) + '|', end='' )
                    elif j == coma_pos + 1 :
                        print("|" + str(j) + ' ' + str(len(token_array) - coma_pos - 1)+ '|', end='')
            else :
                for k in range (1 , len(token_array)):
                    print('|' + str(k) + ' 1' + '|' )
    print('\n')


def create_array_of_tokens(token, array):
    array.append(token)

file = open_file('testfile')
counter = 0
labels_array = []
positions_of_labels = []
segment_index = []
for line in file:
    token = ''
    counter += 1
    if line.strip() == '':
        continue
    line_tokens = []
    print("|SOURCE :  " + line[:-1] + "|")
    print("|TOKENS : [ ", end = '')
    for i in range(0, len(line)):
        if line[i] in splitters:
            token_print(token, line_tokens)
            token_print(line[i], line_tokens)
            token = ''
            continue
        else:
            token += line[i]
    print(']')
    if line_tokens[0] == '.' and line_tokens[1] == 'data':
        segment_index.append(counter)
    if line_tokens[0] == '.' and line_tokens[1] == 'code':
        segment_index.append(counter)
    sentence_analysis(line_tokens, counter, labels_array, positions_of_labels)
for i in range(0, len(labels_array)):
    print('LABEL #' + str(i) + ' ' + str(labels_array[i]) + ' at the ' + str(positions_of_labels[i]) + ':' + str(len(labels_array[i])))
print("OPENED SEGMENTS:", end = '')
print("\ndata opened at line " + str(segment_index[0]) + '\n' + 'code opened at line ' + str(segment_index[1]))

file.close()
