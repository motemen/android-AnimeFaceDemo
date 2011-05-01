package net.tokyoenvious.droid.animeface;

import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.Color;

public class AnimeFace {

    public static Result[] detect (int[] pixels, int width, int height) {
        float[] result = nvxsDetect(pixels, width, height);
        return Result.resultsFromFloatArray(result);
    }

    public native static float[] nvxsDetect(int[] pixels, int w, int h);

    static {
        System.loadLibrary("nvxs-bridge");
    }

    public static class Result {
        private static Rect rectFromFloats (float[] floats, int i) {
            return new Rect(
                (int)floats[i],   // x
                (int)floats[i+1], // y
                (int)(floats[i] + floats[i+2]),  // x + w
                (int)(floats[i+1] + floats[i+3]) // y + h
            );
        }

        private static int colorFromFloats (float[] floats, int i) {
            return Color.rgb((int)floats[i+2], (int)floats[i+1], (int)floats[i]);
        }

        private static Point pointFromFloats (float[] floats, int i) {
            return new Point((int)floats[i], (int)floats[i+1]);
        }

        static Result[] resultsFromFloatArray (float[] floats) {
            Result[] results = new Result[ floats.length / 51 ];

            for (int i = 0, r = 0; i < floats.length; r++) {
                Result result = new Result();

                result.likelihood = floats[i]; i += 1;
                result.face = rectFromFloats(floats, i); i += 4;
                result.skinColor = colorFromFloats(floats, i); i += 3;
                result.hairColor = colorFromFloats(floats, i); i += 3;
                result.leftEye = rectFromFloats(floats, i); i += 4;
                result.leftEyeColors = new int[4];
                for (int j = 0; j < 4; ++j) {
                    result.leftEyeColors[j] = colorFromFloats(floats, i); i += 3;
                }

                result.rightEye = rectFromFloats(floats, i); i += 4;
                result.rightEyeColors = new int[4];
                for (int j = 0; j < 4; ++j) {
                    result.rightEyeColors[j] = colorFromFloats(floats, i); i += 3;
                }

                result.nose = pointFromFloats(floats, i); i += 2;
                result.mouth = rectFromFloats(floats, i); i += 4;
                result.chin = pointFromFloats(floats, i); i += 2;

                results[r] = result;
            }

            return results;
        }

        public float likelihood;
        public Rect face;
        public int skinColor;
        public int hairColor;
        public Rect leftEye;
        public int[] leftEyeColors;
        public Rect rightEye;
        public int[] rightEyeColors;
        public Point nose;
        public Rect mouth;
        public Point chin;

        public String toString () {
            return "likelihood=" + likelihood
                + " face=" + face
                + " skinColor=" + Integer.toHexString(skinColor)
                + " hairColor=" + Integer.toHexString(hairColor)
                + " leftEye=" + leftEye
                + " leftEyeColors=" + leftEyeColors.toString()
                + " rightEye=" + rightEye
                + " rightEyeColors=" + rightEyeColors.toString()
                + " nose=" + nose
                + " mouth=" + mouth
                + " chin=" + chin;
        }
    }
}

