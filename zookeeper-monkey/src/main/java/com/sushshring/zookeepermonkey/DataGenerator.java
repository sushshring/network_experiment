package com.sushshring.zookeepermonkey;

import org.apache.log4j.Logger;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;

import java.util.Random;

public class DataGenerator extends DataWrangler implements Watcher, Runnable {

    private final ZookeeperData zookeeperData;
    private final Random rng = new Random();
    private Logger logger = Logger.getLogger(DataGenerator.class);

    DataGenerator(ZookeeperData data) {
        super(data);
        this.zookeeperData = data;
    }

    @Override
    protected Logger logger() {
        return logger;
    }

    @Override
    protected void processEventInternal(WatchedEvent event) {
        try {
            Thread.sleep(this.generateWaitTime());
            zookeeperData.getZookeeper().setData(zookeeperData.getZnode(), this.generateData(), 1);
            this.run();
        } catch (KeeperException | InterruptedException e) {
            logger.error(e);
        }
    }

    @Override
    protected void processResultInternal() {

    }

    @Override
    protected void handleDeath() {

    }


    @Override
    public void run() {
        synchronized (this) {
            while (true) {
                try {
                    logger.debug("Sending new message");
                    zookeeperData.getZookeeper().setData(zookeeperData.getZnode(), this.generateData(), 1);
                    Thread.sleep(this.generateWaitTime());
                } catch (InterruptedException | KeeperException e) {
                    logger.error(e);
                }
            }
        }
    }

    private byte[] generateData() {
        byte[] bytes = new byte[1024];
        this.rng.nextBytes(bytes);
        return bytes;
    }

    private long generateWaitTime() {
        return this.rng.nextInt(10000);
    }
}
