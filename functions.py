import numpy as np
from typing import List
from scipy.optimize import linprog
import plotly.express as px

def nash_equilibrium(A: np.ndarray) -> dict:
    m, n = A.shape

    b_ub1, b_ub2 = -np.ones(m, dtype=int), np.ones(n, dtype=int)

    gap = abs(A.min())
    A_ub1 = -np.transpose(A + gap)
    A_ub2 = A

    p = linprog(c=b_ub2, A_ub=A_ub1, b_ub=b_ub1, method="simplex")
    q = linprog(c=b_ub1, A_ub=A_ub2, b_ub=b_ub2, method="simplex")

    return {'first player strategy' : p.x / abs(p.fun),
            'second player strategy' : q.x / abs(q.fun),
            'result' : np.array([1 / p.fun - gap])}


def visualize(ans_list: dict):
    fig = px.scatter(x=np.arange(1, ans_list['first player strategy'].shape[0] + 1),
                     y=ans_list['first player strategy'],
                     title = 'Спектр оптимальных стратегий первого игрока',
                     labels={'x':'', 'y':'Оптимальные стратегии'}
                    )
    fig.show()
    fig = px.scatter(x=np.arange(1, ans_list['second player strategy'].shape[0] + 1),
                     y=ans_list['second player strategy'],
                     title = 'Спектр оптимальных стратегий второго игрока',
                     labels={'x':'', 'y':'Оптимальные стратегии'}
                    )
    fig.show()