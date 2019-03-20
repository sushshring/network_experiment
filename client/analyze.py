import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy import stats

intf_times = open(sys.argv[1])
no_intf_times = open(sys.argv[2])

def movingaverage (values, window):
    weights = np.repeat(1.0, window)/window
    sma = np.convolve(values, weights, 'valid')
    return sma

def parse_client_times(times):
    rtts_wo_flooder = []
    rtts_w_flooder = []
    rtts = []
    startimes = []
    flooder_on = True
    stime = 0
    first_start = None
    while True:
        line = times.readline()
        if not line:
            break
        if line.startswith('FLOODER_START'):
            flooder_on = True
        if line.startswith('FLOODER_END'):
            flooder_on = False
        if line.startswith('START'):
            stime = float(line.split()[1])
            if not first_start:
                first_start = stime
        if line.startswith('END'):
            etime = float(line.split()[1])
            rtt = etime - stime
            rtts.append(((stime-first_start) / (10**9), rtt))
            if flooder_on:
                rtts_w_flooder.append(((stime-first_start) / (10**9), rtt))
            else:
                rtts_wo_flooder.append(((stime-first_start) / (10**9), rtt))
    return rtts, rtts_w_flooder, rtts_wo_flooder
#norm_starttimes = map(lambda x: (x - startimes[0]) / (10**9), startimes)
#fig, ax = plt.subplots(1)
#print(','.join(map(str, map(int, rtts))))
#ax.plot(norm_starttimes, rtts, 'b')
#plt.show()
rtts, rtts_w_flooder, rtts_wo_flooder = parse_client_times(intf_times)


# a = movingaverage(rtts_w_flooder, 100)
# b = movingaverage(rtts_wo_flooder, 100)
# print(len(a), len(b))
# print(np.mean(a), np.mean(b))
# print(np.std(a), np.std(b))
# plt.hist(a, bins=1000, color='Orange', alpha=0.5, label='With flooder')
# plt.hist(b, bins=1000, color='Red', alpha=0.5, label='Without flooder')
# plt.legend()
# plt.show()

fig1, ax = plt.subplots(1)
ax.scatter(*zip(*rtts_wo_flooder), label='Without flooder')
ax.scatter(*zip(*rtts_w_flooder), label='With flooder')
ax.plot(*zip(*rtts))
ax.legend()
fig2, ax2 = plt.subplots(1)

b, a = signal.butter(3, 0.05)
justSendTime, justRtt = [x[0] for x in rtts], [x[1] for x in rtts]
filtered = signal.lfilter(b, a, justRtt)
norm_filtered = np.divide(np.subtract(filtered, np.mean(filtered)), np.std(filtered))

t = np.linspace(0, 60, len(justSendTime))
pureSquare = signal.square(0.1 * t * (2 * np.pi))

ax2.plot(justSendTime, norm_filtered)
nrtts, nrtts_w_flodoer, nrtts_wo_flooder = parse_client_times(no_intf_times)
njustSendTime, njustRtt = [x[0] for x in nrtts], [x[1] for x in nrtts] 
D, pval = stats.ks_2samp(njustRtt, justRtt)
print(D, pval)

D, pval = stats.ks_2samp(justRtt, pureSquare)
print(D, pval)
D, pval = stats.ks_2samp(njustRtt, pureSquare)
print(D, pval)
fig2, ax3 = plt.subplots(1)
ax3.scatter(*zip(*rtts_wo_flooder), label='Without flooder')
# ax3.scatter(*zip(*rtts_w_flooder), label='With flooder')
ax3.plot(*zip(*rtts))
ax3.legend()
fig4, ax4 = plt.subplots(1)

nb, na = signal.butter(3, 0.05)
njustSendTime, njustRtt = [x[0] for x in nrtts], [x[1] for x in nrtts]
nfiltered = signal.lfilter(nb, na, njustRtt)
nnorm_filtered = np.divide(np.subtract(nfiltered, np.mean(nfiltered)), np.std(nfiltered))
ax4.plot(njustSendTime, nnorm_filtered)
ax4.plot(t, pureSquare)
plt.show()
