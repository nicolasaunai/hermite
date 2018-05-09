import equation
import sympy as sym

# Configuration dicionaries
misc, eq, num = {}, {}, {}

# Variables and function
x, y, f = equation.x, equation.y, equation.f

# Short-hand notation
r = sym.Rational

# Configuration of numerical method
num['degree'] = 50  # degree of approximation
num['n_points_num'] = 2*num['degree'] + 1  # (*2 for varf)
num['μx'] = r(1, 5)
num['σx'] = r(1, 10)
num['λ'] = r(1, 2)

# Scalar parameters of the equation
eq['βx'] = r(2)
eq['βy'] = r(1)
eq['ε'] = r(.5)
eq['γ'] = r(0)
eq['θ'] = r(.5)

# Functional parameters of the equation
eq['Vp'] = x**4/4 - x**2/2

# Miscellaneous parameters
misc['cache'] = True
misc['parallel'] = False
misc['tensorize'] = False
misc['trails'] = False
misc['verbose'] = False
misc['symbolic'] = 2  # Values 0, 1, 2