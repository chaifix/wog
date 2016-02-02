#include "../src/wog.h"
#include <stdio.h>
#include <Windows.h>
#include <gl/GL.h>

void init()
{
    glColor4f(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
}

void update(float dt)
{

}

void draw()
{
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    // top
    glColor3f(1.0f, 0.0f, 0.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glEnd();

    glBegin(GL_QUADS);
    // front
    glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glEnd();

    glBegin(GL_QUADS);
    // right
    glColor3f(0.0f, 0.0f, 1.0f);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    glEnd();

    glBegin(GL_QUADS);
    // left
    glColor3f(0.0f, 0.0f, 0.5f);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);

    glEnd();

    glBegin(GL_QUADS);
    // bottom
    glColor3f(0.5f, 0.0f, 0.0f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);

    glEnd();

    glBegin(GL_QUADS);
    // back
    glColor3f(0.0f, 0.5f, 0.0f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glRotatef(0.5, 0.6, 0.4, 1.0); // 2. Rotate the object.
}

int main(int argc, char** argv)
{
    wog_Window* wnd = wog_createWindow("test", 300, 300, 0, 0, 0);
    wog_GLContext* ctx = wog_createGLContext(wnd);
    wog_makeCurrent(wnd, ctx); 
    int running = 1;
    void init();
    while (running)
    {
        wog_Event e; 
        while (wog_pollEvent(wnd, &e))
        {
            if (e.type == WOG_EKEYDOWN)
            {
                printf("%d", e.key);
            }
            if (e.type == WOG_EQUIT)
            {
                running = 0; 
            }
            if (e.type == WOG_EMOUSEMOTION)
            {
                printf("%d .. %d\n", e.pos.x, e.pos.y);
            }
        }

        update(1/ 60.f); 

        glClearColor(0, 0, 0, 1); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();

        wog_swapBuffers(wnd);
        Sleep(16);  // 60fps
    }

    wog_destroyGLContext(ctx);
    wog_destroyWindow(wnd);

    return 0;
}