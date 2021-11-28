import numpy as np
from cycler import cycler
import mlflow
import secrets
from pathlib import Path
import random
import os
# • GEN PROC COELHOS - número de gerações até que um coelho possa
# procriar
# • GEN PROC RAPOSAS - número de gerações até que uma raposa
# possa procriar
# • GEN COMIDA RAPOSAS - número de gerações para uma raposa
# morrer de fome
# • N GEN - número de gerações para a simulação
# • L - número de linhas da matriz representando o ecossistema
# • C - número de colunas da matriz representando o ecossistema
# • N - número de objetos no ecossistema inicial
# GEN PROC COELHOS
# GEN PROC RAPOSAS
# GEN COMIDA RAPOSAS
# N GEN
# L
# C
# N
RAND_SEED = 0

def create_path_to_file(file_name):
    Path("/".join(file_name.split("/")[:-1])).mkdir(parents=True, exist_ok=True)


parameters = (
    cycler(L=2 ** (np.arange(3, dtype=int) + 7))
    * cycler(C=2 ** (np.arange(3, dtype=int) + 7))
    * cycler(N=2 ** (np.arange(3, dtype=int) + 6))
    * cycler(GEN_PROC_COELHOS=[2, 4, 6])
    * cycler(GEN_PROC_RAPOSAS=[4, 5, 6])
    * cycler(GEN_COMIDA_RAPOSAS=[3, 4, 5])
    * cycler(N_GEN=[6, 36, 100, 226])
)

def create_dataset(f,param):
    first_line = "{} {} {} {} {} {} {}\n".format(
        param["GEN_PROC_COELHOS"],
        param["GEN_PROC_RAPOSAS"],
        param["GEN_COMIDA_RAPOSAS"],
        param["N_GEN"],
        param["L"],
        param["C"],
        param["N"],
    )
    f.write(first_line)
    i = 0
    matrix = np.zeros((param["L"], param["C"]), dtype=bool)
    while i < param["N"]:
        line = np.random.randint(param["L"], size=1)[0]
        column = np.random.randint(param["C"], size=1)[0]
        if not matrix[line, column]:
            matrix[line, column] = 1
            choice = np.random.choice(
                ["ROCHA", "RAPOSA", "COELHO"], 1, p=[0.2, 0.4, 0.4]
            )[0]
            f.write("{} {} {}\n".format(choice, line, column))
            i += 1

