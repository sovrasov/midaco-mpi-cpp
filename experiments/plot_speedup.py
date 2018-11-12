import argparse
import json
import os

import matplotlib.pyplot as plt

def get_single_thread_time(file_folder, delay = ''):
    file_name = 'midaco' + str(delay) + '.json'
    data = json.load(open(os.path.join(file_folder, file_name)))
    return data['avg_time']

def get_multi_threaded_time(file_folder, delay = ''):
    avg_time = []
    for i in range(1, 5):
        file_name = 'midaco_' + str(2**i) + str(delay) + '.json'
        data = json.load(open(os.path.join(file_folder, file_name)))
        avg_time.append(data['avg_time'])
    return avg_time

def add_line(single_thread_time, multi_threaded_time, delay):
    x = [2, 4, 8, 16]
    y= [single_thread_time/multi_threaded_time[i - 1] for i in range(1, 5)]
    lable = ''
    if delay == '_01':
        lable = 'delay = 0.1'
    elif delay == '_05':
        lable = 'delay = 0.5'
    elif delay == '_1':
        lable = 'delay = 1'
    plt.plot(x, y, label = lable)


def plot_speedup(stats_folder, delays, show=True, filename=None,):
    for delay in delays:
        single_thread_time = get_single_thread_time(stats_folder, delay)
        multi_threaded_time = get_multi_threaded_time(stats_folder, delay)
        add_line(single_thread_time, multi_threaded_time, delay)

    plt.legend ()
    plt.xlabel(r'$Treads$') 
    plt.ylabel(r'$Speedup$') 
    plt.title(r'$midaco$')
    plt.grid(True)

    if filename:
        plt.savefig(filename, dpi=300)

    if show:
        plt.show()


def main(args):
    delays = ['_01', '_05', '_1']
    plot_speedup(args.stats_folder, delays,
                 show = args.show, filename = os.path.join(args.stats_folder, 'speedup.pdf'))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('stats_folder', type=str)
    parser.add_argument('--show', action='store_true')
    parser.add_argument('--output', type=str, default='')

    main(parser.parse_args())
