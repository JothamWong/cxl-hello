#!/usr/bin/env python3

import sys
import re
import argparse

def parse_address(addr_str):
    try:
        return int(addr_str, 16)
    except ValueError:
        print(f"Error: Invalid address format '{addr_str}'. Please use hex (e.g., 0x1000).")
        sys.exit(1)

def ranges_overlap(start1, end1, start2, end2):
    return max(start1, start2) < min(end1, end2)

def main():
    parser = argparse.ArgumentParser(
        description="Filter kernel PAT memtype list by an overlapping address range."
    )
    parser.add_argument("start_addr", help="Target start address (hex, e.g., 0x100000)")
    parser.add_argument("end_addr", help="Target end address (hex, e.g., 0x200000)")
    parser.add_argument("--pat-file", default="/sys/kernel/debug/x86/pat_memtype_list",
                        help="Path to PAT list (default: /sys/kernel/debug/x86/pat_memtype_list)")

    args = parser.parse_args()

    target_start = parse_address(args.start_addr)
    target_end = parse_address(args.end_addr)

    if target_start >= target_end:
        print("Error: Target start address must be less than the end address.")
        sys.exit(1)

    # Regex to extract start and end addresses from a line like:
    # write-back @ 0x0000000000100000-0x0000000004000000
    line_regex = re.compile(r"@\s*(0x[0-9a-fA-F]+)-(0x[0-9a-fA-F]+)")

    try:
        with open(args.pat_file, "r") as f:
            for line in f:
                match = line_regex.search(line)
                if match:
                    line_start = parse_address(match.group(1))
                    line_end = parse_address(match.group(2))
                    if ranges_overlap(target_start, target_end, line_start, line_end):
                        print(line.strip())

    except PermissionError:
        print(f"Error: Permission denied. You must run this script as root (sudo) to read {args.pat_file}.")
    except FileNotFoundError:
        print(f"Error: {args.pat_file} not found.")
        print("Make sure debugfs is mounted and you are on an x86 Linux system.")

if __name__ == "__main__":
    main()