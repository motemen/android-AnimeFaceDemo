#include <jni.h>

#include "nv_core.h"
#include "nv_ip.h"
#include "nv_ml.h"
#include "nv_face.h"

#define NV_MAX_FACE 20

#define COLOR_R(c) (((c) >> 16) & 0xFF)
#define COLOR_G(c) (((c) >>  8) & 0xFF)
#define COLOR_B(c) (((c) >>  0) & 0xFF)

#define NV_C2V(x) ((float)((unsigned int)x))

jfloatArray Java_net_tokyoenvious_droid_animeface_AnimeFace_nvxsDetect(
        JNIEnv* env,
        jclass klass,
        jintArray pixelsArray,
        jint w,
        jint h
    ) {

    float step = 4.0;
    float scale_factor = 1.095;
    float min_window_size = 42.592;

    static const nv_mlp_t *detector_mlp = &nv_face_mlp_face_00;
    static const nv_mlp_t *face_mlp[] = {
        &nv_face_mlp_face_01,
        &nv_face_mlp_face_02,
        NULL
    };
    static const nv_mlp_t *dir_mlp = &nv_face_mlp_dir;
    static const nv_mlp_t *parts_mlp = &nv_face_mlp_parts;

    jint* pixels = (*env)->GetIntArrayElements(env, pixelsArray, 0);

    nv_matrix_t *bgr = nv_matrix3d_alloc(3, h, w);
    nv_matrix_t *gray = nv_matrix3d_alloc(1, h, w);
    nv_matrix_t *smooth = nv_matrix3d_alloc(1, h, w);
    nv_matrix_t *edge = nv_matrix3d_alloc(1, h, w);
    nv_matrix_t *gray_integral = nv_matrix3d_alloc(1, h + 1, w + 1);
    nv_matrix_t *edge_integral = nv_matrix3d_alloc(1, h + 1, w + 1);

    nv_matrix_zero(bgr);
    nv_matrix_zero(gray);
    nv_matrix_zero(edge);
    nv_matrix_zero(gray_integral);
    nv_matrix_zero(edge_integral);

    nv_rect_t image_size;

    image_size.x = image_size.y = 0;
    image_size.width = gray->cols;
    image_size.height = gray->rows;

    // convert format
    // nv_conv_imager2nv(bgr, gray, im);
    int x, y;
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            int p = pixels[x + y * w];
            NV_MAT3D_V(bgr, y, x, NV_CH_B) = NV_C2V(COLOR_R(p));
            NV_MAT3D_V(bgr, y, x, NV_CH_G) = NV_C2V(COLOR_G(p));
            NV_MAT3D_V(bgr, y, x, NV_CH_R) = NV_C2V(COLOR_B(p));
        }
    }
    nv_gray(gray, bgr);

    // edge
    nv_gaussian5x5(smooth, 0, gray, 0);
    nv_laplacian1(edge, smooth, 4.0f);
    // integral
    nv_integral(gray_integral, gray, 0);
    nv_integral(edge_integral, edge, 0);

    // detect face
    int nface;
    nv_face_position_t face_pos[NV_MAX_FACE];

    nface = nv_face_detect(
        face_pos, NV_MAX_FACE,
        gray_integral, edge_integral, &image_size,
        dir_mlp,
        detector_mlp, face_mlp, 2,
        parts_mlp,
        step, scale_factor, min_window_size
    );

    jfloatArray result = (*env)->NewFloatArray(env, nface * 51);
    jfloat* floatResult = (*env)->GetFloatArrayElements(env, result, NULL);

    // analyze face 
    int i, j;
    int r = 0;
    for (i = 0; i < nface; ++i) {
        nv_face_feature_t face_feature = {0};
        nv_face_analyze(&face_feature, &face_pos[i], bgr);

        /*
         * likelihood => ,
         * face => {x=>,y=>,width=>,height=>, skin_color =>, hair_color=>, },
         * eyes => {left} => { x=>,y=>,width=>,height=> , color => (,,,,)},
         * eyes => {right} => { x=>,y=>,width=>,height=> , color => (,,,,)}, 
         * nose => {x=>, y=>, width=>1, height=>1},
         * mouse => { x=>, y=>, width=>, height=>},
         * chin => { x=>, y =>, width=>1, height=>1}
         */

        //
        // likelihood
        // av_push(results, newSVnv(face_pos[i].likelihood));
        floatResult[r++] = face_pos[i].likelihood;

        // face.x,y,w,h,skin_color(b,g,r),hair_color(b,g,r)
        // av_push(results, newSViv(face_pos[i].face.x));
        // av_push(results, newSViv(face_pos[i].face.y));
        // av_push(results, newSViv(face_pos[i].face.width));
        // av_push(results, newSViv(face_pos[i].face.height));
        floatResult[r++] = face_pos[i].face.x;
        floatResult[r++] = face_pos[i].face.y;
        floatResult[r++] = face_pos[i].face.width;
        floatResult[r++] = face_pos[i].face.height;

        // av_push(results, newSViv(face_feature.skin_bgr.v[0]));
        // av_push(results, newSViv(face_feature.skin_bgr.v[1]));
        // av_push(results, newSViv(face_feature.skin_bgr.v[2]));
        // av_push(results, newSViv(face_feature.hair_bgr.v[0]));
        // av_push(results, newSViv(face_feature.hair_bgr.v[1]));
        // av_push(results, newSViv(face_feature.hair_bgr.v[2]));
        floatResult[r++] = face_feature.skin_bgr.v[0];
        floatResult[r++] = face_feature.skin_bgr.v[1];
        floatResult[r++] = face_feature.skin_bgr.v[2];
        floatResult[r++] = face_feature.hair_bgr.v[0];
        floatResult[r++] = face_feature.hair_bgr.v[1];
        floatResult[r++] = face_feature.hair_bgr.v[2];

        // left_eye x,y,w,h,color[4](b,g,r)
        // av_push(results, newSViv(face_pos[i].left_eye.x));
        // av_push(results, newSViv(face_pos[i].left_eye.y));
        // av_push(results, newSViv(face_pos[i].left_eye.width));
        // av_push(results, newSViv(face_pos[i].left_eye.height));
        floatResult[r++] = face_pos[i].left_eye.x;
        floatResult[r++] = face_pos[i].left_eye.y;
        floatResult[r++] = face_pos[i].left_eye.width;
        floatResult[r++] = face_pos[i].left_eye.height;

        for (j = 0; j < 4; ++j) {
            // av_push(results, newSViv(face_feature.left_eye_bgr[j].v[0]));
            // av_push(results, newSViv(face_feature.left_eye_bgr[j].v[1]));
            // av_push(results, newSViv(face_feature.left_eye_bgr[j].v[2]));
            floatResult[r++] = face_feature.left_eye_bgr[j].v[0];
            floatResult[r++] = face_feature.left_eye_bgr[j].v[1];
            floatResult[r++] = face_feature.left_eye_bgr[j].v[2];
        }

        // right_eye x,y,w,h,color[4](b,g,r)
        // av_push(results, newSViv(face_pos[i].right_eye.x));
        // av_push(results, newSViv(face_pos[i].right_eye.y));
        // av_push(results, newSViv(face_pos[i].right_eye.width));
        // av_push(results, newSViv(face_pos[i].right_eye.height));
        floatResult[r++] = face_pos[i].right_eye.x;
        floatResult[r++] = face_pos[i].right_eye.y;
        floatResult[r++] = face_pos[i].right_eye.width;
        floatResult[r++] = face_pos[i].right_eye.height;
        for (j = 0; j < 4; ++j) {
            // av_push(results, newSViv(face_feature.right_eye_bgr[j].v[0]));
            // av_push(results, newSViv(face_feature.right_eye_bgr[j].v[1]));
            // av_push(results, newSViv(face_feature.right_eye_bgr[j].v[2]));
            floatResult[r++] = face_feature.right_eye_bgr[j].v[0];
            floatResult[r++] = face_feature.right_eye_bgr[j].v[1];
            floatResult[r++] = face_feature.right_eye_bgr[j].v[2];
        }

        // nose x,y
        // av_push(results, newSViv(face_pos[i].nose.x));
        // av_push(results, newSViv(face_pos[i].nose.y));
        floatResult[r++] = face_pos[i].nose.x;
        floatResult[r++] = face_pos[i].nose.y;

        // mouth x,y,w,h
        // av_push(results, newSViv(face_pos[i].mouth.x));
        // av_push(results, newSViv(face_pos[i].mouth.y));
        // av_push(results, newSViv(face_pos[i].mouth.width));
        // av_push(results, newSViv(face_pos[i].mouth.height));
        floatResult[r++] = face_pos[i].mouth.x;
        floatResult[r++] = face_pos[i].mouth.y;
        floatResult[r++] = face_pos[i].mouth.width;
        floatResult[r++] = face_pos[i].mouth.height;

        // chin x, y
        // av_push(results, newSViv(face_pos[i].chin.x));
        // av_push(results, newSViv(face_pos[i].chin.y));
        floatResult[r++] = face_pos[i].chin.x;
        floatResult[r++] = face_pos[i].chin.y;
    }
    nv_matrix_free(&bgr);
    nv_matrix_free(&gray);
    nv_matrix_free(&smooth);
    nv_matrix_free(&edge);
    nv_matrix_free(&gray_integral);
    nv_matrix_free(&edge_integral);

    // return (*env)->NewStringUTF(env, "Hello from JNI !++");

    (*env)->ReleaseFloatArrayElements(env, result, floatResult, 0);
    return result;
}
