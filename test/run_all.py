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

if __name__ == "__main__":
    argparser= argparse.ArgumentParser()
    argparser.add_argument('-e',type=str,default="./serial-rabbits-and-foxes")
    argparser.add_argument('-d',type=str,default="")
    args=argparser.parse_args()
    executable = args.e
    env_variables = args.d
    np.random.seed(utils.RAND_SEED)

    os.popen("make release_time_debug").read()
    mlflow.set_experiment("dataset")
    experiment_id = mlflow.get_experiment_by_name("dataset").experiment_id
    runs_infos = mlflow.list_run_infos(
        experiment_id,
        order_by=["attribute.end_time DESC"],
    )
    for param in utils.parameters:
        print(param)
        # try:
            # mlflow.set_experiment('dataset')
            # print(param)
            # print({k: str(v) for k, v in param.items()})
        run = utils.already_ran(
            {str(k): str(v) for k, v in param.items()},
            experiment_id,
            runs_infos=runs_infos,
        )
        client = MlflowClient()
        artifact_path = client.download_artifacts(run.info.run_id, "dataset.txt")
        # print(artifact_path)
        # metric_values = pickle.load(open(artifact_path, "rb"))
        mlflow.set_experiment("run")
        with mlflow.start_run():
            for k, v in param.items():
                mlflow.log_param(k, v)

            f_result_name = 'tmp/result.txt'


            f_resource_name=  'tmp/resource.txt'
            utils.create_path_to_file(f_resource_name)

            mlflow.log_param('executable',executable)
            mlflow.log_param('env_variables',env_variables)
            # if env_variables!="":
            result_text=os.popen(f"{env_variables} /usr/bin/time -v {executable} < "+artifact_path+ f" 2> {f_resource_name}").read()
            # else:
                # result_text=os.popen(f"/usr/bin/time -v {executable} < "+artifact_path+ f" 2> {f_resource_name}").read()
            print(result_text)
            
            # print(result_text)
            # result = subprocess.run(
                # ["./serial-rabbits-and-foxes", "<", artifact_path],
                # stdout=subprocess.PIPE,
            # )
            # result_text = result.stdout.decode("utf-8")
            resource_usage= '\n'.join(result_text.splitlines()[-24:])
            last_line = result_text.splitlines()[-1]

            # print(result_text)
            computation_time_str =[last_line[len("Time spend with computation: ") : -1] for line in result_text.splitlines() if line.startswith('Time spend with computation: ')][0]

            # print()
            log_metric("computation_time", float(computation_time_str))
            
            utils.create_path_to_file(f_result_name)
            with open(f_result_name, mode="w") as f:
                f.write(result_text)
                f.flush()
            mlflow.log_artifact(f_result_name)

            # with open(f_resource_name, mode="w") as f:
                # f.write(resource_usage)
                # f.flush()
            mlflow.log_artifact(f_resource_name)
        # except Exception as e:
            # print(e)
