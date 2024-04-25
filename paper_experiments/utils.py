import csv
import numpy as np


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
