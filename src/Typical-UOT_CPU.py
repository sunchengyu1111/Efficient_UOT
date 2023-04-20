import os
# os.environ["MKL_NUM_THREADS"] = "1"
# os.environ["NUMEXPR_NUM_THREADS"] = "1"
# os.environ["OMP_NUM_THREADS"] = "1"
# os.environ["OPENBLAS_NUM_THREADS"] = "1"
import sys
import numpy as np
from timeit import default_timer as timer
num_row = int(str(sys.argv[1]))
num_col = int(str(sys.argv[2]))
uot = np.random.rand(num_row, num_col) * 100
row = np.random.rand(num_row) * 100
col = np.random.rand(num_col) * 100
eps = 1e-1
error = 1
count = 0
fi = 0.5
col_t0 = uot.sum(0)
start = timer()
while(error > eps):
	# print(count)
	count = count + 1
	col_t1 = col_t0.copy()
	uot *= ((col / col_t0)**fi).reshape(1,-1)
	uot *= ((row / uot.sum(1))**fi).reshape(-1,1)
	col_t0 = uot.sum(0)
	col_t1 = (col_t0 - col_t1)
	col_t1 = np.maximum(col_t1, -col_t1)
	error = col_t1.sum()
	# print(error)
finish = timer()
print((finish - start) / count * 1000)
