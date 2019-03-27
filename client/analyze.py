import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy import stats

intf_times = open(sys.argv[1])
# no_intf_times = open(sys.argv[2])

def remove_outliers_pct(elements):
    lt = np.percentile(elements, 0.05)
    ht = np.percentile(elements, 99.95) 
    return filter(lambda x: x <= ht and x >= lt, elements)

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
            # rtts.append(rtt)
            if flooder_on:
                rtts_w_flooder.append(((stime-first_start) / (10**9), rtt))
                # rtts_w_flooder.append(rtt)
            else:
                rtts_wo_flooder.append(((stime-first_start) / (10**9), rtt))
                # rtts_wo_flooder.append(rtt)
    return rtts, rtts_w_flooder, rtts_wo_flooder
#norm_starttimes = map(lambda x: (x - startimes[0]) / (10**9), startimes)
#fig, ax = plt.subplots(1)
#print(','.join(map(str, map(int, rtts))))
#ax.plot(norm_starttimes, rtts, 'b')
#plt.show()
rtts, rtts_w_flooder, rtts_wo_flooder = parse_client_times(intf_times)


bb = remove_outliers_pct([x[1] for x in rtts])
plt.figure()
plt.plot([x[0] for x in rtts], [x[1] for x in rtts])
plt.show()
print(','.join([str(x[0]) for x in rtts]))
print(','.join([str(x[1]) for x in rtts]))


# r = movingaverage(rtts, 1000)
# lt = np.percentile(r, 0.05)
# ht = np.percentile(r, 99.95) 
# r_nooutlier = filter(lambda x: x <= ht and x >= lt, r)
# plt.figure()
# plt.hist(r_nooutlier, bins=1000)
# plt.show()


# a = movingaverage(rtts_w_flooder, 100)
# b = movingaverage(rtts_wo_flooder, 100)
# print(len(a), len(b))
# print(np.mean(a), np.mean(b))
# print(np.std(a), np.std(b))
# # print(stats.mannwhitneyu(a, b, False))
# plt.hist(a, bins=1000, color='Orange', alpha=0.5, label='With flooder')
# plt.hist(b, bins=1000, color='Blue', alpha=0.5, label='Without flooder')
# plt.legend()
# plt.show()

# # a = remove_outliers_pct(remove_outliers_pct(rtts_w_flooder))
# # b = remove_outliers_pct(remove_outliers_pct(rtts_wo_flooder))
# # plt.hist(a, bins=1000, color='Orange', alpha=0.5, label='With flooder')
# # plt.hist(b, bins=1000, color='Blue', alpha=0.5, label='Without flooder')
# # plt.legend()
# # plt.show()



# def remove_outliers(elements):
#     mean = np.mean(elements, axis=0)
#     sd = np.std(elements, axis=0)
#     final_list = [x for x in elements if (x > mean - 0.5 * sd)]
#     final_list = [x for x in final_list if (x < mean + 0.5 * sd)]
#     return np.array(final_list)

# def norm_fit_plot(data):
#     plt.figure()
#     loc, scale = stats.norm.fit(data)
#     print(loc, scale)
#     n = stats.norm(loc=loc, scale=scale)
#     sample = np.random.choice(data, 1000)
#     plt.hist(sample, bins=5000)
#     x = np.arange(sample.min(), sample.max()+0.2, 0.2)
#     plt.plot(x, 8500000 * n.pdf(x))
#     print(stats.shapiro(sample))
#     plt.show()
# norm_fit_plot(remove_outliers(np.array(rtts)))

# def ks_plot_norm(data):
#     length = len(data)
#     plt.figure(figsize=(12, 7))
#     plt.plot(np.sort(data), np.linspace(0, 1, len(data), endpoint=True))
#     plt.plot(np.sort(stats.norm.rvs(loc=np.mean(data), scale=np.std(data), size=length)), np.linspace(0, 1, len(data), endpoint=True))
#     plt.legend('top right')
#     plt.legend(['Data', 'Theoretical Values'])
#     plt.title('Comparing CDFs for KS-Test')
# ks_plot_norm(a)
# sample = np.random.choice(rtts, 10000)
# print(np.mean(sample), np.std(sample))
# print(stats.kstest(sample, 'norm', args=(np.mean(sample), np.std(sample))))
# ks_plot_norm(rtts)
# plt.hist(sample, bins=100)
# plt.show()

# fig1, ax = plt.subplots(1)
# ax.scatter(*zip(*rtts_wo_flooder), label='Without flooder')
# ax.scatter(*zip(*rtts_w_flooder), label='With flooder')
# ax.plot(*zip(*rtts))
# ax.legend()


# fig2, ax2 = plt.subplots(1)
# b, a = signal.butter(3, 0.05)
# justSendTime, justRtt = [x[0] for x in rtts], [x[1] for x in rtts]
# print(','.join([str(x) for x in justRtt]) + ';')
# print(','.join([str(x) for x in justSendTime]) + ';')
# filtered = signal.lfilter(b, a, justRtt)
# norm_filtered = np.divide(np.subtract(filtered, np.mean(filtered)), np.std(filtered))

# t = np.linspace(0, justSendTime[-1] - justSendTime[0], len(justSendTime))
# pureSquare = signal.square(0.1 * t * (2 * np.pi))

# ax2.plot(justSendTime, norm_filtered)
# ax2.plot(justSendTime, pureSquare)
# nrtts, nrtts_w_flodoer, nrtts_wo_flooder = parse_client_times(no_intf_times)
# njustSendTime, njustRtt = [x[0] for x in nrtts], [x[1] for x in nrtts] 
# plt.show()
# D, pval = stats.ks_2samp(njustRtt, justRtt)
# print(D, pval)

# D, pval = stats.ks_2samp(justRtt, pureSquare)
# print(D, pval)
# D, pval = stats.ks_2samp(njustRtt, pureSquare)
# print(D, pval)
# fig2, ax3 = plt.subplots(1)
# ax3.scatter(*zip(*rtts_wo_flooder), label='Without flooder')
# # ax3.scatter(*zip(*rtts_w_flooder), label='With flooder')
# ax3.plot(*zip(*rtts))
# ax3.legend()
# fig4, ax4 = plt.subplots(1)

# nb, na = signal.butter(3, 0.05)
# njustSendTime, njustRtt = [x[0] for x in nrtts], [x[1] for x in nrtts]
# nfiltered = signal.lfilter(nb, na, njustRtt)
# nnorm_filtered = np.divide(np.subtract(nfiltered, np.mean(nfiltered)), np.std(nfiltered))
# ax4.plot(njustSendTime, nnorm_filtered)
# ax4.plot(t, pureSquare)
# plt.show()
