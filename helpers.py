import os
import re
import sys


def top_level_file():
    top_level_file_path = os.path.realpath(sys.modules["__main__"].__file__)
    this_directory = os.path.dirname(os.path.realpath(__file__))

    return os.path.relpath(top_level_file_path, this_directory).replace("\\", "/")


def preamble():
    return "// This file was generated by " + top_level_file() + "\n\n"


def camel_case(string):
    result = ""

    for word in re.sub("[^0-9a-zA-Z]", " ", string).split():
        if result == "":
            result += word.lower()
        elif len(word) == 2 and word == word.upper():  # if two letter acronym
            result += word
        else:
            result += word.lower().capitalize()

    return result


def macro_case(string):
    return re.sub("[^0-9a-zA-Z]", " ", string).upper().replace(" ", "_")


def pascal_case(string):
    result = ""

    for word in re.sub("[^0-9a-zA-Z]", " ", string).split():
        if len(word) == 2 and word == word.upper():  # if two letter acronym
            result += word
        else:
            result += word.lower().capitalize()

    return result


def snake_case(string):
    return re.sub("[^0-9a-zA-Z]", " ", string).lower().replace(" ", "_")


def title_case(string):
    result = ""

    for word in string.split():
        result += word[0].upper() + word[1:] + " "

    return result[:-1]


def insert(file_path, code, id):
    with open(file_path) as file:
        all_lines = file.readlines()

    start_key = "// Start of code block #" + str(id) + " generated by " + top_level_file() + "\n"
    end_key = "// End of code block #" + str(id) + " generated by " + top_level_file() + "\n"

    waiting_for_start = True
    waiting_for_end = True
    lines_before_start = []
    lines_after_end = []

    for line in all_lines:
        if waiting_for_start:
            lines_before_start += line

            if line.strip() == start_key.strip():
                waiting_for_start = False

            continue

        if waiting_for_end:
            if line.strip() == end_key.strip():
                lines_after_end += line
                waiting_for_end = False

            continue
        else:
            lines_after_end += line

    if waiting_for_start:
        raise Exception("Error: Cannot find \"" + start_key.strip() + "\" in " + file_path)

    if waiting_for_end:
        raise Exception("Error: Cannot find \"" + end_key.strip() + "\" in " + file_path)

    with open(file_path, "w") as file:
        for line in lines_before_start:
            file.write(line)

        file.write(code)

        for line in lines_after_end:
            file.write(line)
