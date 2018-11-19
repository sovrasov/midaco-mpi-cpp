import argparse
import json
import csv
import os
import re
from tabulate import tabulate

def parse_file_name(file_name):
    result = re.findall(r'\d+', file_name)
    nodes = result[0]
    threads = result[1]
    if result[2] == '01':
        delay = '0.1'
    elif result[2] == '05':
        delay = '0.5'
    elif result[2] == '1':
        delay = '1'
    
    return nodes, threads, delay

def get_single_node_time(folder):
    dict_single_time = {}
    for directory in ['gklsh4d', 'gklss4d', 'gklsh4d_serg', 'gklss4d_serg']:
        for file, delay in zip(['midaco_1_1_1.json', 'midaco_1_1_05.json', 'midaco_1_1_01.json'], ['1', '0.5', '0.1']):
            data = json.load(open(os.path.join(folder, directory, file)))
            dict_single_time[(directory, delay)] = data['avg_time']
    return dict_single_time


def main(args):
    all_stats = []
    for subdir, dirs, files in sorted(os.walk(args.root_folder)):
        print('Prosessing directory ' + subdir)
        directory_stats = []
        directory_files_stats = []
        for file in files:
            _, ext = os.path.splitext(file)
            if ext == '.json':
                full_path = os.path.join(subdir, file)
                stats = {}
                with open(full_path, 'r') as f:
                    stats = json.load(f)
                    directory_stats.append(stats)
                    directory_files_stats.append(file)
        if len(directory_stats):
            all_stats.append((directory_stats, os.path.basename(subdir), directory_files_stats))

    single_node_time = get_single_node_time(args.root_folder)

    #all_tables = {}
    #columns = ['Method', 'Average number of trials', 'Problems solved']
    table_name = '../' + args.root_folder + '_table.csv'
    with open(os.path.join(args.root_folder, table_name), 'w') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['class', 'nodes', 'threads', 'delay', 'method', 'solved', 'avg trials', 'avg time', 'speedup'])
        for class_results, class_name, class_file_name in all_stats:
            #table_rows = []
            for method_results, file_name in zip(class_results, class_file_name):
                nodes, threads, delay = parse_file_name(file_name)
                #table_rows.append((method_results['capture'], method_results['calc_counters'][0], method_results['num_solved']))
                writer.writerow([class_name, nodes, threads, delay, method_results['capture'],
                    method_results['num_solved'], round(method_results['calc_counters'][0]), 
                    method_results['avg_time'],float(single_node_time[(class_name, delay)]/method_results['avg_time']),])
            writer.writerow([])
            #table = tabulate(sorted(table_rows), headers=columns, tablefmt='latex', floatfmt='.2f', numalign='center')
            #all_tables[class_name] = table


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('root_folder', type=str)
    parser.add_argument('--report_tex', type=str)
    main(parser.parse_args())
