package com.bzf.ffmpegdemo

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import com.bzf.ffmpegdemo.databinding.ActivityMainBinding
import java.io.File
import java.util.*

const val REQUEST_VIDEO_OPEN = 1
private val PERMISSIONS_REQUIRED = Manifest.permission.READ_EXTERNAL_STORAGE

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val cpuAPI = android.os.Build.SUPPORTED_ABIS
        Log.d("bzf", Arrays.toString(cpuAPI))

        binding.selectBt.setOnClickListener {
//                printFFmpegInfo()
//            openVideo()
//            decodeMP2()
//            decodeMP4()
            demuxingDecoding()
//            startEncodeAudio()
//            startYUVToH264()
        }


    }

    private fun startYUVToH264(){
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        val encodeDir = File(dir, "encode")
        if(!encodeDir.exists()){
            encodeDir.mkdir()
        }
        val outputPath = encodeDir.absolutePath + "/output.h264"
        Thread{
            yuvEncodeH264(outputPath)
        }.start()
    }

    private fun startEncodeAudio() {
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        val encodeDir = File(dir, "encode")
        encodeDir.mkdir()
        val outputPath = encodeDir.absolutePath + "/output.mp2"
        Thread{
            encodeAudio("", outputPath)
        }.start()
    }

    private fun printFFmpegInfo() {
        Thread {
            val ffmpegInfo = ffmpegInfo()
            binding.contentTV.post {
                binding.contentTV.text = ffmpegInfo
            }
        }.start()
    }

    private fun demuxingDecoding() {
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        val listFiles = dir.listFiles()
        if (listFiles.isNotEmpty()) {
            var videoPath: String = ""
            for (file in listFiles) {
                if (file.name.endsWith(".mp4")) {
                    videoPath = file.absolutePath
                    break
                }
            }
            val outputDir = File(dir, "demuxing")
            outputDir.mkdir()
            Thread {
                demuxingDecoding(videoPath, outputDir.absolutePath)
            }.start()
        }
    }

    private fun decodeMP4() {
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        val listFiles = dir.listFiles()
        if (listFiles.isNotEmpty()) {
            val videoPath = "/storage/emulated/0/Android/media/com.bzf.ffmpegdemo/test.mpeg"
            val outputDir = File(dir, "PGM")
            outputDir.mkdir()
            Thread {
                decodeVideo(videoPath, outputDir.absolutePath)
            }.start()
        }
    }

    private fun decodeMP2() {
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        Log.i("bzf", "dir=" + dir)
        val listFiles = dir.listFiles()
        if (listFiles.isNotEmpty()) {
//            val filePath = listFiles[1].absolutePath
//            Log.i("bzf","file="+filePath)
            Thread {
                // filePath=/storage/emulated/0/Android/media/com.bzf.ffmpegdemo/test.mp2
                decodeAudio(dir.absolutePath + "/test.mp2", dir.absolutePath + "/output.pcm")
            }.start()
        }
    }

    private fun openVideo() {
        val externalMediaDirs = externalMediaDirs
        val dir = externalMediaDirs[0]
        val listFiles = dir.listFiles()
        if (listFiles.isNotEmpty()) {
            val filePath = listFiles[0].absolutePath
            Log.i("bzf", "file=" + filePath)
            Thread {
                readingAV(filePath)
            }.start()
        }
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun ffmpegInfo(): String
    external fun mp4ToAVI(inputPath: String, outputPath: String): Boolean

    //将一个视频文件解码成为单独的音频PCM文件和视频YUV文件
    external fun videoDecodePCMAndYUV(videoPath: String, pcmUrl: String, yuvUrl: String)
    external fun readingAV(avUrl: String)
    external fun decodeAudio(inputPath: String, outputPath: String)
    external fun decodeVideo(inputPath: String, outputDir: String)
    external fun demuxingDecoding(inputPath: String, outputDir: String)
    external fun encodeAudio(inputPath: String, outputPath: String)
    external fun yuvEncodeH264(outputPath: String)

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }

        /*权限数组里所有权限都已经被允许了，就返回true*/
        @JvmStatic
        fun hasPermissions(context: Context) = (ContextCompat.checkSelfPermission(
            context,
            PERMISSIONS_REQUIRED
        ) == PackageManager.PERMISSION_GRANTED)
    }
}