import sourcextractor.config as config
import numpy as np

a = 42


def myfunc1(x):
    return -x / 2 + np.power(x, 5)


def myfunc2(x, y):
    z = y
    #if x > y:
    #    z = x
    return 5 * x + np.log(np.exp(1 - z) / 2)


config.evaluate = dict()
config.evaluate[0] = lambda: 5 + a // 10
config.evaluate[1] = lambda x: myfunc1(x)
config.evaluate[2] = lambda x, y: myfunc2(x, y)
config.evaluate[3] = lambda x, y, z: z + y + x  # myfunc2(x, y)
