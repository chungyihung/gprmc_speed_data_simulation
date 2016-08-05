# Ref: http://docs.scipy.org/doc/numpy/reference/generated/numpy.polyfit.html
import matplotlib.pyplot as plt
import warnings
import numpy as np

x = np.array([0.0, 20, 100, 250,300,350,450, 500])
y = np.array([0.0,  10 ,30, 60, 50, 40, 50, 0])
z = np.polyfit(x,y,6)

print(z)

p = np.poly1d(z)

xp = np.linspace(-1, 500, 500)

temp = plt.plot(x, y, '.', xp, p(xp), '-')
plt.show()
