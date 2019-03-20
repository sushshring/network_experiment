import numpy as np
import matplotlib.pyplot as plt
rtts = []
startimes = []
times = open('client_times')
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
norm_starttimes = map(lambda x: (x - startimes[0]) / (10**9), startimes)
fig, ax = plt.subplots(1)
print(','.join(map(str, map(int, rtts))))
ax.plot(norm_starttimes, rtts, 'b')
plt.show()
