#!/usr/bin/env python3.7
# Set variables for scripts
import sys
import subprocess
from datetime import datetime
import argparse


def run_experiment(server='192.168.1.103', port='6000', orchestrator='k8', flooding='quad'):
    print('Restart the flooder now....')
    client_cmd = ['../client/client', '-v', server, port]
    proc = subprocess.run(client_cmd)
    print('Finished running client. Hit enter to continue...')
    sys.stdin.readline()
    analysis_cmd = ['./analyze.py', '--show-rtts', '--show-histogram', '--show-histogram-control',
                    '--remove-outliers', '--outlier-removal-type', '3', '--full-histogram', './client/client_times',
                    '%s with %s flooding' % (orchestrator, flooding)]
    proc1 = subprocess.run(analysis_cmd)
    now = datetime.now()
    mv_cmd = ['mv', '../client/client_times', '../data/client_times_%s_%s_intf_%s%s%s_%s%s' % (
        orchestrator, flooding, now.month, now.day, now.year, now.hour, now.second)]
    proc2 = subprocess.run(mv_cmd)
    return 0
    pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run the experiment')
    parser.add_argument('--server', type=str, help='IP address of the server')
    parser.add_argument('--port', type=str, help='port of the server')
    parser.add_argument('--orchestrator', type=str, help='Orchestrator type for data and labels')
    parser.add_argument('--flooding', type=str, help='Flooding level for data and labels')
    args = parser.parse_args()
    print(args)
    while True:
        run_experiment(server=args.server, port=args.port, orchestrator=args.orchestrator, flooding=args.flooding)
