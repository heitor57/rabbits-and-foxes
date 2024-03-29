
from mlflow.tracking.fluent import log_metric
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


if __name__ == "__main__":
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
        mlflow.set_experiment("profile")
        with mlflow.start_run():
            for k, v in param.items():
                mlflow.log_param(k, v)
            executable = "./serial-rabbits-and-foxes"
            mlflow.log_param('executable',executable)
            result=os.popen(f"{executable} < "+artifact_path).read()
            result_text = result
            # result = subprocess.run(
                # ["./serial-rabbits-and-foxes", "<", artifact_path],
                # stdout=subprocess.PIPE,
            # )
            # result_text = result.stdout.decode("utf-8")
            print(result_text.splitlines()[0])
            last_line = result_text.splitlines()[-1]
            print(last_line)

            # print(result_text)

            last_line = last_line[len("Time spend with computation: ") : -1]
            # print()
            log_metric("computation_time", float(last_line))
            # if os.path.isfile('gmoun')
            os.popen(f"gprof {executable} gmon.out > gprof_result.txt").read()
            mlflow.log_artifact('gprof_result.txt')

        # except Exception as e:
            # print(e)
