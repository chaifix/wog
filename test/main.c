/**
* Copyright (c) 2015~2017 chai(neonum)
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the MIT license. See LICENSE for details.
*/
#include "../src/wog.h"
#include <stdio.h>
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

void onSizeChanged(wog_Window* wnd)
{
    printf("size changed\n");
}

int main(int argc, char** argv)
{
    wog_Window* wnd = wog_createWindow("wog test", 300, 300, 400, 400, WOG_WND_RESIZABLE);
    wog_GLContext* ctx = wog_createGLContext(wnd);
    wog_makeCurrent(wnd, ctx); 
    int w, h; 
    wog_getwindowsize(wnd, &w, &h);
    wog_registerResizeCallback(onSizeChanged);
    int running = 1;
    void init();
    while (running)
    {
        wog_Event e; 
        while (wog_pollEvent(wnd, &e))
        {
            if (e.type == WOG_EKEYDOWN)
            {
                printf("key is pressed: %d\n", e.key);
                if (e.key == 'A')
                {
                    printf("A key is down\n");
                }
            }
            if (e.type == WOG_ECLOSE)
            {
                running = 0;
                break;
            }
            if (e.type == WOG_EMOUSEMOTION)
            {
                printf("mouse moved: (%d , %d) \n", e.pos.x, e.pos.y);
            }
            if (e.type == WOG_EMOUSEWHEEL)
            {
                printf("mouse wheel scrolled: %d\n", e.wheel);
            }
            if (e.type == WOG_EMOUSEBUTTONDOWN)
            {
                char* bnt = 0 ;
                switch(e.button)
                {
                case WOG_MOUSE_LBUTTON: bnt = "left"; break;
                case WOG_MOUSE_RBUTTON: bnt = "right"; break;
                case WOG_MOUSE_MIDDLE: bnt = "middle"; break;
                }
                printf("%s mouse button is pressed\n", bnt);
            }
        }
        if (!running) break;

        update(1/ 60.f); 

        glClearColor(0, 0, 0, 1); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw();

        wog_swapBuffers(wnd);
        wog_sleep(16);  // 60fps
    }

    wog_destroyWindow(wnd);
    wog_destroyGLContext(ctx);
    wog_quit(); 

    return 0;
}