package com.bzf.ffmpegdemo

import android.app.Activity
import android.content.Context
import android.content.ContextWrapper


/**
 * @author：luck
 * @date：2021/7/14 3:15 PM
 * @describe：ImageLoaderUtils
 */
object ImageLoaderUtils {
    fun assertValidRequest(context: Context?): Boolean {
        if (context is Activity) {
            return !isDestroy(context)
        } else if (context is ContextWrapper) {
            val contextWrapper = context
            if (contextWrapper.baseContext is Activity) {
                val activity = contextWrapper.baseContext as Activity
                return !isDestroy(activity)
            }
        }
        return true
    }

    private fun isDestroy(activity: Activity?): Boolean {
        return if (activity == null) {
            true
        } else activity.isFinishing || activity.isDestroyed
    }
}