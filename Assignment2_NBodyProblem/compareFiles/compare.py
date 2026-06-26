#!/usr/bin/env python3
"""
compare.py - Numerical validator for the N-Body simulation.

Compares two particle CSV files (reference vs program output) and
checks that the maximum position difference is below a threshold.

Usage:
    python3 compare.py <reference.csv> <output.csv> [--threshold 0.2]
"""

import sys
import csv
import math
import argparse


def load_csv(path):
    particles = []
    with open(path, newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            particles.append({k: float(v) for k, v in row.items()})
    return particles


def compare(ref_path, out_path, threshold=0.2):
    ref = load_csv(ref_path)
    out = load_csv(out_path)

    if len(ref) != len(out):
        print(f"ERROR: particle count mismatch: ref={len(ref)} out={len(out)}")
        return False

    max_diff = 0.0
    max_idx  = -1

    for i, (r, o) in enumerate(zip(ref, out)):
        dx = r['x'] - o['x']
        dy = r['y'] - o['y']
        dz = r['z'] - o['z']
        dist = math.sqrt(dx*dx + dy*dy + dz*dz)
        if dist > max_diff:
            max_diff = dist
            max_idx  = i

    passed = max_diff <= threshold
    status = "PASS" if passed else "FAIL"

    print(f"[compare] {status}")
    print(f"  Particles compared : {len(ref)}")
    print(f"  Max difference     : {max_diff:.6f}  (threshold={threshold})")
    print(f"  Worst particle idx : {max_idx}")

    if not passed:
        r = ref[max_idx]
        o = out[max_idx]
        print(f"  Reference : x={r['x']:.6f} y={r['y']:.6f} z={r['z']:.6f}")
        print(f"  Output    : x={o['x']:.6f} y={o['y']:.6f} z={o['z']:.6f}")

    return passed


def main():
    parser = argparse.ArgumentParser(description="N-Body numerical validator")
    parser.add_argument("reference", help="Reference CSV file")
    parser.add_argument("output",    help="Program output CSV file")
    parser.add_argument("--threshold", type=float, default=0.2,
                        help="Maximum allowed difference (default: 0.2)")
    args = parser.parse_args()

    ok = compare(args.reference, args.output, args.threshold)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
