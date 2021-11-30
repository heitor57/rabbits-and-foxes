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
    # cycler(L=2 ** (np.arange(3, dtype=int) + 7))
    # * cycler(C=2 ** (np.arange(3, dtype=int) + 7))
    # * cycler(N=2 ** (np.arange(3, dtype=int) + 6))
    # cycler(L=[100,250,500])
    # * cycler(C=[100,250,500])
    # * cycler(N=[250,500,1000])
    # * cycler(GEN_PROC_COELHOS=[2,4])
    # * cycler(GEN_PROC_RAPOSAS=[4,6])
    # * cycler(GEN_COMIDA_RAPOSAS=[3,5])
    # * cycler(GEN_PROC_COELHOS=[2])
    # * cycler(GEN_PROC_RAPOSAS=[4])
    # * cycler(GEN_COMIDA_RAPOSAS=[3])
    # * cycler(N_GEN=[10,20])
    # cycler(L=2 ** (np.arange(1, dtype=int) + 7))
    # * cycler(C=2 ** (np.arange(1, dtype=int) +7))
    # * cycler(N=2 ** (np.arange(1, dtype=int) + 9))
    cycler(L=[10000])
    * cycler(C=[10000])
    # * cycler(N=[100])
    * cycler(N=[100000])
    * cycler(GEN_PROC_COELHOS=[2])
    * cycler(GEN_PROC_RAPOSAS=[4])
    * cycler(GEN_COMIDA_RAPOSAS=[3])
    * cycler(N_GEN=[10])
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



def already_ran(parameters, experiment_id, runs_infos):
    """Best-effort detection of if a run with the given entrypoint name,
    parameters, and experiment id already ran. The run must have completed
    successfully and have at least the parameters provided.
    """
    # parameters={str(k):str(v) for k,v in parameters.items()}

    def _get_params(run):
        """Converts [mlflow.entities.Param] to a dictionary of {k: v}."""
        return run.data.params
    # print('Exp',experiment_id)
    all_run_infos = runs_infos
    for run_info in all_run_infos:
        # print(run_info,parameters)

        full_run = mlflow.get_run(run_info.run_uuid)
        run_params = _get_params(full_run)
        match_failed = False
        # print(parameters,"---",run_params)
        for param_key, param_value in parameters.items():
            run_value = run_params.get(param_key)
            if run_value != param_value:
                match_failed = True
                break
        if match_failed:
            continue

        if run_info.status != "FINISHED":
            print(
                (
                    "Run matched, but is not FINISHED, so skipping "
                    "(run_id=%s, status=%s)"
                )
                % (run_info.run_uuid, run_info.status)
            )
            continue
        return mlflow.get_run(run_info.run_uuid)
    # raise IndexError("Could not find the run with the given parameters.")
    return None
