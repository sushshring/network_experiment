from analyzer import Analyzer
from parser.arg_parser import ArgParser
from parser.data_parser import Parser


# def movingaverage(values, window):
#     weights = np.repeat(1.0, window) / window
#     sma = np.convolve(values, weights, 'valid')
#     return sma


# bb = remove_outliers_pct([x[1] for x in rtts])
# plt.figure()
# plt.plot([x[0] for x in rtts], [x[1] for x in rtts])
# plt.show()
# print(','.join([str(x[0]) for x in rtts]))
# print(','.join([str(x[1]) for x in rtts]))


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
# print(stats.mannwhitneyu(a, b, False))
# plt.hist(a, bins=1000, color='Orange', alpha=0.5, label='With flooder')
# plt.hist(b, bins=1000, color='Blue', alpha=0.5, label='Without flooder')
# plt.legend()
# plt.show()

# a = remove_outliers_pct(remove_outliers_pct(rtts_w_flooder))
# b = remove_outliers_pct(remove_outliers_pct(rtts_wo_flooder))
# plt.hist(a, bins=1000, color='Orange', alpha=0.5, label='With flooder')
# plt.hist(b, bins=1000, color='Blue', alpha=0.5, label='Without flooder')
# plt.legend()
# plt.show()


# def remove_outliers(elements):
#     mean = np.mean(elements, axis=0)
#     sd = np.std(elements, axis=0)
#     final_list = [x for x in elements if (x > mean - 0.5 * sd)]
#     final_list = [x for x in final_list if (x < mean + 0.5 * sd)]
#     return np.array(final_list)

# def norm_fit_plot(data):
#     plt.figure()
#     # prob, pearson = stats.boxcox_normplot(data, -20, 20, plot=plt)
#     # print(prob, pearson)
#     # plt.show()
#     #
#     loc, scale = stats.norm.fit(data)
#     print(loc, scale)
#     n = stats.norm(loc=loc, scale=scale)
#     sample = np.random.choice(data, 1000)
#     plt.hist(data, bins=5000)
#     x = np.arange(data.min(), data.max() + 0.2, 0.2)
#     # plt.plot(data)
#     print(stats.shapiro(data))
#     plt.show()

#
# conv = stats.boxcox(remove_outliers_pct(rtts), 0)
# norm_fit_plot(conv)

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
def main():
    arg_parser = ArgParser()
    parser = Parser(arg_parser.file)
    analyzer = Analyzer(parser.rtts)
    analyzer.analyze(arg_parser)


if __name__ == '__main__':
    main()
