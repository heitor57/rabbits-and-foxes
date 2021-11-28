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


if __name__ == "__main__":
    np.random.seed(utils.RAND_SEED)

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
            result=os.popen("./serial-rabbits-and-foxes < "+artifact_path).read()
            result_text = result
            # print(result_text)
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
            
            utils.create_path_to_file(f_result_name)
            with open(f_result_name, mode="w") as f:
                f.write(result_text)
                f.flush()
            mlflow.log_artifact(f_result_name)
        # except Exception as e:
            # print(e)
