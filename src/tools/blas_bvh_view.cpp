#include "core/narukami.h"
#include "core/mesh.h"
#include "core/meshloader.h"
#include "core/primitive.h"
#include "core/accelerator.h"
#include "core/geometry.h"
#include <GLFW/glfw3.h>

using namespace narukami;
shared<MeshBLAS> get_blas()
{
    std::vector<std::shared_ptr<TriangleMesh>> meshs;
    {
        auto transform = make_shared(identity());
        auto inv_transform = transform;
        meshs = load_mesh_triangles_from_obj(transform, inv_transform, "bunny.obj", ".");
    }
    auto primitives = create_mesh_primitives(meshs);
    return shared<MeshBLAS>(new MeshBLAS(primitives));
}

void draw_mesh(const shared<MeshPrimitive> &p)
{
    auto v0 = (*p->mesh())[0];
    auto v1 = (*p->mesh())[1];
    auto v2 = (*p->mesh())[2];

    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}

void draw_bounds(const Bounds3f &bounds, float r, float g, float b)
{
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    Point3f mi = bounds.min_point;
    Point3f ma = bounds.max_point;

    Point3f m000 = mi;
    Point3f m001 = Point3f(mi.x, mi.y, ma.z);
    Point3f m010 = Point3f(mi.x, ma.y, mi.z);
    Point3f m011 = Point3f(mi.x, ma.y, ma.z);
    Point3f m100 = Point3f(ma.x, mi.y, mi.z);
    Point3f m101 = Point3f(ma.x, mi.y, ma.z);
    Point3f m110 = Point3f(ma.x, ma.y, mi.z);
    Point3f m111 = ma;

    glVertex3f(m000.x, m000.y, m000.z);
        glVertex3f(m001.x, m001.y, m001.z);

        glVertex3f(m010.x, m010.y, m010.z);
        glVertex3f(m011.x, m011.y, m011.z);

        glVertex3f(m100.x, m100.y, m100.z);
        glVertex3f(m101.x, m101.y, m101.z);

        glVertex3f(m110.x, m110.y, m110.z);
        glVertex3f(m111.x, m111.y, m111.z);

        //////////////////////////////////
        glVertex3f(m000.x, m000.y, m000.z);
        glVertex3f(m010.x, m010.y, m010.z);

        glVertex3f(m101.x, m101.y, m101.z);
        glVertex3f(m111.x, m111.y, m111.z);

        glVertex3f(m110.x, m110.y, m110.z);
        glVertex3f(m100.x, m100.y, m100.z);

        glVertex3f(m001.x, m001.y, m001.z);
        glVertex3f(m011.x, m011.y, m011.z);

        //////////////////////////////////
        glVertex3f(m000.x, m000.y, m000.z);
        glVertex3f(m100.x, m100.y, m100.z);

        glVertex3f(m011.x, m011.y, m011.z);
        glVertex3f(m111.x, m111.y, m111.z);

        glVertex3f(m110.x, m110.y, m110.z);
        glVertex3f(m010.x, m010.y, m010.z);

        glVertex3f(m001.x, m001.y, m001.z);
        glVertex3f(m101.x, m101.y, m101.z);
    glEnd();
}

void draw_bounds(const QBVHNode &node, float r, float g, float b)
{
    Bounds3f array[4];
    store(node.bounds, array);
    for (int i = 0; i < 4; ++i)
    {
       draw_bounds(array[i],r,g,b);
    }

}

static int depth = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        depth++;
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && depth > 0)
    {
        depth--;
    }
}

const static int WIDTH = 512;
const static int HEIGHT = 512;

int main(int argc, char *argv[])
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "MeshBLAS BVH Viewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    shared<MeshBLAS> blas = get_blas();
    auto bounds = blas->bounds();
    auto primitives = blas->get_mesh_primitives();

    glfwSetKeyCallback(window, key_callback);

    int cache_depth = 0;
    auto nodes = blas->get_nodes_by_depth(cache_depth);
    glEnable(GL_DEPTH_TEST);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height; //窗口的宽高比

        float max_extent_x = bounds.max_point.x - bounds.min_point.x;
        float max_extent_y = bounds.max_point.y - bounds.min_point.y;

        float look_x = (bounds.max_point.x + bounds.min_point.x)/2;
        float look_y = (bounds.max_point.y + bounds.min_point.y)/2;
        float look_z = (bounds.max_point.z + bounds.min_point.z)/2;

        float horizontal0 = 0;
        float horizontal1 = 0;
        float vertical0 = 0;
        float vertical1 = 0;

        float zNear = 0;
        float zFar = 0;

        if (max_extent_x >= max_extent_y)
        {
            horizontal0 = look_x - max_extent_x/2;
            horizontal1 = look_x + max_extent_x/2;
            vertical0 =   look_y - max_extent_x / ratio / 2;
            vertical1 =   look_y + max_extent_x / ratio / 2;

            zNear = max_extent_x / 2 / narukami::tan(deg2rad(30));
            zFar = bounds.max_point.z - bounds.min_point.z + zNear;
        }
        else
        {
            vertical0 = look_y - max_extent_y/2;
            vertical1 = look_y + max_extent_y/2;
            horizontal0 = look_x - max_extent_y * ratio / 2;
            horizontal1 = look_x + max_extent_y * ratio / 2;
            zNear = max_extent_y / 2 / narukami::tan(deg2rad(30));
            zFar = bounds.max_point.z - bounds.min_point.z + zNear;
        }

        glViewport(0, 0, width, height);

        glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(                    // 设置投影矩阵为透视投影:
            horizontal0, horizontal1, //   - 透视视景体的左右边位置
            vertical0, vertical1,     //   - 透视视景体的上下边位置
            zNear, zFar+zNear);             //   - 透视视景体的前后边位置

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        auto transform = look_at(0.0f, 0.0f, bounds.min_point.z - zNear, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);//look_at(look_x, look_y, bounds.min_point.z - zNear, look_x, look_y, look_z, 0.0f, 1.0f, 0.0f);
        float matrix[16]; // = {1,0,0,0,
        //                     0,1,0,0,
        //                     0,0,1,0,
        //                     0,0,-0.5f,1};
        store(transform.mat, matrix);
        glLoadMatrixf(matrix);

        if (cache_depth != depth)
        {
            cache_depth = depth;
            nodes = blas->get_nodes_by_depth(cache_depth);
            if (nodes.size() == 0)
            {
                cache_depth = depth = 0;
                nodes = blas->get_nodes_by_depth(cache_depth);
            }
        }

        draw_bounds(bounds, 0, 1, 0);
         for (auto &node : nodes)
        {
            draw_bounds(node,1,0,0);
        }
        for (auto &p : primitives)
        {
            draw_mesh(p);
        }

        char  buffer[200];
        sprintf(buffer,"MeshBLAS BVH Viewer : primitive %d depth %d",primitives.size(),depth);
        glfwSetWindowTitle(window,buffer);
       

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}