import numpy as np
import matplotlib.pyplot as plt
rtts = []
startimes = []
iars = []
times = open('client_times')
lastend = 1547225497162683600
flooder_times = open('../flooder/flooder_times')
floodersy = []
floodersx = []
while True:
    sline = times.readline()
    eline = times.readline()
    if not eline:
        break
    stime = float(sline.split()[1])
    etime = float(eline.split()[1])
    rtt = etime - stime
    rtts.append(rtt)
    startimes.append(stime)
    iars.append(stime - lastend)
    lastend = etime
#while True:
#    time = flooder_times.readline()
#    if not time:
#        break
#    ftime = float(time.split()[1])
#    if ftime >= startimes[0] and ftime <= startimes[-1]:
#        floodersy.append(ftime)
#        floodersx.append(0)
# print(",".join(map(str, rtts)))
norm_starttimes = map(lambda x: (x - startimes[0]) / (10**9), startimes)
print(np.var(iars) / (10**9))
fig, ax = plt.subplots(1)
#f2, ax2 = plt.subplots(1)
ax.plot(norm_starttimes, rtts, 'b', norm_starttimes, iars, 'r')
#ax2.hist(iars)
# fig, ax = plt.subplots()
# ax[0].hist(iars)
# ax[0].show()
# plt.hist(iars)
plt.show()
# plt.plot(iars, startimes)
# plt.show()
