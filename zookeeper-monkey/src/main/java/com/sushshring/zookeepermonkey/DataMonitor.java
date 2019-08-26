package com.sushshring.zookeepermonkey;

import org.apache.log4j.Logger;
import org.apache.zookeeper.AsyncCallback;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;

import java.util.Arrays;

public class DataMonitor extends DataWrangler implements Watcher, AsyncCallback.StatCallback {
    private final DataMonitorListener listener;
    private boolean dead;
    private byte[] prevData;
    private Logger logger = Logger.getLogger(DataMonitor.class);

    DataMonitor(ZookeeperData zookeeperData, DataMonitorListener listener) {
        super(zookeeperData);
        this.listener = listener;
        this.dead = false;
    }

    boolean dead() {
        return dead;
    }

    @Override
    protected Logger logger() {
        return logger;
    }

    public void processEventInternal(WatchedEvent event) {
        logger.debug("Got Watch event");
        if (zk.getWatcher() != null) {
            zk.getWatcher().process(event);
        }
    }

    @Override
    protected void processResultInternal() {
        byte[] b = null;
        try {
            b = zk.getZookeeper().getData(zk.getZnode(), false, null);
            logger.debug("Got data from servers");
        } catch (KeeperException | InterruptedException e) {
            logger.error(e);
        }
        if ((b == null && b != prevData)
                || (b != null && !Arrays.equals(prevData, b))) {
            listener.exists(b);
            prevData = b;
        }
    }

    @Override
    protected void handleDeath() {
        dead = true;
        listener.closing(KeeperException.Code.SESSIONEXPIRED.intValue());
    }

    public interface DataMonitorListener {
        /**
         * The existence status of the node has changed.
         */
        void exists(byte data[]);

        /**
         * The ZooKeeper session is no longer valid.
         *
         * @param rc the ZooKeeper reason code
         */
        void closing(int rc);
    }
}
