#!/usr/bin/env python3.7
# Set variables for scripts
import sys
from subprocess import Popen

def run(server = '192.168.1.103', port = '6000', orchestrator='k8', flooding='4x'):
    client_cmd = ['./client/client', '-v', server, port]
    proc = Popen(client_cmd)
    print('Restart the flooder and press enter....')
    sys.stdin.readline()
    analysis_cmd = ['./analysis/analyze.py', '--show-rtts', '--show-histogram', '--show-histogram-control', '--remove-outliers', '--outlier-removal-type', '3', '--full-histogram', './client/client_times', '%s with %s flooding' % (orchestrator, flooding level)]
    proc1 = Popen(analysis_cmd)
    now = datetime.now()
    mv_cmd = ['mv', './client/client_times', ('./data/client_times_%s_%s_intf_%s%s%s_%s%s') % (orchestrator, flooding,
        now.month, now.day, now.year, now.hour, now.second)]
    proc2 = Popen(mv_cmd)
    return 0
    pass

if __name__ == '__main__':
    while True:
        run(*sys.argv[1:])
