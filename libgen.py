#!/usr/bin/env python3


def read_file(filename):
    """Read the input C file"""
    with open(filename, 'r') as f:
        return f.readlines()


def write_file(filename, lines):
    """Write the output to a file"""
    with open(filename, 'w') as f:
        f.writelines(lines)


def transform_line(line):
    new_line = line.rstrip('\n') + "\\\n"
    new_line = new_line.replace("Vector", "Struct_Name_")
    new_line = new_line.replace("VECTOR_DEFINE_PANIC(vector)", "VECTOR_DEFINE_PANIC(Functions_Prefix_)")
    new_line = new_line.replace("vector", "Functions_Prefix_##")
    new_line = new_line.replace("SampleType", "Custom_Type_")
    return new_line


def main():
    lines = read_file("vector.in.h")
    result = []
    in_macro = False
    for i, line in enumerate(lines):
        if "typedef int SampleType;" in line:
            continue
        if "/* Declarations start here */" in line:
            in_macro = True
            result.append("""#define VECTOR_DECLARE(Struct_Name_, Functions_Prefix_, Custom_Type_)\\
""")
            continue
        elif "/* Declarations stop here */" in line:
            in_macro = False
            continue
        elif "/* Definitions start here */" in line:
            in_macro = True
            result.append("""#define VECTOR_DEFINE(Struct_Name_, Functions_Prefix_, Custom_Type_)\\
""")
            continue
        elif "/* Definitions stop here */" in line:
            in_macro = False
            continue

        if in_macro:
            result.append(transform_line(line))
        else:
            result.append(line)

    write_file("vector.h", result)


if __name__ == "__main__":
    main()
