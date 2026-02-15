package org.ntop.n2n;

public class Supernode {
    static {
        System.loadLibrary("n2n_jni");
    }

    private long nativePtr; // 用于存储 C 层的 n2n_sn_t 指针

    /**
     * 初始化 supernode 配置
     * @param port 监听端口
     * @param mgmtPort 管理端口
     * @return 0 成功，非 0 失败
     */
    public native int init(int port, int mgmtPort);

    /**
     * 运行 supernode 主循环（阻塞）
     * @return 运行结果状态码
     */
    public native int run();

    /**
     * 停止 supernode
     */
    public native void stop();

    /**
     * 释放资源
     */
    public native void destroy();

    public static void main(String[] args) {
        Supernode sn = new Supernode();
        int port = 1234;
        int mgmtPort = 5645;
        
        if (args.length >= 1) {
            port = Integer.parseInt(args[0]);
        }
        if (args.length >= 2) {
            mgmtPort = Integer.parseInt(args[1]);
        }

        System.out.println("Starting n2n supernode on port " + port + ", mgmt port " + mgmtPort);
        
        if (sn.init(port, mgmtPort) == 0) {
            // 在新线程中运行，或者直接运行（如果这是主程序）
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                System.out.println("Shutting down...");
                sn.stop();
            }));
            
            int result = sn.run();
            System.out.println("Supernode exited with code: " + result);
            sn.destroy();
        } else {
            System.err.println("Failed to initialize supernode");
        }
    }
}
