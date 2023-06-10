def ask_question(question):
    while True:
        key = input(question + " [Y/N]\n")

        if key == "y" or key == "Y":
            return True

        if key == "n" or key == "N":
            return False
