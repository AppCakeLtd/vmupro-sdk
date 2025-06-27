import os

for file in os.listdir('.'):
    if file.endswith('.map'):
        name = os.path.splitext(file)[0]
        headerName = name + '.h'
        with open(file, 'rb') as inFile:
            data = inFile.read()
        with open(headerName, 'w') as outFile:
            outFile.write(f'unsigned char {name}_data[] = {{\n')
            for i in range(len(data)):
                if i % 12 == 0:
                    outFile.write('  ')
                outFile.write(f'0x{data[i]:02X}')
                if i != len(data) - 1:
                    outFile.write(', ')
                if i % 12 == 11 or i == len(data) - 1:
                    outFile.write('\n')
            outFile.write(f'}};\nunsigned int {name}_size = {len(data)};\n')
