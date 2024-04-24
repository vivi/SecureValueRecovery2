import csv
import numpy as np
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("folder", type=str, help="Folder containing the csv files")
parser.add_argument("run", type=str, help="Which run to process (leader/follower)")
args = parser.parse_args()


def parse_csv(file_path):
    with open(file_path, "r") as csv_file:
        csv_reader = csv.reader(csv_file)
        rows = list(csv_reader)
        measurements = {}
        for row in rows[:10]:
            measurements[row[0]] = float(row[1])
        measurements["create"] = []
        measurements["restore"] = []
        i = 11
        for row in rows[11:]:
            try:
                i += 1
                measurements["create"].append(float(row[0]) / 1e3)
                measurements["restore"].append(float(row[1]) / 1e3)
            except:
                print(i, row)
                continue
    return measurements


def get_cdf(values):
    # Sort the list
    values = np.sort(values)
    # Sample so that you end up with 10k points
    # values = values[::int(len(values)/10000)]

    # Get normalized cumulative probabilities
    norm_cdf = np.arange(len(values)) / (len(values) - 1)

    # Create CDF
    cdf = np.vstack([values, norm_cdf]).T
    return cdf


FOLDER = args.folder
RUN = args.run
OUT_FOLDER = f"{FOLDER}/{RUN}"
print(OUT_FOLDER)
os.makedirs(OUT_FOLDER, exist_ok=True)
for i in [5, 10, 20, 50, 100]:
    measurements = {
        "create": [],
        "restore": [],
    }
    FILES = [RUN]
    for j in FILES:
        file_path = f"{FOLDER}/p{i}{j}10M.csv"
        m = parse_csv(file_path)
        measurements["create"].extend(m["create"])
        measurements["restore"].extend(m["restore"])
    print(i, "AVERAGE CREATE LATENCY", np.mean(measurements["create"]))
    print(i, "AVERAGE RESTORE LATENCY", np.mean(measurements["restore"]))
    # continue

    cdf = get_cdf(measurements["create"])
    out_file = f"{OUT_FOLDER}/cdf_p{i}_create.csv"
    np.savetxt(out_file, cdf, delimiter=",")

    cdf = get_cdf(measurements["restore"])
    out_file = f"{OUT_FOLDER}/cdf_p{i}_restore.csv"
    np.savetxt(out_file, cdf, delimiter=",")
