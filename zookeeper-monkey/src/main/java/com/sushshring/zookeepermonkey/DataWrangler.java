package com.sushshring.zookeepermonkey;

import org.apache.zookeeper.*;
import org.apache.zookeeper.data.Stat;
import org.apache.log4j.Logger;

public abstract class DataWrangler implements AsyncCallback.StatCallback, Watcher {
    protected final ZookeeperData zk;

    private boolean init = false;
    public DataWrangler(ZookeeperData zookeeperData) {
        this.zk = zookeeperData;
        // Get things started by checking if the node exists. We are going
        // to be completely event driven
        zk.getZookeeper().exists(zk.getZnode(), true, this, null);
    }

    @Override
    public void process(WatchedEvent event) {
        if (event.getType() == Watcher.Event.EventType.None) {
            // We are are being told that the state of the
            // connection has changed
            switch (event.getState()) {
                case SyncConnected:
                    // In this particular example we don't need to do anything
                    // here - watches are automatically re-registered with
                    // server and any watches triggered while the client was
                    // disconnected will be delivered (in order of course)
                    break;
                case Expired:
                    // It's all over
                    handleDeath();
                    break;
            }
        } else {
            if (event.getPath() != null && event.getPath().equals(this.zk.getZnode())) {
                // Something has changed on the node, let's find out
                zk.getZookeeper().exists(zk.getZnode(), true, this, null);
            }
        }
        if (!init) {
            try {
                zk.getZookeeper().create(zk.getZnode(), new byte[0], null, CreateMode.PERSISTENT);
                init = true;
            } catch (KeeperException | InterruptedException e) {
                logger().error(e);
            }
        }
        processEventInternal(event);
    }

    protected abstract Logger logger();

    protected abstract void processEventInternal(WatchedEvent event);

    @Override
    public void processResult(int rc, String path, Object ctx, Stat stat) {
        boolean exists;
        KeeperException.Code code = KeeperException.Code.get(rc);
        switch (code) {
            case OK:
                exists = true;
                break;
            case NONODE:
                exists = false;
                break;
            case SESSIONEXPIRED:
            case NOAUTH:
                handleDeath();
                return;
            default:
                // Retry errors
                zk.getZookeeper().exists(zk.getZnode(), true, this, null);
                return;
        }
        if (exists) {
            processResultInternal();
        }
    }

    protected abstract void processResultInternal();

    protected abstract void handleDeath();
}
