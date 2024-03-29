#include <iostream>
#include <limits>

#include "draw.h"
#include "geometry.h"
#include "model.h"
#include "timage.h"

const TColour red = {255, 0, 0};
const TColour green = {0, 255, 0};
const TColour white = {255, 255, 255};
const TColour black = {0, 0, 0};
const TColour cream = {240, 226, 182};

void draw_wireframe(const int width, const int height, const Model &model) {
    TImage image(width, height);

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Vec3f v1 = model.m_faces.at(i)[0];
        const Vec3f v2 = model.m_faces.at(i)[1];
        const Vec3f v3 = model.m_faces.at(i)[2];

        const int hw = width / 2;
        const int hh = height / 2;

        const Vec2f mapped_v1({(v1[0] * hw) + hw, (v1[1] * hh) + hh});
        const Vec2f mapped_v2({(v2[0] * hw) + hw, (v2[1] * hh) + hh});
        const Vec2f mapped_v3({(v3[0] * hw) + hw, (v3[1] * hh) + hh});

        const TColour white(255, 255, 255);

        line2d(mapped_v1, mapped_v2, image, white);
        line2d(mapped_v2, mapped_v3, image, white);
        line2d(mapped_v3, mapped_v1, image, white);
    }

    image.write("./out/wireframe.tga");
}

void draw_red_flat_triangles(const int width, const int height, const Model &model) {
    TImage image(width, height);

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Vec3f v1 = model.m_faces.at(i)[0];
        const Vec3f v2 = model.m_faces.at(i)[1];
        const Vec3f v3 = model.m_faces.at(i)[2];

        const int hw = width / 2;
        const int hh = height / 2;

        const Vec2f mapped_v1({(v1[0] * hw) + hw, (v1[1] * hh) + hh});
        const Vec2f mapped_v2({(v2[0] * hw) + hw, (v2[1] * hh) + hh});
        const Vec2f mapped_v3({(v3[0] * hw) + hw, (v3[1] * hh) + hh});

        flat_triangle(mapped_v1, mapped_v2, mapped_v3, image, TColour((int)((mapped_v1[1] / height) * 255), 0, 0));
    }

    image.write("./out/flat_red.tga");
}

void draw_normal_intensity_mapped_triangles(const int width, const int height, Model &model) {
    TImage image(width, height);
    image.setColour(black);

    Vec3f lighting_dir({0.0, 0.0, 1.0});
    lighting_dir = lighting_dir.norm();

    float *const zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = -std::numeric_limits<float>::max();

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Vec3f v1 = model.m_faces.at(i)[0];
        const Vec3f v2 = model.m_faces.at(i)[1];
        const Vec3f v3 = model.m_faces.at(i)[2];

        // Light Intensity Computations
        const Vec3f v1v2 = v2 - v1;
        const Vec3f v1v3 = v3 - v1;
        Vec3f normal = v1v2.cross(v1v3);
        normal = normal.norm();

        const float light_intensity = normal.dot(lighting_dir);

        if (light_intensity > 0) {
            const int light_level = (int)(light_intensity * 255);

            // Rendering
            const int hw = width / 2;
            const int hh = height / 2;

            const Vec3f mapped_v1({(v1[0] * hw) + hw, (v1[1] * hh) + hh, v1[2]});
            const Vec3f mapped_v2({(v2[0] * hw) + hw, (v2[1] * hh) + hh, v2[2]});
            const Vec3f mapped_v3({(v3[0] * hw) + hw, (v3[1] * hh) + hh, v3[2]});

            flat_triangle3(mapped_v1, mapped_v2, mapped_v3, image, TColour(light_level, light_level, light_level), zbuffer);
        }
    }

    free(zbuffer);
    image.write("./out/intensity_mapped.tga");
}

void draw_textured_model_with_camera(const int width, const int height, Model &model, Mat44f &camera_model) {
    TImage image(width, height);
    image.setColour(black);

    Vec3f lighting_dir({0.0, 0.0, 1.0});
    lighting_dir = lighting_dir.norm();

    float *zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = -std::numeric_limits<float>::max();

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Face face = model.m_faces.at(i);
        const TextureFace t_face = model.m_texture_faces.at(face.m_texture_face_id);

        const Vec3f v1 = to_cartesian<4>(camera_model.dot(to_homogeneous<3>(face[0])));
        const Vec3f v2 = to_cartesian<4>(camera_model.dot(to_homogeneous<3>(face[1])));
        const Vec3f v3 = to_cartesian<4>(camera_model.dot(to_homogeneous<3>(face[2])));

        // Light Intensity Computations
        const Vec3f v1v2 = v2 - v1;
        const Vec3f v1v3 = v3 - v1;
        Vec3f normal = v1v2.cross(v1v3);
        normal = normal.norm();

        const float light_intensity = normal.dot(lighting_dir);

        if (light_intensity > 0) {
            // Rendering
            const int hw = width / 2;
            const int hh = height / 2;

            const Vec3f mapped_v1({(v1[0] * hw) + hw, (v1[1] * hh) + hh, v1[2]});
            const Vec3f mapped_v2({(v2[0] * hw) + hw, (v2[1] * hh) + hh, v2[2]});
            const Vec3f mapped_v3({(v3[0] * hw) + hw, (v3[1] * hh) + hh, v3[2]});

            textured_triangle(mapped_v1, mapped_v2, mapped_v3, light_intensity, image, zbuffer, t_face, model);
        }
    }

    free(zbuffer);
    image.write("./out/texture_mapped.tga");
}

void draw_textured_model_proj(const int width, const int height, Model &model, bool is_perspective) {
    if (is_perspective) {
        Mat33f rotation_mat = rotation_z(0).mul<3, 3>(rotation_x(0).mul<3, 3>(rotation_z(0)));
        Mat44f rotation_mat_homo = to_homogeneous<3>(rotation_mat);
        Mat44f camera_model_perspective = get_projection_mat(4) + rotation_mat_homo - identity<4>();
        draw_textured_model_with_camera(width, height, model, camera_model_perspective);
    } else {
        Mat44f camera_model_ortho = identity<4>();
        draw_textured_model_with_camera(width, height, model, camera_model_ortho);
    }
}

void draw_textured_model_simple_model_mat(const int width, const int height, Model &model) {
    TImage image(width, height);
    image.setColour(black);

    Vec3f eye = {1, 1, 3};
    Vec3f center = {0, 0, 0};
    Vec3f up = {0, 1, 0};
    Mat44f projection_mat = get_projection_mat(12);
    Mat44f viewport_mat = get_viewport_mat(0, 0, width, height, 255);
    Mat44f view_mat = get_view_mat(eye, center, up);
    Mat44f total_screen_mat = viewport_mat.mul<4, 4>(projection_mat.mul<4, 4>(view_mat));

    Vec3f lighting_dir({0.0, 0.0, 1.0});
    lighting_dir = lighting_dir.norm();

    float *zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = -std::numeric_limits<float>::max();

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Face face = model.m_faces.at(i);
        const TextureFace t_face = model.m_texture_faces.at(face.m_texture_face_id);

        const Vec3f v1 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[0])));
        const Vec3f v2 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[1])));
        const Vec3f v3 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[2])));

        // Light Intensity Computations
        const Vec3f v1v2 = v2 - v1;
        const Vec3f v1v3 = v3 - v1;
        Vec3f normal = v1v2.cross(v1v3);
        normal = normal.norm();

        const float light_intensity = normal.dot(lighting_dir);

        if (light_intensity > 0)
            // Rendering
            textured_triangle(v1, v2, v3, light_intensity, image, zbuffer, t_face, model);
    }

    free(zbuffer);
    image.write("./out/texture_mapped_with_camera.tga");
}

void draw_textured_model_interpolated_normals(const int width, const int height, Model &model) {
    TImage image(width, height);
    image.setColour(black);

    Vec3f eye = {1, 1, 3};
    Vec3f center = {0, 0, 0};
    Vec3f up = {0, 1, 0};
    Mat44f projection_mat = get_projection_mat(12);
    Mat44f viewport_mat = get_viewport_mat(0, 0, width, height, 255);
    Mat44f view_mat = get_view_mat(eye, center, up);
    Mat44f total_screen_mat = viewport_mat.mul<4, 4>(projection_mat.mul<4, 4>(view_mat));

    Mat44f MinvT;
    inverse_4d(total_screen_mat, MinvT);
    MinvT = MinvT.transposed<4, 4>();

    Vec3f lighting_dir({0.0, 0.0, 1.0});
    lighting_dir = lighting_dir.norm();

    float *zbuffer = new float[width * height];
    for (int i = 0; i < width * height; i++)
        zbuffer[i] = -std::numeric_limits<float>::max();

    for (int i = 0; i < model.m_faces.size(); i++) {
        const Face face = model.m_faces.at(i);
        const TextureFace t_face = model.m_texture_faces.at(face.m_texture_face_id);
        const NormalFace n_face = model.m_normal_faces.at(face.m_normal_face_id);

        const Vec3f v1 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[0])));
        const Vec3f v2 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[1])));
        const Vec3f v3 = to_cartesian<4>(total_screen_mat.dot(to_homogeneous<3>(face[2])));

        const Vec3f n1 = to_cartesian<4>(MinvT.dot(to_homogeneous<3>(n_face[0]))).norm();
        const Vec3f n2 = to_cartesian<4>(MinvT.dot(to_homogeneous<3>(n_face[1]))).norm();
        const Vec3f n3 = to_cartesian<4>(MinvT.dot(to_homogeneous<3>(n_face[2]))).norm();

        // Light Intensity Computations
        // const Vec3f v1v2 = v2 - v1;
        // const Vec3f v1v3 = v3 - v1;
        // Vec3f normal = v1v2.cross(v1v3);
        // normal = normal.norm();

        // const float light_intensity = normal.dot(lighting_dir);

        Mat33f normals;
        for (int j = 0; j < 3; j++) {
            normals.set(n1[j], j, 0);
            normals.set(n2[j], j, 1);
            normals.set(n3[j], j, 2);
        }
        int_normal_triangle(v1, v2, v3, normals, lighting_dir, image, zbuffer, t_face, model);
    }

    free(zbuffer);
    image.write("./out/texture_mapped_with_interpolated_normals.tga");
}

int main() {
    Model m;
    // m.loadModel("../models/african_head.obj", true, true);
    // m.loadTextures("../models/african_head_diffuse.tga");
    m.loadModel("../models/diablo3_pose.obj", true, true);
    m.loadTextures("../models/diablo3_pose_diffuse.tga");

    draw_textured_model_interpolated_normals(1000, 1000, m);
    draw_textured_model_simple_model_mat(1000, 1000, m);
    // draw_textured_model_proj(1000, 1000, m, true);
    // draw_red_flat_triangles(1000, 1000, m);
    // draw_normal_intensity_mapped_triangles(1000, 1000, m);
    // draw_wireframe(1000, 1000, m);
}
