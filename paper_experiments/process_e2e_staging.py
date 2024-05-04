import csv
import numpy as np
from utils import get_cdf
from collections import defaultdict

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("folder", type=str, help="Folder containing the csv files")
args = parser.parse_args()


def parse_csv(file_path):
    with open(file_path, "r") as csv_file:
        csv_reader = csv.reader(csv_file)
        rows = list(csv_reader)
        measurements = defaultdict(list)
        # measurements["prepare OPRFs"] = []
        # measurements["call servers"] = []
        # measurements["finalize OPRFs"] = []
        # measurements["create shares"] = []
        # measurements["total"] = []
        i = 1
        for row in rows[1:]:
            try:
                i += 1
                total = (float(row[4]) + float(row[5])) / 1e3
                measurements["prepare oprfs"].append(float(row[0]) / 1e3)
                measurements["prepare oprfs %"].append(float(row[0]) / 1e3 / total)
                measurements["call servers"].append(float(row[1]) / 1e3)
                measurements["call servers %"].append(float(row[1]) / 1e3 / total)
                measurements["finalize oprfs"].append(float(row[2]) / 1e3)
                measurements["finalize oprfs %"].append(float(row[2]) / 1e3 / total)
                measurements["total attestation"].append(float(row[4]) / 1e3)
                measurements["total attestation %"].append(float(row[4]) / 1e3 / total)
                measurements["compute backup"].append(float(row[3]) / 1e3)
                measurements["compute backup %"].append(float(row[3]) / 1e3 / total)
                measurements["total"].append(total)
            except:
                print(i, row)
                continue
    return measurements


FOLDER = args.folder
OUT_FOLDER = f"{FOLDER}"

file_path = f"{FOLDER}/e2e_latency_breakdown.csv"
measurements = parse_csv(file_path)

print("AVERAGE TOTAL LATENCY", np.mean(measurements["total"]))
print("AVERAGE PREP OPRF %", np.mean(measurements["prepare oprfs %"]))
print("AVERAGE CALL SERVER %", np.mean(measurements["call servers %"]))
print("AVERAGE FINALIZE OPRF %", np.mean(measurements["finalize oprfs %"]))
print("AVERAGE CREATE SHARES %", np.mean(measurements["compute backup %"]))
print("AVERAGE ATTESTATION TIME%", np.mean(measurements["total attestation %"]))
cdf = get_cdf(measurements["total"])
out_file = f"{OUT_FOLDER}/cdf_total.csv"
np.savetxt(out_file, cdf, delimiter=",")

out_file = f"{OUT_FOLDER}/percent_breakdown.csv"
with open(out_file, "w") as f:
    f.write("i,attestation,prepare OPRFs,call servers,finalize OPRFs,create shares,total\n")
    for i, total in enumerate(measurements["total"]):
        a = measurements["total attestation"][i] / total * 100
        b = measurements["prepare oprfs"][i] / total * 100 + a
        c = measurements["call servers"][i] / total * 100 + b
        d = measurements["finalize oprfs"][i] / total * 100 + c
        e = measurements["compute backup"][i] / total * 100 + d
        tot = total / total * 100
        f.write(f"{i},{a},{b},{c},{d},{e},{tot}\n")
