package com.will.openslesrecord;

import android.content.Context;
import android.os.Environment;
import android.os.StatFs;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * 判断ＳＤ卡是否挂载
 * 获取ＳＤ卡的根目录
 * 获取ＳＤ卡总容量
 * 获取ＳＤ卡剩余容量
 * 向ＳＤ卡９大共有目录保存数据
 * 向ＳＤ卡私有File目录写入数据
 * 向ＳＤ卡私有Cache目录写入数据
 * 从ＳＤ卡中读取数据
 */
public class SDUtils {

    /**
     * 判断ＳＤ卡是否挂载
     *
     * @return
     */
    public static boolean isSDCardMounted() {
        String state = Environment.getExternalStorageState();
        return state.equals(Environment.MEDIA_MOUNTED);
    }

    /**
     * @return 返回ＳＤ卡根目录
     */
    public static String getSDCardRootDir() {
        return Environment.getExternalStorageDirectory().getPath();
    }

    /***
     *
     * @return 获取ＳＤ卡总容量  返回ＭＢ
     */
    public static long getSDCardTotalSize() {
        if (isSDCardMounted()) {
            //StatFs Statistic File System  簇
            StatFs sf = new StatFs(getSDCardRootDir());
            int blockSize = sf.getBlockSize();
            int blockCount = sf.getBlockCount();
            return blockSize * blockCount / 1024 / 1024;
        }
        return 0;
    }

    /**
     * @return 获取ＳＤ卡可用容量  返回ＭＢ
     */
    public static long getSDCardAvailableSize() {
        if (isSDCardMounted()) {
            StatFs sf = new StatFs(getSDCardRootDir());
            int availableBlockCount = sf.getAvailableBlocks();
            int blockSize = sf.getBlockSize();

            return availableBlockCount * blockSize / 1024 / 1024;
        }
        return 0;
    }

    /**
     * 向ＳＤ卡９大共有目录保存数据
     *
     * @param data     需要保存的数据
     * @param type     区分９大共有目录的type
     * @param fileName 另存为的文件名称
     * @return
     */
    public static boolean saveFileToPublicDirectory(byte[] data,
                                                    String type, String fileName) {
        if (isSDCardMounted()) {
            BufferedOutputStream bos = null;
            File fileDir =
                    Environment.getExternalStoragePublicDirectory(type);
            File file = new File(fileDir, fileName);
            if (!file.exists()) {
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            try {
                bos = new BufferedOutputStream(new FileOutputStream(file));
                bos.write(data);
                bos.flush();
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (bos != null) {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return false;
    }

    /**
     * 创建文件
     * @param context
     * @param fileName
     * @return
     */
    public static String crateFile(Context context,String fileName){
        if (isSDCardMounted()){

            File fileDir = context.getExternalFilesDir(null);
            File filePath = new File(fileDir,"will");
            if (!filePath.exists()){
                filePath.mkdirs();
            }
            File file = new File(filePath, fileName);
            if (!file.exists()){
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                    return null;
                }
            }
            return file.getAbsolutePath();
        }

        return null;
    }

    /**
     * 向ＳＤ卡私有File目录写入数据
     *
     * @param context  　上下文，用来找到storage/sdcard0/Android/data/packageName/files
     * @param data     需要保存的数据
     * @param fileName 　另存为的文件名名称
     * @return
     */
    public static boolean saveFileToExternalFileDir(Context context, byte[] data, String fileName) {
        if (isSDCardMounted()) {
            BufferedOutputStream bos = null;
            File fileDir = context.getExternalFilesDir(null);
            File file = new File(fileDir, fileName);
            if (!file.exists()) {
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            try {
                bos = new BufferedOutputStream(new FileOutputStream(file));
                bos.write(data);
                bos.flush();
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (bos != null) {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return false;
    }

    /**
     * 向ＳＤ卡私有Cache目录写入数据
     *
     * @param context  　上下文，用来找到storage/sdcard0/Android/data/packageName/cache
     * @param data     需要保存的数据
     * @param fileName 　另存为的文件名名称
     * @return
     */
    public static boolean saveFileToExternalCacheDir(Context context, byte[] data,
                                                     String fileName) {
        if (isSDCardMounted()) {
            BufferedOutputStream bos = null;
            File fileDir = context.getExternalCacheDir();
            File file = new File(fileDir, fileName);
            if (!file.exists()) {
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            try {
                bos = new BufferedOutputStream(new FileOutputStream(file));
                bos.write(data);
                bos.flush();
                return true;
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (bos != null) {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return false;
    }

    /**
     * 从ＳＤ卡中读取数据
     *
     * @param fileAbsolutePath 　　读取文件的绝对路径
     * @return
     */
    public static byte[] loadDataFromSDCard(String fileAbsolutePath) {
        if (isSDCardMounted()) {
            BufferedInputStream bis = null;
            ByteArrayOutputStream baos = new ByteArrayOutputStream();

            try {
                bis = new BufferedInputStream(
                        new FileInputStream(fileAbsolutePath));
                byte[] buffer = new byte[1024 * 8];
                int len = 0;
                while ((len = bis.read(buffer)) != -1) {
                    baos.write(buffer, 0, len);
                    baos.flush();
                }
                return baos.toByteArray();
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                try {
                    if (bis != null) {
                        bis.close();
                    }
                    if (baos != null) {
                        baos.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    public static String getSDCardCacheDir(Context context) {
        return context.getExternalCacheDir().getPath();
    }
}