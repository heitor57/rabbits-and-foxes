import numpy as np
from cycler import cycler
import mlflow
import secrets
from pathlib import Path
import random
import os
import utils

if __name__ == "__main__":
    np.random.seed(utils.RAND_SEED)
    mlflow.set_experiment("dataset")
    for param in utils.parameters:
        print(param)
        with mlflow.start_run():
            for k, v in param.items():
                mlflow.log_param(k, v)

            fname = "dataset.txt"
            fnametmp = os.path.join("tmp", secrets.token_urlsafe(16), fname)
            utils.create_path_to_file(fnametmp)
            with open(fnametmp, mode="w") as f:
                utils.create_dataset(f,param)
                f.flush()
            mlflow.log_artifact(fnametmp)
    # print(len(parameters))
