import time


def yes_or_no(question):
    while True:
        key = input(question + " [Y/N]\n")

        if key == "y" or key == "Y":
            return True

        if key == "n" or key == "N":
            return False


def wait(seconds):
    if seconds < 0:
        while True:
            time.sleep(0.01)

    for _ in range(seconds * 100):
        time.sleep(0.01)
