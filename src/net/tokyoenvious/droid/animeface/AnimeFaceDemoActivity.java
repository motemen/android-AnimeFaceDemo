package net.tokyoenvious.droid.animeface;

import java.io.IOException;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.AsyncTask;

import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import android.provider.MediaStore;

import android.widget.ImageView;
import android.widget.Toast;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;

import android.util.Log;

public class AnimeFaceDemoActivity extends Activity {
    static int REQUEST_CODE_PICK_IMAGE = 1;

    private ProgressDialog dialog;

    @Override
    public void onCreate (Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = new Intent(Intent.ACTION_PICK);
        intent.setType("image/*");
        startActivityForResult(intent, REQUEST_CODE_PICK_IMAGE);
    }

    @Override
    public void onActivityResult (int requestCode, int resultCode, Intent data) {
        Uri uri = data.getData();
        ContentResolver resolver = getContentResolver();
        
        try {
            Bitmap bmp = MediaStore.Images.Media.getBitmap(resolver, uri);
            Bitmap bitmap = bmp.copy(Bitmap.Config.RGB_565, true);  

            ImageView image = new ImageView(this);
            image.setImageBitmap(bitmap);
            setContentView(image);

            dialog = new ProgressDialog(this);
            dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            dialog.setMessage("Processing...");
            dialog.show();

            new NvxsDetectTask().execute(bitmap);
        } catch (IOException e) {
        }
    }

    private class NvxsDetectTask extends AsyncTask<Bitmap, Void, AnimeFace.Result[]> {
        private long start;
        private Bitmap bitmap;
        private int width, height;

        @Override
        protected AnimeFace.Result[] doInBackground (Bitmap... bitmaps) {
            bitmap = bitmaps[0];

            width = bitmap.getWidth();  
            height = bitmap.getHeight();  

            int pixels[] = new int[width * height];  
            Log.d("animefacedemo", "width: " + width + ", height: " + height);
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height); 

            Log.d("animefacedemo", "detect start");
            start = System.currentTimeMillis();

            return AnimeFace.detect(pixels, width, height);

        }

        @Override
        protected void onProgressUpdate (Void... progress) {
        }

        @Override
        protected void onPostExecute (AnimeFace.Result[] results) {
            Log.d("animefacedemo", "detect end");

            long diff = System.currentTimeMillis() - start;
            Log.d("animefacedemo", "took " + diff + "ms");

            Canvas canvas = new Canvas(bitmap);
            Paint p = new Paint();
            p.setARGB(255, 255, 0, 0);
            p.setStyle(Paint.Style.STROKE);

            for (int i = 0; i < results.length; i++) {
                Log.d("animefacedemo", "result: " + results[i]);
                canvas.drawRect(results[i].face, p);
            }

            ImageView image = new ImageView(AnimeFaceDemoActivity.this);
            image.setImageBitmap(bitmap);
            setContentView(image);

            dialog.dismiss();

            Toast.makeText(AnimeFaceDemoActivity.this, "" + width + "x" + height + ": " + diff + "ms", Toast.LENGTH_LONG).show();
        }
    }
}
