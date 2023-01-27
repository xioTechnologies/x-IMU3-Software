import time


def yes_or_no(question):
    while True:
        key = input(question + " [Y/N]\n")

        if key == "y" or key == "Y":
            return True

        if key == "n" or key == "N":
            return False
