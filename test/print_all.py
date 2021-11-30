from sys import executable
from mlflow.tracking.fluent import log_artifact, log_metric
import numpy as np
import subprocess
from cycler import cycler
import mlflow
import os
from mlflow.tracking.client import MlflowClient
import secrets
from pathlib import Path
import random
import os
import utils
import argparse
import tabulate

def get_cmp_time(executable,env_variables,param,runs_infos):
    mlflow.set_experiment("run")
    dtmp = {str(k): str(v) for k, v in param.items()}
    dtmp['executable']=executable
    dtmp['env_variables']=env_variables
    run = utils.already_ran(
        dtmp,
        experiment_id,
        runs_infos=runs_infos,
    )
    client = MlflowClient()
    cmp_time = client.get_metric_history(run.info.run_id, 'computation_time')
    return cmp_time[-1].value

if __name__ == "__main__":
    # argparser= argparse.ArgumentParser()
    # argparser.add_argument('-e',type=str,default="./serial-rabbits-and-foxes")
    # argparser.add_argument('-d',type=str,default="")
    # args=argparser.parse_args()
    # executable = args.e
    # env_variables = args.d
    np.random.seed(utils.RAND_SEED)

    # os.popen("make release_time_debug").read()
    mlflow.set_experiment("run")
    experiment_id = mlflow.get_experiment_by_name("run").experiment_id
    runs_infos = mlflow.list_run_infos(
        experiment_id,
        order_by=["attribute.end_time DESC"]
    )
    executable='./serial-rabbits-and-foxes'
    env_variables=''
    cmp_times=[]
    for param in utils.parameters:
        cmptime = get_cmp_time(executable,env_variables,param,runs_infos)
        cmp_times.append([executable,'','',cmptime])

    executable='./parallel-rabbits-and-foxes'
    
    for j in ["static","dynamic","guided","dynamic, 400","guided, 400"]:
        for i in [4,8,12]:
            for param in utils.parameters:
                env_variables=f"OMP_NUM_THREADS='{i}' OMP_SCHEDULE='{j}'"
                print(env_variables)
                cmptime = get_cmp_time(executable,env_variables,param,runs_infos)
                cmp_times.append([executable,i,j,cmptime])

    print(tabulate.tabulate(cmp_times,tablefmt="latex"))
