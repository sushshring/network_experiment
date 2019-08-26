package com.sushshring.zookeepermonkey;

import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;

final class ZookeeperData {

    private final ZooKeeper zookeeper;
    private final String znode;
    private final Watcher watcher;

    ZookeeperData(ZooKeeper zk, String znode, Watcher chainedWatcher) {
        this.zookeeper = zk;
        this.znode = znode;
        this.watcher = chainedWatcher;
    }

    ZooKeeper getZookeeper() {
        return zookeeper;
    }

    String getZnode() {
        return znode;
    }

    Watcher getWatcher() {
        return watcher;
    }
}
