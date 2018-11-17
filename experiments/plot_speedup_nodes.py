import argparse
import json
import os

import matplotlib.pyplot as plt

def get_single_node_time(file_folder, threads, delay):
    file_name = 'midaco_1' + str(threads) + str(delay) + '.json'
    data = json.load(open(os.path.join(file_folder, file_name)))
    return data['avg_time']

def get_multi_node_time(file_folder, threads, delay = ''):
    avg_time = []
    for i in [2, 4, 8, 12]:
        file_name = 'midaco_' + str(i) + str(threads) + str(delay) + '.json'
        data = json.load(open(os.path.join(file_folder, file_name)))
        avg_time.append(data['avg_time'])
    return avg_time

def add_line(single_thread_time, multi_threaded_time, thread, delay):
    x = [2, 4, 8, 12]
    y= [single_thread_time/multi_threaded_time[i - 1] for i in range(1, 5)]
    lable = ''
    if delay == '_01':
        lable = 'delay = 0.1'
    elif delay == '_05':
        lable = 'delay = 0.5'
    elif delay == '_1':
        lable = 'delay = 1'

    if thread == '_1':
        lable += ' threads = 1'
    elif thread == '_16':
        lable += ' threads = 16'

    plt.plot(x, y, label = lable)


def plot_speedup_nodes(stats_folder, nodes, threads, delays, show=True, filename=None,):
    for delay in delays:
        for thread in threads:
            single_node_time = get_single_node_time(stats_folder, thread, delay)
            multi_node_time = get_multi_node_time(stats_folder, thread, delay)
            add_line(single_node_time, multi_node_time, thread, delay)

    plt.legend ()
    plt.xlabel(r'$Nodes$') 
    plt.ylabel(r'$Speedup$') 
    plt.title(r'$midaco$')
    plt.grid(True)

    if filename:
        plt.savefig(filename, dpi=300)

    if show:
        plt.show()


def main(args):
    nodes = ['_1', '_2', '_4', '_8', '_12']
    threads = ['_1', '_16']
    delays = ['_01', '_05', '_1']
    filename = 'speedup_nodes.pdf'
    try:
        plot_speedup_nodes(args.stats_folder, nodes, threads, delays,
                    show = args.show, filename = os.path.join(args.stats_folder, filename))
    except BaseException:
        print('Something is wrong. Most likely there are no necessary files.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('stats_folder', type=str)
    parser.add_argument('--show', action='store_true')
    parser.add_argument('--output', type=str, default='')

    main(parser.parse_args())
