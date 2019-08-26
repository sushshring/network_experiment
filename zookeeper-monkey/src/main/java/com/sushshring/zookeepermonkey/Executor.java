package com.sushshring.zookeepermonkey;

import org.apache.log4j.Logger;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;

import java.io.FileOutputStream;
import java.io.IOException;

public class Executor implements Watcher, Runnable, DataMonitor.DataMonitorListener {

    private final DataMonitor dm;
    private final String filename;
    private Logger logger = Logger.getLogger(Executor.class);

    Executor(String hostPort, String znode, String filename) throws IOException {
        this.filename = filename;
        ZooKeeper zk = new ZooKeeper(hostPort, 3000, this);
        ZookeeperData zookeeperData = new ZookeeperData(zk, znode, null);
        new DataGenerator(zookeeperData);
        dm = new DataMonitor(zookeeperData, this);
    }

    public void run() {
        try {
            synchronized (this) {
                while (!dm.dead()) {
                    wait();
                }
            }
        } catch (InterruptedException e) {
            logger.error(e);
        }
    }

    @Override
    public void exists(byte[] data) {
        if (data != null) {
            try {
                FileOutputStream fos = new FileOutputStream(filename);
                fos.write(data);
                fos.close();
            } catch (IOException e) {
                logger.error(e);
            }
        }
    }

    public void closing(int rc) {
        synchronized (this) {
            notifyAll();
        }
    }

    @Override
    public void process(WatchedEvent watchedEvent) {
        dm.process(watchedEvent);
    }
}
