#!/usr/bin/env python3.7
from analyzer import Analyzer
from parser.arg_parser import ArgParser
from parser.data_parser import Parser


# def movingaverage(values, window):
#     weights = np.repeat(1.0, window) / window
#     sma = np.convolve(values, weights, 'valid')
#     return sma


# r = movingaverage(rtts, 1000)
# lt = np.percentile(r, 0.05)
# ht = np.percentile(r, 99.95) 
# r_nooutlier = filter(lambda x: x <= ht and x >= lt, r)
# plt.figure()
# plt.hist(r_nooutlier, bins=1000)
# plt.show()

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
def main():
    arg_parser = ArgParser()
    parser = Parser(arg_parser.file)
    analyzer = Analyzer(parser.rtts, parser.rtts_control)
    analyzer.analyze(arg_parser)


if __name__ == '__main__':
    main()
