package com.sushshring.zookeepermonkey;

    import org.apache.log4j.BasicConfigurator;
    import org.apache.log4j.Logger;

public class App {
    public static void main(String[] args) throws InterruptedException {
        if (args.length < 3) {
            System.err
                    .println("USAGE: Executor hostPort znode filename program [args ...]");
            System.exit(2);
        }
        String hostPort = args[0];
        String znode = args[1];
        String filename = args[2];
        BasicConfigurator.configure();
        Logger logger = Logger.getLogger(App.class);
        Thread.sleep(10000);
        logger.debug("Starting application");
        try {
            new Executor(hostPort, znode, filename).run();
        } catch (Exception e) {
            logger.error(e);
        }
    }
}
