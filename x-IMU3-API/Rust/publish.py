import os

os.system("cargo publish --no-verify --token " + open("token.txt").read())
