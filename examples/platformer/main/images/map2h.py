import os

for file in os.listdir("."):
    if file.endswith(".map"):
        name = os.path.splitext(file)[0]
        headerName = name + ".h"
        with open(file, "rb") as inFile:
            data = inFile.read()
        with open(headerName, "w") as outFile:
            outFile.write(f"const uint8_t {name}_data[] = {{\n")

            outFile.write("// X and Y size, uint32_t\n")
            # 2x 32 bit header vals
            outFile.write("  ")
            for i in range(8):
                str = f"0x{data[i]:02X}, "
                outFile.write(str)
            outFile.write("\n")

            # then the rest of the bytes
            outFile.write("// The tile IDs as uint8_t\n")
            counter = 0
            for i in range(8, len(data)):
                # just want the little endian bit
                if i % 4 != 0: 
                    continue
                if counter % 16 == 0:
                    outFile.write("  ")
                outFile.write(f"0x{data[i]:02X}")
                if i != len(data) - 1:
                    outFile.write(", ")
                if counter % 16 == 15 or i == len(data) - 1:
                    outFile.write("\n")

                counter += 1
            outFile.write(f"}};\nconst uint32_t {name}_size = {counter};\n")

            print("Wrote {} to {}".format(file, headerName))
