import tkinter as tk
from tkinter import filedialog
root = tk.Tk()
root.withdraw()

file_path = filedialog.askopenfilename(initialdir = "/home/mike/proyectos/stormbreaker/sw_team")
print (file_path)
