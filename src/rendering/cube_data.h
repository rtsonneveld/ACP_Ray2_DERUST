#pragma once

const float vertices[] = {
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f
};

const unsigned int indices[] = {
    0,1,2, 2,3,0, 4,5,6, 6,7,4,
    0,1,5, 5,4,0, 2,3,7, 7,6,2,
    0,3,7, 7,4,0, 1,2,6, 6,5,1
};
