import os
import subprocess

if not os.path.exists("build"):
    os.mkdir("build")

os.chdir("build")

subprocess.run(['cmake', '..', '-G', 'Visual Studio 15 2017 Win64'])
os.startfile('lia.sln')

os.chdir("..")
