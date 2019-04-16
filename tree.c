#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h> // this might be alloca.h in linux

// TODO ONLY DO IF IN WINDOWS
#ifndef __linux__
#include <GL/glew.h>
#include <GL/wglew.h>
#endif




#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pcg/pcg_basic.h"
#include "pcg/pcg_basic.c"
#include "cglm/cglm.h"

// rừng ngập mặn :D


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifndef M_PI
#define M_PI 3.14159265
#endif




static void fileToString( char** dstStr, FILE* file )
{
    long length;
    if( file )
    {
        fseek( file, 0, SEEK_END );
        length = ftell( file );
        fseek( file, 0, SEEK_SET );
        *dstStr = malloc( length + 1 );
        if( *dstStr ) fread( *dstStr, 1, length, file );
        fclose( file );
        (*dstStr)[ length ] = '\0';
    }
    if( *dstStr == 0 )
    {
        printf( "ERROR LOADING FILE TO STRING\n" );
    }
}

int timems, oldtimems;
float dt; // in seconds
void updateTime()
{
    timems = glutGet( GLUT_ELAPSED_TIME );
    dt = ( timems - oldtimems ) / 1000.;
    oldtimems = timems;
}






// ---

static GLint win = 0;

bool keys[128];

int globalwidth, globalheight;
int mousex, mousey = 450;

bool firstpersonmode = false;


vec3 camPos = (vec3){ 0, -1, -8 };
float camYaw = 10, camPitch = 0.174532925; // glm_rad( 10. ); fuck c lol

float yawSpinRate = -.5;






unsigned int woodTexture, leafTexture; // needed in tree renderer section





// from tree renderer
// static GLfloat view_rotx = 275.0,view_roty = 0.0,view_rotz = 0.0;
    //static double pdx = 0, pdy = 0, pdz = 3;
    static double angleLower = 5, angleHigher = 35, thicc = 2., size = 0.85;
    static double crazy = 45;
    static int cylinderResolution = 5, branchMin = 4, branchMax = 4, iterations = 7, maxiterations = 7;
    static int maxDepth, branchDepth = 5; // set via max cD2

    typedef struct stack{
        int maxSize;
        int size;
        float* stac;
    } Stack;

    struct stack a,b,c,d,e,f,g, vertices, triangles;


    //vXnX + vYnY + vZnZ = 0
    //nX = nY*(vY/vX)+nZ(vZ/vX)
    //nY = nX*(vX/vY)+nZ(vZ/vY)
    //nX^2 + nY^2 + nZ^2 = 1

    //

    //1/rt3(nX+nY+nZ) = 0


    // vertexArr = {x, y, z, nX, nY, nZ, tX, tY, t, depth...}
    // triArr = {v1, v2, v3...}


    // of a given triangle
    // static void normal2(float* normal, float a, float b, float c, float d, float e, float f, float g, float h, float i){
    //     float Vx = d-a, Vy = e-b, Vz = f-c, Wx = g-a, Wy = h-b, Wz = i-c;
    //     float Nx = (Vy*Wz)-(Vz*Wy), Ny = (Vz*Wx)-(Vx*Wz), Nz = (Vx*Wy)-(Vy*Wx);
    //     float M = abs(Nx)+abs(Ny)+abs(Nz);
    //     // glNormal3f(Nx/M,Ny/M,Nz/M);
    //     normal[0] = Nx/M;
    //     normal[1] = Ny/M;
    //     normal[2] = Nz/M;
    // }

    static void push(struct stack* st, float elem){
        if(st->size == st->maxSize){
            st->maxSize *= 2;
            float* newStack = realloc(st->stac,(st->maxSize)*sizeof(float)+1);
            if(newStack) st->stac = newStack;
            else {
                printf("failed to push. Trying again\n");
                st->maxSize /= 2;
                push(st, elem);
                return;
            }
            // printf("Size increased to %d\n", st->maxSize);
        }
        st->stac[st->size] = elem;
        st->size += 1;
    }

    static float pop(struct stack* st){
        float ret = st->stac[st->size-1];
        st->size--;
        return ret;
    }

    static void fakeFree(struct stack* st){
        while (st->size > 0) pop(st);
    }

    static char* helper(char* str, int recursions){
        if(recursions == 0) return str;
        int len = 0;
        pcg32_random_t myrng;
        pcg32_srandom_r(&myrng, time(NULL), (intptr_t)&myrng);
        // printf("%s\n", str);
        for (int i = 0; i < strlen(str); i++){
            char c = str[i];
            // if(c == '[' || c == '_' || c == ']') len += 1;
            if(c == '[' || c == '_' || c == ']' || c == '1') len += 1;
            // if(c == '[' || c == '_' || c == ']' || c == '1' || c == '2') len += 1;
            else if (c == '0') len += 5+2*(branchMax-1);
            // else if (c == '3') len += 2;
            else if (c == '2') len += 2;
        }
        // printf( "len %d\n", len );
        char* ret = malloc(len*sizeof(char) + 1);
        int i = 0;
        for(int j = 0; j < strlen(str); j++, i++){
            // printf("eh?%d/%d\n", i, len);
            char c = str[j];
            if(c == '[' || c == '_' || c == ']') ret[i] = c;
            else if (c == '1') {
                // ret[i++] = '1';
                // ret[i] = '1';
                ret[i] = '2';
            }
            else if (c == '2') {
                ret[i++] = '1';
                ret[i] = '1';
                // ret[i] = '3';
            }
            else if (c == '0') {
                int branchNo = ((int)ldexp(pcg32_random_r(&myrng), -32)*(branchMax-branchMin+1))+branchMin;
                if(branchMax > branchNo) printf("%d %d\n", branchMax, branchNo);
                ret[i++] = '1';
                ret[i++] = '[';
                for (int k = 2; k < branchNo; k++){
                    ret[i++] = '0';
                    ret[i++] = '_';
                }
                ret[i++] = '0';
                ret[i++] = ']';
                ret[i] = '0';
            }
            else if (c == '3'){
                ret[i++] = '1';
                ret[i] = '1';
            } 
        }
        ret[i] = '\0';
        free(str);
        return helper(ret, recursions-1);
    }

    //0: branch with leaf
    //1: branch
    //[: push angles, turn left 45 degrees
    //_: pop angles, push angles, rotate 60 degrees, turn left 45 degrees
    //]: pop angles, rotate 60 degrees, turn left 45 degrees
    static char* generateString(int recursions){
        return helper("0", recursions);
    }

    static void drawFromString(char* string, float lineLen, float xPos, float yPos, float zPos){
        // printf("(%f)\n",lineLen );
        struct stack *theta = &a, *phi = &b, *x = &c, *y = &d, *z = &e, *depth = &f, *depth2 = &g;
        struct stack *vert = &vertices, *tri = &triangles;
        float cT = 0.0, cP = 0, cX = xPos, cY = yPos, cZ = zPos, angle, angle2, scale = 1;
        int cD = 1, cD2 = 1, leaf = 0;
        pcg32_random_t myrng;
        pcg32_srandom_r(&myrng, time(NULL), (intptr_t)&myrng);

        int draw = 0, branchNo = (branchMin + branchMax)/2;
        float bottomPoints[cylinderResolution*3];
        float topPoints[cylinderResolution*3];

        float twistAngle = 0.0;

        // int vertIndex;

        unsigned int tex = -1;
        // unsigned int prevTex = -1;

        fakeFree(vert);
        fakeFree(tri);

        // glRotatef(view_rotx,1.0,0.0,0.0);
        // glRotatef(view_roty,0.0,1.0,0.0);
        // glRotatef(view_rotz,0.0,0.0,1.0);
        // printf("%d\n", view_rotx);
        
        // glBegin(GL_LINES);
        // printf("%s\n", string);
        for(int i = 0; i < strlen(string); i++){
            draw = 0;
            // double rand = ldexp(pcg32_random_r(&myrng), -32);
            switch(string[i]){
            case '[':
                angle = (float)ldexp(pcg32_random_r(&myrng), -32)*(angleHigher-angleLower) + angleLower;
                // printf("12, %f\n", rand);
                // printf("pushing to phi, theta, x, y, z, depth\n");
                push(theta, cT);
                push(phi, cP);
                push(x, cX);
                push(y, cY);
                push(z, cZ);
                push(depth, cD);
                push(depth2, cD2);
                cD += 1;
                cD2++;
                cP += angle/2;
                // glRotatef(cT, 0.0, 0.0, 1.0);
                break;
            case '_':
                angle = (float)ldexp(pcg32_random_r(&myrng), -32)*(angleHigher-angleLower) + angleLower;
                angle2 = (float)ldexp(pcg32_random_r(&myrng), -32)*(crazy) - crazy/2.0;
                // printf("11\n");
                cP = pop(phi) + angle/2;
                cT = pop(theta) + (360.0/((float)branchNo) + angle2)/2.0;
                cX = pop(x);
                cY = pop(y);
                cZ = pop(z);
                cD = pop(depth);
                cD2 = pop(depth2);
                // printf("pushing to phi, theta, x, y, z, depth\n");
                push(phi, cP-angle/2);
                push(theta, cT-(360.0/((float)branchNo) + angle2)/2.0);
                push(x, cX);
                push(y, cY);
                push(z, cZ);
                push(depth, cD);
                push(depth2, cD2);
                cD += 1;
                cD2++;
                // glRotatef(cT, 0.0, 0.0, 1.0);
                // glRotatef(cP, 0.0, 1.0, 0.0);
                break;
            case ']':
                angle = (float)ldexp(pcg32_random_r(&myrng), -32)*(angleHigher-angleLower) + angleLower;
                angle2 = (float)ldexp(pcg32_random_r(&myrng), -32)*(crazy) - crazy/2.0;
                // printf("10\n");
                cP = pop(phi) + angle/2;
                cT = pop(theta) + (360.0/((float)branchNo)+ angle2);
                cX = pop(x);
                cY = pop(y);
                cZ = pop(z);
                cD = pop(depth);
                cD2 = pop(depth2);
                cD += 1;
                cD2++;
                // glRotatef(cT, 0.0, 0.0, 1.0);
                // glRotatef(cP, 0.0, 1.0, 0.0);
                break;
            case '0':
                leaf = 1;
                scale = 1;
                draw = 1;
                cD2++;
                break;
            case '1':
                leaf = 0;
                scale = 1;
                draw = 1;
                cD2++;
                break;
            case '2':
                leaf = 0;
                scale = 1.33;
                draw = 1;
                cD2++;
                break;
            case '3':
                leaf = 0;
                scale = 1.67;
                draw = 1;
                cD2++;
                break;
            }
            if(draw){
                // glBegin( GL_LINES );
                // // printf("9\n");
                // glColor3f( 1., 1., 1. );
                // glVertex3f(cX, cY, cZ);
                float cosP = cos(cP/180*2*M_PI);
                float sinP = sin(cP/180*2*M_PI);
                float sinT = sin(cT/180*2*M_PI);
                float cosT = cos(cT/180*2*M_PI);
                // printf("a\n");

                // vertIndex = vert->size;

                vec3 n = {cosT*sinP*scale, sinT*sinP*scale, cosP*scale};
                vec3 p;
                glm_vec_ortho(n, p);
                glm_vec_normalize(p);

                float thicc2 = thicc/((float)cD2), thicc3 = thicc/((float)(cD2+1));
                
                glm_vec_rotate(p, twistAngle, n);

                for(int i = 0; i < cylinderResolution; i++){
                    glm_vec_rotate(p, 2.0*M_PI/((float)cylinderResolution), n);
                    // printf("%f\n", 2*M_PI/((float)cylinderResolution)*(float)i);
                    // printf("%d\n", cD);
                    bottomPoints[ i*3 ] = cX + (thicc2)*p[0];
                    bottomPoints[i*3+1] = cY + (thicc2)*p[1];
                    bottomPoints[i*3+2] = cZ + (thicc2)*p[2];
                    // float cosT2 = cos(2*M_PI/cylinderResolution*i);
                    // float sinT2 = sin(2*M_PI/cylinderResolution*i);
                    // bottomPoints[i*3] = cX + (thicc/2.0)*cosP*sinT*cosT2;
                    // bottomPoints[i*3+1] = cY + (thicc/2.0)*cosP*cosT*sinT2;
                    // bottomPoints[i*3+2] = cZ - (thicc/2.0)*sinP*cosT2;

                }
                // printf("b\n");

                //
                // printf("(%f, %f, %f),\n", cX, cY, cZ);
                // for(int i = 0; i < cylinderResolution; i++){
                //  printf("(%f, %f, %f), ", bottomPoints[i*3],bottomPoints[i*3+1],bottomPoints[i*3+2]);
                // }
                // printf("\n");

                // printf("(%f, %f, %f),\n", lineLen*cosT*sinP*scale, lineLen*sinT*sinP*scale, lineLen*cosP*scale);
                // for(int i = 0; i < cylinderResolution; i++){
                //  printf("(%f, %f, %f), ", bottomPoints[i*3]-cX,bottomPoints[i*3+1]-cY,bottomPoints[i*3+2]-cZ);
                // }
                // printf("\n");
                //

                cX += lineLen*n[0];
                cY += lineLen*n[1];
                cZ += lineLen*n[2];


                // glVertex3f(cX, cY, cZ);
                // glEnd();

                glm_vec_rotate(p, M_PI/((float)cylinderResolution), n);
                twistAngle += M_PI/((float)cylinderResolution);

                for(int i = 0; i < cylinderResolution; i++){
                    glm_vec_rotate(p, 2.0*M_PI/((float)cylinderResolution), n);
                    topPoints[ i*3 ] = cX + (thicc3)*p[0];
                    topPoints[i*3+1] = cY + (thicc3)*p[1];
                    topPoints[i*3+2] = cZ + (thicc3)*p[2];
                }
                // for(int i = 0; i < cylinderResolution; i++){
                //  float cosT2 = cos(2*M_PI/cylinderResolution*i);
                //  bottomPoints[i*3] = cX + sqrt(thicc/2)*cosP*sinT*cosT2;
                //  bottomPoints[i*3+1] = cY + sqrt(thicc/2)*cosP*cosT*cosT2;
                //  bottomPoints[i*3+2] = cZ - sqrt(thicc/2)*sinP*cosT2;
                // }
                // printf("d\n");
                // glBegin(GL_POINTS);
                // glVertex3f(bottomPoints[0], bottomPoints[1], bottomPoints[2]);
                // glEnd();
                if(leaf) {
                    glColor3f(1.,1.,1.);
                    // glBindTexture(GL_TEXTURE_2D, leafTexture);
                    tex = leafTexture;
                    // else tex = -1;
                    // prevTex = leafTexture;
                }
                else {
                    // printf("setting wood texture: %d (not %d)\n", woodTexture, leafTexture);
                    glColor3f(1.,1.,1.);
                    // glBindTexture(GL_TEXTURE_2D, woodTexture);
                    tex = woodTexture;
                    // else tex = -1;
                    // prevTex = woodTexture;
                }


// tex = 2;

maxDepth = fmax( maxDepth, cD2 );

int botStart = vert->size / 10;
for( int i = 0; i < cylinderResolution; i++ )
{
    vec3 a1 = {bottomPoints[3*((i+0)%cylinderResolution)+0], // two triangles to find average normal of
               bottomPoints[3*((i+0)%cylinderResolution)+1],
               bottomPoints[3*((i+0)%cylinderResolution)+2]};

    vec3 a2 = {bottomPoints[3*((i+1)%cylinderResolution)+0],
               bottomPoints[3*((i+1)%cylinderResolution)+1],
               bottomPoints[3*((i+1)%cylinderResolution)+2]};

    vec3 a3 = {topPoints   [3*((i+0)%cylinderResolution)+0],
               topPoints   [3*((i+0)%cylinderResolution)+1],
               topPoints   [3*((i+0)%cylinderResolution)+2]};

    vec3 ad1, ad2;
    glm_vec_sub( a2, a1, ad1 );
    glm_vec_sub( a3, a1, ad2 );

    vec3 normal;
    glm_vec_cross( ad1, ad2, normal );
    glm_vec_normalize( normal );




    push( vert, bottomPoints[3*i] ); // push position
    push( vert, bottomPoints[3*i+1] );
    push( vert, bottomPoints[3*i+2] );

    push( vert, normal[0] ); // push normal
    push( vert, normal[1] );
    push( vert, normal[2] );
    // printf( "%f %f %f\n", normal[0], normal[1], normal[2] );

    push( vert, i / (float)cylinderResolution ); // texture coords
    push( vert, 0. );

// if( tex == leafTexture ) printf( "wat %d\n", cD );
    push( vert, (float)tex ); // IDK WHICH TEXTURE

    push( vert, cD2 );
}
int topStart = vert->size / 10;
for( int i = 0; i < cylinderResolution; i++ )
{
    vec3 a1 = {topPoints[3*((i+cylinderResolution-0)%cylinderResolution)+0], // two triangles to find average normal of
               topPoints[3*((i+cylinderResolution-0)%cylinderResolution)+1],
               topPoints[3*((i+cylinderResolution-0)%cylinderResolution)+2]};

    vec3 a2 = {bottomPoints[3*((i+1)%cylinderResolution)+0],
               bottomPoints[3*((i+1)%cylinderResolution)+1],
               bottomPoints[3*((i+1)%cylinderResolution)+2]};

    vec3 a3 = {topPoints[3*((i+1)%cylinderResolution)+0],
               topPoints[3*((i+1)%cylinderResolution)+1],
               topPoints[3*((i+1)%cylinderResolution)+2]};

    vec3 ad1, ad2;
    glm_vec_sub( a2, a1, ad1 );
    glm_vec_sub( a3, a1, ad2 );

    vec3 normal;
    glm_vec_cross( ad1, ad2, normal );
    glm_vec_normalize( normal );


    push( vert, topPoints[3*i] ); // push position
    push( vert, topPoints[3*i+1] );
    push( vert, topPoints[3*i+2] );

    push( vert, normal[0] ); // push normal
    push( vert, normal[1] );
    push( vert, normal[2] );

    push( vert, i / (float)cylinderResolution ); // texture coords
    push( vert, 1. );

    push( vert, (float)tex ); // IDK WHICH TEXTURE

    push( vert, cD2 );
}
for( int i = 0; i < cylinderResolution; i++ )
{
    push( tri, botStart + ((i+0)%cylinderResolution) );
    push( tri, botStart + ((i+1)%cylinderResolution) );
    push( tri, topStart + ((i+1)%cylinderResolution) );

    push( tri, botStart + ((i+1)%cylinderResolution) );
    push( tri, topStart + ((i+2)%cylinderResolution) );
    push( tri, topStart + ((i+1)%cylinderResolution) );
}


/*
                glBegin(GL_TRIANGLES);
                // glColor3f(0.,1.,0.2);
                float prevNormal[3] = { 0., 0., 0. }; // to remove warning, doesn't matter cause it is actually initialized before use
                float smol = 1.0;
                for(int i = 1; i < cylinderResolution+1; i++){
                    // vertexArr = {x, y, z, nX, nY, nZ, tX, tY, t, cD...}
                    // triArr = {v1, v2, v3...}
                    float normal[3];
                    normal2(normal, bottomPoints[3*(i-1)],bottomPoints[3*(i-1)+1],bottomPoints[3*(i-1)+2], topPoints[3*(i-1)],topPoints[3*(i-1)+1],topPoints[3*(i-1)+2], bottomPoints[3*(i%cylinderResolution)],bottomPoints[3*(i%cylinderResolution)+1],bottomPoints[3*(i%cylinderResolution)+2]);
                    if(i < cylinderResolution){
                        float avgNorm[3] = {
                            i>1?(normal[0]+prevNormal[0])/2:normal[0],
                            i>1?(normal[1]+prevNormal[1])/2:normal[1],
                            i>1?(normal[2]+prevNormal[2])/2:normal[2]};
                        prevNormal[0] = normal[0];
                        prevNormal[1] = normal[1];
                        prevNormal[2] = normal[2];
                        // printf("pushing to vert\n");
                        push(vert, bottomPoints[3*(i-1)]);
                        push(vert, bottomPoints[3*(i-1)+1]);
                        push(vert, bottomPoints[3*(i-1)+2]);
                        push(vert, avgNorm[0]);
                        push(vert, avgNorm[1]);
                        push(vert, avgNorm[2]);
                        push(vert, 1.0/((float)cylinderResolution)*(i-1));
                        push(vert, 0);
                        push(vert, tex);
                        push(vert, cD);

                        push(vert, topPoints[3*(i-1)]);
                        push(vert, topPoints[3*(i-1)+1]);
                        push(vert, topPoints[3*(i-1)+2]);
                        push(vert, avgNorm[0]);
                        push(vert, avgNorm[1]);
                        push(vert, avgNorm[2]);
                        push(vert, 1.0/((float)cylinderResolution)*(i-1));
                        push(vert, 1);
                        push(vert, tex);
                        push(vert, cD);
                    }
                    else {
                        vert->stac[vertIndex + 3] = (vert->stac[vertIndex + 3]+normal[0])/2.0;
                        vert->stac[vertIndex + 4] = (vert->stac[vertIndex + 4]+normal[1])/2.0;
                        vert->stac[vertIndex + 5] = (vert->stac[vertIndex + 5]+normal[2])/2.0;
                        vert->stac[vertIndex + 10 + 3] = (vert->stac[vertIndex + 10 + 3]+normal[0])/2.0;
                        vert->stac[vertIndex + 10 + 4] = (vert->stac[vertIndex + 10 + 4]+normal[1])/2.0;
                        vert->stac[vertIndex + 10 + 5] = (vert->stac[vertIndex + 10 + 5]+normal[2])/2.0;
                    }

                    push(tri, vertIndex/10);
                    push(tri, vertIndex/10+1);
                    push(tri, vertIndex/10+2);

                    push(tri, vertIndex/10+3);
                    push(tri, vertIndex/10+2);
                    push(tri, vertIndex/10+1);

                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 0.0/smol);
                    glVertex3f(bottomPoints[3*(i-1)],bottomPoints[3*(i-1)+1],bottomPoints[3*(i-1)+2]);
                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 1.0/smol);
                    glVertex3f(topPoints[3*(i-1)],topPoints[3*(i-1)+1],topPoints[3*(i-1)+2]);
                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 0.0/smol);
                    glVertex3f(bottomPoints[3*(i%cylinderResolution)],bottomPoints[3*(i%cylinderResolution)+1],bottomPoints[3*(i%cylinderResolution)+2]);

                    // normals(topPoints[3*(i%cylinderResolution)],topPoints[3*(i%cylinderResolution)+1],topPoints[3*(i%cylinderResolution)+2], bottomPoints[3*(i%cylinderResolution)],bottomPoints[3*(i%cylinderResolution)+1],bottomPoints[3*(i%cylinderResolution)+2], topPoints[3*(i-1)],topPoints[3*(i-1)+1],topPoints[3*(i-1)+2]);
                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 1.0/smol);
                    glVertex3f(topPoints[3*(i%cylinderResolution)],topPoints[3*(i%cylinderResolution)+1],topPoints[3*(i%cylinderResolution)+2]);
                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 0.0/smol);
                    glVertex3f(bottomPoints[3*(i%cylinderResolution)],bottomPoints[3*(i%cylinderResolution)+1],bottomPoints[3*(i%cylinderResolution)+2]);
                    glTexCoord2f(1.0/((float)cylinderResolution)*(i-1), 1.0/smol);
                    glVertex3f(topPoints[3*(i-1)],topPoints[3*(i-1)+1],topPoints[3*(i-1)+2]);
                }
                // glVertex3f(-1.373860, 0.658685, 15.238823);
                // glVertex3f(-1.475824, 0.599816, 15.304967);
                // glVertex3f(-1.399990, 0.643599, 15.255774);

                // glVertex3f(-1.300957, -0.471624, 19.257740); glVertex3f(-1.256402, -0.445900, 19.518955); glVertex3f(-1.156288, -0.388100, 20.105900);
                // glVertex3f(-1.300957, -0.471624, 19.257740); glVertex3f(-1.156288, -0.388100, 20.105900); glVertex3f(-1.076003, -0.341747, 20.576591);
                // glVertex3f(-1.300957, -0.471624, 19.257740); glVertex3f(-1.076003, -0.341747, 20.576591); glVertex3f(-1.076003, -0.341747, 20.576591);
                // glVertex3f(-1.300957, -0.471624, 19.257740); glVertex3f(-1.076003, -0.341747, 20.576591); glVertex3f(-1.156288, -0.388100, 20.105900);
                // glVertex3f(-1.300957, -0.471624, 19.257740); glVertex3f(-1.156288, -0.388100, 20.105900); glVertex3f(-1.256402, -0.445900, 19.518955);



                glEnd();
*/
                // printf("eh?\n");

                
                // printf("e\n");

                // glBegin(GL_LINES);
            }
        }
        // glEnd();
        // printf("1\n");
        fakeFree(&a);
        // printf("1\n");
        fakeFree(&b);
        // printf("2\n");
        fakeFree(&c);
        // printf("2\n");
        fakeFree(&d);
        // printf("1\n");
        fakeFree(&e);
        // printf("1\n");
        fakeFree(&f);
        fakeFree(&g);
        // if(string) free(string);
    }















// float vertices[] = { // what once was a cube is now a tree: vertices->stac
    //     -0.5f, -0.5f, -0.5f,  1., 0., 0.,  0.0f, 0.0f,
    //      0.5f, -0.5f, -0.5f,  1., 0., 0.,  1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,  1., 0., 0.,  1.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,  1., 0., 0.,  1.0f, 1.0f,
    //     -0.5f,  0.5f, -0.5f,  1., 0., 0.,  0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  1., 0., 0.,  0.0f, 0.0f,

    //     -0.5f, -0.5f,  0.5f,  0., 1., 0., 0.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,  0., 1., 0., 1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  0., 1., 0., 1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  0., 1., 0., 1.0f, 1.0f,
    //     -0.5f,  0.5f,  0.5f,  0., 1., 0., 0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,  0., 1., 0., 0.0f, 0.0f,

    //     -0.5f,  0.5f,  0.5f,  0., 0., 1., 1.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,  0., 0., 1., 1.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0., 0., 1., 0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,  0., 0., 1., 0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,  0., 0., 1., 0.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,  0., 0., 1., 1.0f, 0.0f,

    //      0.5f,  0.5f,  0.5f,  1., 0., 1.,  1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,  1., 0., 1.,  1.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  1., 0., 1.,  0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  1., 0., 1.,  0.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,  1., 0., 1.,  0.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  1., 0., 1.,  1.0f, 0.0f,

    //     -0.5f, -0.5f, -0.5f,  1., 1., 0.,  0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,  1., 1., 0.,  1.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,  1., 1., 0.,  1.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,  1., 1., 0.,  1.0f, 0.0f,
    //     -0.5f, -0.5f,  0.5f,  1., 1., 0.,  0.0f, 0.0f,
    //     -0.5f, -0.5f, -0.5f,  1., 1., 0.,  0.0f, 1.0f,

    //     -0.5f,  0.5f, -0.5f,  0., 1., 1.,  0.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,  0., 1., 1.,  1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,  0., 1., 1.,  1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,  0., 1., 1.,  1.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,  0., 1., 1.,  0.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,  0., 1., 1.,  0.0f, 1.0f
    // };
// unsigned int indices[] = { // triangles->stac
    //     0,1,2,
    //     3,4,5,

    //     6,7,8,
    //     9,10,11,

    //     12,13,14,
    //     15,16,17,

    //     18,19,20,
    //     21,22,23,

    //     24,25,26,
    //     27,28,29,

    //     30,31,32,
    //     33,34,35
    // };
unsigned int* treeindices;
unsigned int VBO, EBO, VAO;

float lightVertices[] =
{
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f, -0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
     0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f,  0.1f,  0., 0., 0.,  0., 0., -1., -1.,
    -0.1f,  0.1f, -0.1f,  0., 0., 0.,  0., 0., -1., -1.
};
unsigned int lightVBO, lightVAO;


#define mirrorwaterres 600
float mirrorVertices[ 6 * ( mirrorwaterres * mirrorwaterres + (mirrorwaterres+1) * (mirrorwaterres+1) ) ];// = { // 6 * ( res * res + (res+1)*(res+1) ) see genMirrorWaterVerts
    // -4., 0., -4.,  0.,0.,0.,
    //  4., 0., -4.,  0.,0.,0.,
    //  4., 0.,  4.,  0.,0.,0.,
    // -4., 0.,  4.,  0.,0.,0.
//};
unsigned int mirrorIndices[ 3 * ( 2 * mirrorwaterres * mirrorwaterres + 2 * mirrorwaterres * (mirrorwaterres-1) ) ];// = { // 3 * ( 2 * res * res + 2 * res * (res-1) )
    // 0, 1, 2,
    // 2, 3, 0
//};
unsigned int mirrorVBO, mirrorEBO, mirrorVAO; // reuses the texture (because it reuses the shader)
unsigned int mirrorFBO, mirrorRBO, mirrorTexture;

static void genMirrorWaterVerts()
{
    // int res = 4;
    int res2 = 2 * mirrorwaterres + 1;
    float scale = .05;

    int on = 0;
    for( int i = 0; i < res2; i++ )
    {
        for( int j = 0; j < res2; j++ )
        {
            if( j % 2 == i % 2 )
            {
                mirrorVertices[ on++ ] = (float)(i - mirrorwaterres) * scale;
                mirrorVertices[ on++ ] = 0.; // height updated in vertex shader
                mirrorVertices[ on++ ] = (float)(j - mirrorwaterres) * scale;

                mirrorVertices[ on++ ] = 0.; // maybe could change colour idk
                mirrorVertices[ on++ ] = 0.;
                mirrorVertices[ on++ ] = 0.;
            }
            // printf( "%s", j % 2 == i % 2 ? "x" : " " );
        }
        // printf( "\n" );
    }

    on = 0;
    for( int i = 0; i < mirrorwaterres; i++ )
        for( int j = 0; j < mirrorwaterres; j++ )
        {
            mirrorIndices[ on++ ] = i * res2 + j;
            mirrorIndices[ on++ ] = i * res2 + j + 1;
            mirrorIndices[ on++ ] = i * res2 + mirrorwaterres + 1 + j;

            mirrorIndices[ on++ ] = i * res2 + mirrorwaterres + 1 + j;
            mirrorIndices[ on++ ] = ( i + 1 ) * res2 + j;
            mirrorIndices[ on++ ] = ( i + 1 ) * res2 + j + 1;
        }
    for( int i = 0; i < mirrorwaterres; i++ )
        for( int j = 1; j < mirrorwaterres; j++ )
        {
            mirrorIndices[ on++ ] = i * res2 + j;
            mirrorIndices[ on++ ] = i * res2 + j + mirrorwaterres + 1;
            mirrorIndices[ on++ ] = i * res2 + j + mirrorwaterres;

            mirrorIndices[ on++ ] = ( i + 1 ) * res2 + j;
            mirrorIndices[ on++ ] = i * res2 + j + mirrorwaterres;
            mirrorIndices[ on++ ] = i * res2 + j + mirrorwaterres + 1;
        }
}




mat4 objMat, viewCamMat, projMat;
unsigned int shaderProgram;

unsigned int mirrorShaderProgram;


unsigned int cubemapTexture;
    // char* cubemapFiles[] = { "lposx.png", "lnegx.png", "lposy.png", "lnegy.png", "lposz.png", "lnegz.png" };
    // char* cubemapFiles[] = { "ame_greenhaze/greenhaze_lf.png", "ame_greenhaze/greenhaze_rt.png", "ame_greenhaze/greenhaze_dn.png", "ame_greenhaze/greenhaze_up.png", "ame_greenhaze/greenhaze_ft.png", "ame_greenhaze/greenhaze_bk.png" };
    // char* cubemapFiles[] = { "Storforsen3/posx.jpg", "Storforsen3/negx.jpg", "Storforsen3/posy.jpg", "Storforsen3/negy.jpg", "Storforsen3/posz.jpg", "Storforsen3/negz.jpg" };
    // char* cubemapFiles[] = {
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysLeft2048.png",
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysRight2048.png",
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysUp2048.png",
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysDown2048.png",
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysFront2048.png",
    //     "skyboxset1/CloudyLightRays/CloudyLightRaysBack2048.png"
    //      };
    // char* cubemapFiles[] = {
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterLeft2048.png",
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterRight2048.png",
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterUp2048.png",
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterDown2048.png",
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterFront2048.png",
    //     "skyboxset1/ThickCloudsWater/ThickCloudsWaterBack2048.png"
    //      };
    // char* cubemapFiles[] ={
    //     "skybox/right.jpg",
    //     "skybox/left.jpg",
    //     "skybox/top.jpg",
    //     "skybox/bottom.jpg",
    //     "skybox/front.jpg",
    //     "skybox/back.jpg"
    // };
char* cubemapFiles[] ={
    "morose/morose_ft.jpg",
    "morose/morose_bk.jpg",
    "morose/morose_up.jpg",
    "morose/morose_dn.jpg",
    "morose/morose_rt.jpg",
    "morose/morose_lf.jpg",
};
int imchannels = GL_RGB;
bool imflip = false;

unsigned int cubemapVBO, cubemapVAO;
float cubemapVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
unsigned int cubemapShaderProgram;


int success; char infoLog[ 512 ]; // for shaders





static unsigned int compilevsfsShader( const char* v, const char* f ) // returns shader program
{
    char* vsstr = 0, * fsstr = 0; // load sources
        fileToString( &vsstr, fopen( v, "rb" ) );
        fileToString( &fsstr, fopen( f, "rb" ) );
        const char* vsstring = (const char*)vsstr,
                  * fsstring = (const char*)fsstr;

    unsigned int vshader; // compile shader
        vshader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( vshader, 1, &vsstring, NULL );
        glCompileShader( vshader );
        success = 1; memset( infoLog, 0, 512 ); // check if shader compiled successfully
        glGetShaderiv( vshader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( vshader, 512, NULL, infoLog );
            printf( "ERROR COMPILING VERTEX SHADER\n%s\n", infoLog );
        }

    unsigned int fshader; // compile shader
        fshader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( fshader, 1, &fsstring, NULL );
        glCompileShader( fshader );
        success = 1; memset( infoLog, 0, 512 ); // check if shader compiled successfully
        glGetShaderiv( fshader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( fshader, 512, NULL, infoLog );
            printf( "ERROR COMPILING FRAGMENT SHADER\n%s\n", infoLog );
        }

    unsigned int shaderProgram = glCreateProgram(); // link shaders
        glAttachShader( shaderProgram, vshader );
        glAttachShader( shaderProgram, fshader );
        glLinkProgram( shaderProgram );
        success = 1; memset( infoLog, 0, 512 ); // check if shader program linked
        glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
        if( !success )
        {
            glGetProgramInfoLog( shaderProgram, 512, NULL, infoLog );
            printf( "ERROR LINKING SHADER PROGRAM\n%s\n", infoLog );
        }
        glDeleteShader( vshader );
        glDeleteShader( fshader );

    return shaderProgram;
}

static unsigned int compilevsgsfsShader( const char* v, const char* g, const char* f )
{
    char* vsstr = 0, * fsstr = 0, * gsstr = 0; // load sources
        fileToString( &vsstr, fopen( v, "rb" ) );
        fileToString( &fsstr, fopen( f, "rb" ) );
        fileToString( &gsstr, fopen( g, "rb" ) );
        const char* vsstring = (const char*)vsstr,
                  * fsstring = (const char*)fsstr,
                  * gsstring = (const char*)gsstr;

    unsigned int vshader; // compile shader
        vshader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( vshader, 1, &vsstring, NULL );
        glCompileShader( vshader );
        success = 1; memset( infoLog, 0, 512 ); // check if shader compiled successfully
        glGetShaderiv( vshader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( vshader, 512, NULL, infoLog );
            printf( "ERROR COMPILING VERTEX SHADER\n%s\n", infoLog );
        }

    unsigned int fshader; // compile shader
        fshader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( fshader, 1, &fsstring, NULL );
        glCompileShader( fshader );
        success = 1; memset( infoLog, 0, 512 ); // check if shader compiled successfully
        glGetShaderiv( fshader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( fshader, 512, NULL, infoLog );
            printf( "ERROR COMPILING FRAGMENT SHADER\n%s\n", infoLog );
        }

    unsigned int gshader; // compile shader
        gshader = glCreateShader( GL_GEOMETRY_SHADER );
        glShaderSource( gshader, 1, &gsstring, NULL );
        glCompileShader( gshader );
        success = 1; memset( infoLog, 0, 512 ); // check if shader compiled successfully
        glGetShaderiv( gshader, GL_COMPILE_STATUS, &success );
        if( !success )
        {
            glGetShaderInfoLog( gshader, 512, NULL, infoLog );
            printf( "ERROR COMPILING GEOMETRY SHADER\n%s\n", infoLog );
        }

    unsigned int shaderProgram = glCreateProgram(); // link shaders
        glAttachShader( shaderProgram, vshader );
        glAttachShader( shaderProgram, fshader );
        glAttachShader( shaderProgram, gshader );
        glLinkProgram( shaderProgram );
        success = 1; memset( infoLog, 0, 512 ); // check if shader program linked
        glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
        if( !success )
        {
            glGetProgramInfoLog( shaderProgram, 512, NULL, infoLog );
            printf( "ERROR LINKING SHADER PROGRAM\n%s\n", infoLog );
        }
        glDeleteShader( vshader );
        glDeleteShader( fshader );
        glDeleteShader( gshader );

    return shaderProgram;
}



static void updateVBO()
{
    struct stack *vert = &vertices, *tri = &triangles;

    ///// UPDATE VBO ////////////////
    glBindVertexArray( VAO ); // moved to setup because no reason to keep uploading arrays
        // ! update VBO and EBO buffer before here please; sway ! i lied sway is a matrix or shader something but if i do change verts then yeah
        glBindBuffer( GL_ARRAY_BUFFER, VBO );
        glBufferData( GL_ARRAY_BUFFER, vert->size * sizeof( float ) /* CHECK */, vert->stac, GL_STATIC_DRAW ); // <x,y,z, nx,ny,nz, s,t,tex,depth>
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, tri->size * sizeof( unsigned int /* CHECK */ ), treeindices, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0 ); // positions
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (3 * sizeof(float)) ); // normals
        glEnableVertexAttribArray( 1 ); // can move a lot of this to setup for static stuff
        glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (6 * sizeof(float)) ); // texture and depth info
        glEnableVertexAttribArray( 2 );
}



static void setup()
{
    glEnable( GL_DEPTH_TEST );
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe mode

    genMirrorWaterVerts();



    ///// SHADERS ////////////////
    shaderProgram = compilevsfsShader( "vs.glsl", "fs.glsl" );
        compilevsgsfsShader( "vsmirror.glsl", "gsmirror.glsl", "fsmirror.glsl" ); // probably don't use geometry shader ig idk
        mirrorShaderProgram = compilevsfsShader( "vsmirror.glsl", "fsmirror.glsl" );
        cubemapShaderProgram = compilevsfsShader( "vscubemap.glsl", "fscubemap.glsl" );


    ///// CREATE OBJECT ////////////////
    glGenBuffers( 1, &VBO );
        glGenBuffers( 1, &EBO );
        glGenVertexArrays( 1, &VAO );

        glGenBuffers( 1, &lightVBO );
        glGenVertexArrays( 1, &lightVAO );

        // update light cube data
        glBindVertexArray( lightVAO );
        glBindBuffer( GL_ARRAY_BUFFER, lightVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( lightVertices ), lightVertices, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0 ); // positions
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (3 * sizeof(float)) ); // normals
        glEnableVertexAttribArray( 1 ); // can move a lot of this to setup for static stuff
        glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*) (6 * sizeof(float)) ); // texture and depth info
        glEnableVertexAttribArray( 2 );


    ///// CREATE MIRROR OBJECT ////////////////
    glGenBuffers( 1, &mirrorVBO );
        glGenBuffers( 1, &mirrorEBO );
        glGenVertexArrays( 1, &mirrorVAO );

        glBindVertexArray( mirrorVAO ); // verts never change so can make whole thing here, only texture on it changes
        glBindBuffer( GL_ARRAY_BUFFER, mirrorVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( mirrorVertices ), mirrorVertices, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mirrorEBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( mirrorIndices ), mirrorIndices, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0 );
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)) );
        glEnableVertexAttribArray( 1 ); // can move a lot of this to setup for static stuff
        // glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)) );
        // glEnableVertexAttribArray( 2 );

    ///// CREATE SKYBOX OBJECT ////////////////
    glGenBuffers( 1, &cubemapVBO );
        glGenVertexArrays( 1, &cubemapVAO );
        glBindVertexArray( cubemapVAO );
        glBindBuffer( GL_ARRAY_BUFFER, cubemapVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( cubemapVertices ), cubemapVertices, GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 );
        glEnableVertexAttribArray( 0 );





    ///// TEXTURES ////////////////
    int width, height, nrChannels;
        stbi_set_flip_vertically_on_load( true );
        unsigned char* imdata = stbi_load( "wood.jpg", &width, &height, &nrChannels, 0 );
        glGenTextures( 1, &woodTexture );
        glBindTexture( GL_TEXTURE_2D, woodTexture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        if( imdata )
        {
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imdata );
            glGenerateMipmap( GL_TEXTURE_2D );
        }
        else printf( "ERROR LOADING TEXTURE\n" );
        stbi_image_free( imdata );


        glUseProgram( shaderProgram );
        glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ); // which you have to glActiveTexture before glBindTexture

        // ---

        width = 0; height = 0; nrChannels = 0;
        stbi_set_flip_vertically_on_load( true );
        imdata = stbi_load( "leaf.jpg", &width, &height, &nrChannels, 0 );
        glGenTextures( 1, &leafTexture );
        glBindTexture( GL_TEXTURE_2D, leafTexture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        if( imdata )
        {
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imdata );
            glGenerateMipmap( GL_TEXTURE_2D );
        }
        else printf( "ERROR LOADING TEXTURE\n" );
        stbi_image_free( imdata );

        // glUseProgram( shaderProgram ); // already using from above texture
        glUniform1i( glGetUniformLocation( shaderProgram, "tex2" ), 1 );

        // ---

        // cubemap
        glGenTextures( 1, &cubemapTexture );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        width = 0; height = 0; nrChannels = 0; // i might not need these anywhere except the first but whatever
        for( int i = 0; i < 6; i++ )
        {
            stbi_set_flip_vertically_on_load( imflip );
            imdata = stbi_load( cubemapFiles[i], &width, &height, &nrChannels, 0 );
            if( imdata )
            {
                glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imchannels, width, height, 0, imchannels, GL_UNSIGNED_BYTE, imdata );
                stbi_image_free( imdata );
            }
            else printf( "ERROR LOADING CUBEMAP TEXTURE %s\n", cubemapFiles[i] );
        }
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );





    ///// FROM TREE RENDERER ////////////////
    a = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        b = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        c = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        d = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        e = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        f = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        g = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        vertices = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};
        triangles = (Stack){.maxSize = 1, .size = 0, .stac = malloc(sizeof(float))};

        // tree = glGenLists(1);
        // glNewList(tree, GL_COMPILE);
        struct stack /**vert = &vertices,*/ *tri = &triangles;
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );

        // for( int i = 0; i < vert->size; i += 10 )
        // {
            // // if( vert->stac[i+8] == (float)leafTexture ) printf( "%f\n", vert->stac[i+9] );
            // printf( "%f %f %f, %f %f %f, %f %f, %f, %f\n",
            //     vert->stac[i+0],vert->stac[i+1],vert->stac[i+2],
            //     vert->stac[i+3],vert->stac[i+4],vert->stac[i+5],
            //     vert->stac[i+6],vert->stac[i+7],
            //     vert->stac[i+8],
            //     vert->stac[i+9] );
        // }
        // printf( "\n" );
        // printf( "%d %d  %d %d\n", vert->size, tri->size, woodTexture, leafTexture );



        treeindices = (unsigned int*)malloc( sizeof(unsigned int) * tri->size );
        for( int i = 0; i < tri->size; i++ ) // because for some reason they are currently floats lol
            treeindices[i] = (int)( tri->stac[i] );


    glUniform1f( glGetUniformLocation( shaderProgram, "texSwitchDepth" ), maxDepth - branchDepth );






    updateVBO();

        // printf( "%d\n", glGetError() );

}
static void whsetup() // called after globalwidth and globalheight are set
{
    ///// MIRROR/FBO(/RBO) AND TEXTURE ////////////////
    glGenFramebuffers( 1, &mirrorFBO );
        glBindFramebuffer( GL_FRAMEBUFFER, mirrorFBO );

        glGenTextures( 1, &mirrorTexture );
        glBindTexture( GL_TEXTURE_2D, mirrorTexture );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, globalwidth, globalheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glBindTexture( GL_TEXTURE_2D, 0 );

        // mirrorFBO is still bound at this point
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture, 0 ); // only need color for very shiny mirror


        glGenRenderbuffers( 1, &mirrorRBO );
        glBindRenderbuffer( GL_RENDERBUFFER, mirrorRBO );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, globalwidth, globalheight );
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );

        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorRBO );


        if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
            printf( "ERROR CREATING FRAMEBUFFER" );


    ///// PROJECTION MATRIX ////////////////
    glm_perspective( glm_rad( 45.0f ), (float)globalwidth / (float)globalheight, 0.1f, 100.0f, projMat );
}

static void draw() // maybe ensure whsetup got called idk
{
    updateTime();
    glClearColor( .5, .4, .6, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    struct stack /**vert = &vertices,*/ *tri = &triangles;


    ///// UPDATE CAMERA ////////////////
    if( firstpersonmode )
    {
        float r = dt * 3.0;

        if( keys['W'] || keys['A'] || keys['S'] || keys['D'] ) r *= 2;

        if( keys['w'] || keys['W'] )
            glm_vec_copy( (vec3){ camPos[0] + cos( camYaw + glm_rad( 90 ) ) * r, camPos[1],
                                  camPos[2] + sin( camYaw + glm_rad( 90 ) ) * r }, camPos );
        if( keys['s'] || keys['S'] )
            glm_vec_copy( (vec3){ camPos[0] - cos( camYaw + glm_rad( 90 ) ) * r, camPos[1],
                                  camPos[2] - sin( camYaw + glm_rad( 90 ) ) * r }, camPos );

        if( keys['a'] || keys['A'] )
            glm_vec_copy( (vec3){ camPos[0] + cos( camYaw ) * r, camPos[1],
                                  camPos[2] + sin( camYaw ) * r }, camPos );
        if( keys['d'] || keys['D'] )
            glm_vec_copy( (vec3){ camPos[0] - cos( camYaw ) * r, camPos[1],
                                  camPos[2] - sin( camYaw ) * r }, camPos );

        if( keys[' '] ) glm_vec_copy( (vec3){ camPos[0], camPos[1] - r, camPos[2] }, camPos );
        if( keys['x'] ) glm_vec_copy( (vec3){ camPos[0], camPos[1] + r, camPos[2] }, camPos );

        camYaw += ( mousex - globalwidth / 2 ) / 400.;
        camPitch += ( mousey - globalheight / 2 ) / 250.;

        // printf( "%f, %f, %f\n", camPos[0], camPos[1], camPos[2] );
    }
    else
    {
        camPitch = ( mousey - globalheight / 2. ) * glm_rad( 80 ) / globalheight;
        camYaw += yawSpinRate / 400.;
        // if( abs( yawSpinRate ) > 5 ) yawSpinRate *= .99;
    }


    ///// POSITION TREE ////////////////
    // glm_rotate_make( objMat, timems / 2700., (vec3){ .5, 1., 0. } );
    glm_mat4_identity( objMat );
    // glm_translate_make( objMat, (vec3){ 0., /*0.2*//*.7*/0.7, 0. } );
    // glm_translate_make( objMat, (vec3){ 0., -0.2, 0. } );
    glm_rotate_x( objMat, glm_rad( -90. ), objMat );
    glm_scale_uni( objMat, .2 );
    glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)objMat );



    ///// UPDATE SHADER UNIFORMS ////////////////
    glUseProgram( shaderProgram );
        // glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)viewCamMat ); // updated in mirror render
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "projMat" ), 1, GL_FALSE, (float*)projMat );
        glUniform1f( glGetUniformLocation( shaderProgram, "time" ), timems / 1000.0 );

        // glUniform4f( glGetUniformLocation( shaderProgram, "vertexColour" ), 0.5f, ( sin( timems / 1000. ) + 1. ) / 2., 0.5f, 1.0f );// can move most of this into setup; also check for -1 uniform location that means it couldn't find it



    ///// OFFSCREEN MIRROR RENDER ////////////////
    glBindFramebuffer( GL_FRAMEBUFFER, mirrorFBO );
        // can change clear color here
        glClearColor( 0., 1., 1., 1. );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        mat4 mirrorCamMat;
        if( firstpersonmode )
        {
            glm_rotate_make( mirrorCamMat, -camPitch, (vec3){ 1., 0., 0. } );
            glm_rotate( mirrorCamMat, camYaw, (vec3){ 0., 1., 0. } );
            glm_translate( mirrorCamMat, (vec3){ camPos[0], -camPos[1], camPos[2] } ); // invert camera matrix
        }
        else
        {
            glm_translate_make( mirrorCamMat, (vec3){ camPos[0], -camPos[1], camPos[2] } ); // invert camera matrix
            glm_rotate( mirrorCamMat, -camPitch, (vec3){ 1., 0., 0. } );
            glm_rotate( mirrorCamMat, camYaw, (vec3){ 0., 1., 0. } );
        }

        ///// RENDER CUBEMAP VBO //////////////// // MAKE SURE THIS IS THE SAME AS THE ONE BELOW !! (also these are inefficient but whatever)
        glDepthMask( GL_FALSE );
            glUseProgram( cubemapShaderProgram );
            mat4 untranslated;
            glm_mat4_identity( untranslated );
            mat3 withoutw;
            glm_mat3_identity( withoutw );
            glm_mat4_pick3( mirrorCamMat, withoutw );
            glm_mat4_ins3( withoutw, untranslated );
            // glm_translate_y( untranslated, .25 );
            glUniformMatrix4fv( glGetUniformLocation( cubemapShaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)untranslated );
            glUniformMatrix4fv( glGetUniformLocation( cubemapShaderProgram, "projMat" ), 1, GL_FALSE, (float*)projMat );
            glUniform3fv( glGetUniformLocation( cubemapShaderProgram, "colour" ), 1, (float*)((vec3){ 0., .3, .5 }) );

            glBindVertexArray( cubemapVAO );
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
            glDepthMask( GL_TRUE );


        glUseProgram( shaderProgram );

        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)mirrorCamMat );


        glActiveTexture( GL_TEXTURE0 ); // draw as normal but this time to a texture (to the FBO)
        glBindTexture( GL_TEXTURE_2D, woodTexture );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, leafTexture );
        glBindVertexArray( VAO );
        glDrawElements( GL_TRIANGLES, tri->size * sizeof(unsigned int /* CHECK */), GL_UNSIGNED_INT, 0 );
        // glBindVertexArray( 0 ); // cleared at end


        mat4 lightPos;
        glm_translate_make( lightPos, (vec3){ cos( timems * 1.5 / 1000. ) * 5., 4., sin( timems * 1.5 / 1000. ) * 5. } );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)lightPos );
        glBindVertexArray( lightVAO );
        glDrawArrays( GL_TRIANGLES, 0, sizeof( lightVertices ) );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)objMat );



        glBindFramebuffer( GL_FRAMEBUFFER, 0 ); // restore rendering to screen



    ///// SET CAMERA MATRIX
    if( firstpersonmode )
    {
        glm_rotate_make( viewCamMat, camPitch, (vec3){ 1., 0., 0. } );
        glm_rotate( viewCamMat, camYaw, (vec3){ 0., 1., 0. } );
        glm_translate( viewCamMat, camPos );
    }
    else
    {
        glm_translate_make( viewCamMat, camPos );
        glm_rotate( viewCamMat, camPitch, (vec3){ 1., 0., 0. } );
        glm_rotate( viewCamMat, camYaw, (vec3){ 0., 1., 0. } );
    }
    glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)viewCamMat );


    ///// RENDER CUBEMAP VBO ////////////////
    glDepthMask( GL_FALSE );
        glUseProgram( cubemapShaderProgram );
        // mat4 untranslated; // defined when rendered in mirror
        glm_mat4_identity( untranslated );
        // mat3 withoutw; // defined when rendered in mirror
        glm_mat3_identity( withoutw );
        glm_mat4_pick3( viewCamMat, withoutw );
        glm_mat4_ins3( withoutw, untranslated );
        // glm_translate_y( untranslated, .75 );
        glUniformMatrix4fv( glGetUniformLocation( cubemapShaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)untranslated );
        glUniformMatrix4fv( glGetUniformLocation( cubemapShaderProgram, "projMat" ), 1, GL_FALSE, (float*)projMat );
        glUniform3fv( glGetUniformLocation( cubemapShaderProgram, "colour" ), 1, (float*)((vec3){ 0., .3, .5 }) );

        glBindVertexArray( cubemapVAO );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glDepthMask( GL_TRUE );


    ///// RENDER VBO ////////////////
    glUseProgram( shaderProgram ); // since probably updating uniforms this will already be bound
        glActiveTexture( GL_TEXTURE0 ); // if multiple textures set which texture we're binding
        glBindTexture( GL_TEXTURE_2D, woodTexture );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, leafTexture );
        glBindVertexArray( VAO );
        // glDrawArrays( GL_TRIANGLES, 0, 3 );
        glDrawElements( GL_TRIANGLES, tri->size * sizeof(unsigned int /* CHECK */), GL_UNSIGNED_INT, 0 );
        // glBindVertexArray( 0 ); // cleared at end

        glm_translate_make( lightPos, (vec3){ cos( timems * 1.5 / 1000. ) * 5., 4., sin( timems * 1.5 / 1000. ) * 5. } );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)lightPos );
        glBindVertexArray( lightVAO );
        glDrawArrays( GL_TRIANGLES, 0, sizeof( lightVertices ) );
        glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)objMat );

    ///// RENDER MIRROR VBO ////////////////
        // glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "objMat" ), 1, GL_FALSE, (float*)noRot );
    glUseProgram( mirrorShaderProgram );
        mat4 atorigin;
        glm_mat4_identity( atorigin ); // should probably make this elsewhere
        glUniformMatrix4fv( glGetUniformLocation( mirrorShaderProgram, "projMat" ), 1, GL_FALSE, (float*)projMat ); // todo check if these work jesus
        glUniformMatrix4fv( glGetUniformLocation( mirrorShaderProgram, "objMat" ), 1, GL_FALSE, (float*)atorigin );
        glUniformMatrix4fv( glGetUniformLocation( mirrorShaderProgram, "viewCamMat" ), 1, GL_FALSE, (float*)viewCamMat );
        glUniform1f( glGetUniformLocation( mirrorShaderProgram, "time" ), timems / 1000.0 );
        // printf( "%d  %f   %d\n", glGetUniformLocation( mirrorShaderProgram, "time" ), timems / 1000.0f, glGetError() ); // ALWAYS CHECK glGetError FOR UNIFORM SETTING
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, mirrorTexture );
        glBindVertexArray( mirrorVAO );
        glDrawElements( GL_TRIANGLES, sizeof(mirrorIndices), GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );

        // glUseProgram( shaderProgram );


    glutSwapBuffers();
}




// update camera!

static void end() // yo actually finish this lol
{
    glDeleteFramebuffers( 1, &mirrorFBO );

    // from tree renderer
    free(a.stac);
    free(b.stac);
    free(c.stac);
    free(d.stac);
    free(e.stac);   
    free(f.stac);
    free(g.stac);

    glutDestroyWindow( win );
}















// ---

/* new window size or exposure */ // also abused for zoom
bool firstCallReshape = true;
static void reshape( int width, int height )
{
    globalwidth = width;
    globalheight = height;

    if( firstCallReshape ){ whsetup(); firstCallReshape = false; }
}

// --- input ---

int dragStartX, dragStartY;
void dragStart( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        dragStartX = x; dragStartY = y;

        if( !firstpersonmode ) yawSpinRate = 0;
    }
}
void drag( int x, int y )
{
    if( !firstpersonmode ) yawSpinRate = ( x - dragStartX ) / 10.;
}

void mousePos( int x, int y )
{
    mousex = x; mousey = y;

    if( firstpersonmode )
    {
        glutWarpPointer( globalwidth / 2, globalheight / 2 );
    }
}

static void keyDown( unsigned char k, int x, int y )
{
    if( k == 27 /* ESC */ ) { end(); exit( 0 ); }
    keys[ k ] = true;
}

static void keyUp( unsigned char k, int x, int y )
{
    if( k == 'q' )
    {
        if( firstpersonmode ) // reset camera
        {
            glm_vec_copy( (vec3){ 0, -1, -(sqrt( camPos[0]*camPos[0]+camPos[2]*camPos[2] )) }, camPos );
            camYaw = 0, camPitch = 0.174532925; // glm_rad( 10. ); fuck c lol
        }
        else
        {
            glm_vec_copy( (vec3){ 0, -1, -(sqrt( camPos[0]*camPos[0]+camPos[2]*camPos[2] )) }, camPos );
            camYaw = 0, camPitch = 0.174532925; // glm_rad( 10. ); fuck c lol
            glutWarpPointer( globalwidth / 2, globalheight / 2 );
        }

        firstpersonmode ^= 1;
    }
    else if( k == 'r' )
    {
        angleLower -= 5.0;
        if(angleLower < 0) angleLower = 0;
        printf("angleLower:%f\n", angleLower);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'f' )
    {
        angleLower += 5.0;
        if(angleLower >= angleHigher) angleLower = angleHigher-5;
        printf("angleLower:%f\n", angleLower);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 't' )
    {
        angleHigher -= 5.0;
        if(angleLower >= angleHigher) angleHigher = angleLower+5;
        printf("angleHigher:%f\n", angleHigher);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'g' )
    {
        angleHigher += 5.0;
        if (angleHigher > 180) angleHigher = 180;
        printf("angleHigher:%f\n", angleHigher);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'y' )
    {
        crazy += 5;
        printf("crazy:%f\n", crazy);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'h' )
    {
        crazy -= 5;
        if (crazy < 0) crazy = 0;
        printf("crazy:%f\n", crazy);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'u' )
    {
        thicc += .3;
        printf("thicc:%f\n", thicc);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'j' )
    {
        thicc -= .1;
        if (thicc < .1) thicc = .3;
        printf("thicc:%f\n", thicc);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
    }
    else if( k == 'i' )
    {
        iterations += 1;
        if( iterations > maxiterations ) iterations = maxiterations;
        char* z = generateString(iterations);
        maxDepth = 0;
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        printf("iterations:%d maxDepth:%d\n", iterations, maxDepth);
        free( z );
        updateVBO();
        glUseProgram( shaderProgram );
        glUniform1f( glGetUniformLocation( shaderProgram, "texSwitchDepth" ), maxDepth - branchDepth );
    }
    else if( k == 'k' )
    {
        iterations -= 1;
        if(iterations < 1) iterations = 1;
        char* z = generateString(iterations);
        maxDepth = 0;
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        printf("iterations:%d maxDepth:%d\n", iterations, maxDepth);
        free( z );
        updateVBO();
        glUseProgram( shaderProgram );
        glUniform1f( glGetUniformLocation( shaderProgram, "texSwitchDepth" ), maxDepth - branchDepth );
    }
    else if( k == 'o' )
    {
        branchDepth += 1;
        if( branchDepth > maxDepth ) branchDepth = maxDepth;
        printf("branchDepth:%d\n", branchDepth);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
        glUseProgram( shaderProgram );
        glUniform1f( glGetUniformLocation( shaderProgram, "texSwitchDepth" ), maxDepth - branchDepth );
    }
    else if( k == 'l' )
    {
        branchDepth -= 1;
        if(branchDepth < 0) branchDepth = 0;
        printf("branchDepth:%d\n", branchDepth);
        char* z = generateString(iterations);
        drawFromString(z, size, 0.0, 0.0, 0.0); // fills vertices and triangles
        free( z );
        updateVBO();
        glUseProgram( shaderProgram );
        glUniform1f( glGetUniformLocation( shaderProgram, "texSwitchDepth" ), maxDepth - branchDepth );
    }

    keys[ k ] = false;
}

static void idle()
{
    glutPostRedisplay();
}

static void visible( int vis )
{
    if( vis == GLUT_VISIBLE ) glutIdleFunc( idle );
    else glutIdleFunc( NULL );
}

int main( int argc, char *argv[] )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 800, 800 );
    win = glutCreateWindow( "Elijah Fisher - Michael Rahn - Final Project - Tree" );


    // TODO ONLY DO IF IN WINDOWS
    #ifndef __linux__
    glewInit();
    #endif

    glutMouseFunc( dragStart );
    glutMotionFunc( drag );
    glutPassiveMotionFunc( mousePos );

    glutDisplayFunc( draw );
    glutReshapeFunc( reshape );

    glutKeyboardUpFunc( keyUp );
    glutKeyboardFunc( keyDown );

    glutVisibilityFunc( visible );

    

    setup();

    glutMainLoop();


    return 0;
}
