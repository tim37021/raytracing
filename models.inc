static
light light1 = {
    .light_color = {   0.7,  0.5, 0.5 },
    .position =    { 5, 5, 20 },
    .intensity = 200.0,
};

static
light light2 = {
    .light_color = {   0.8,  0.8, 0.8 },
    .position =    { 10, 10, 20 },
    .intensity = 200.0,
};

static
sphere sphere1 = {
    .center = { 5, 0, 5 },
    .radius = 3,
    .fill = {
        .fill_color = { 0.8, 0.8, 0.8 },
        .Kd = 0.8, .Ks = 0.8, .T = 0.0, .R=0.6,
        .index_of_refraction = 0.0, .phong_power = 30.0
    }
};


static
sphere sphere2 = {
    .center = { 20, 15, 15 },
    .radius = 3,
    .fill = {
        .fill_color = { 0.8, 0.6, 0.4 },
        .Kd = 0.0, .Ks = 1.0, .T = 1.0, .R=1.0,
        .index_of_refraction = 1.5, .phong_power = 30.0
    }
};

static
sphere sphere3 = {
    .center = { 5, 10, 5 },
    .radius = 3,
    .fill = {
        .fill_color = { 0.4, 0.8, 0.6 },
        .Kd = 0.8, .Ks = 0.1, .T = 0.0, .R=0.1,
        .index_of_refraction = 0.0, .phong_power = 30.0
    }
};


static
viewpoint view = {
    .vrp = { 40.0,  40.0,  40.0 },
    .vpn = {  -1.0, -1.0, -1.0 },
    .vup = {  0.0, 0.0,  1.0 }
};


static
rectangle rectangle1 = {
    .vertices = {
        { 0.0, 0.0, 0.0 },	/* v0 */
        { 0.0, 0.0, 20.0 },	/* v1 */
        { 20.0, 0.0, 20.0 },	/* v3 */
        { 20.0, 0.0, 0.0 },	/* v2 */
    },
    .fill = {
        .fill_color = { 0.6, 0.6, 0.6 },
        .Kd = 0.8, .Ks = 0.0, .T = 0.0, .R=0.5,
        .index_of_refraction = 0.0, .phong_power = 5.0,
    },
};

static
rectangle rectangle2 = {
    .vertices = {
        { 0.0, 0.0, 0.0 },	/* v0 */
        { 20.0, 0.0, 0.0 },	/* v1 */
        { 20.0, 20.0, 0.0 },	/* v3 */
        { 0.0, 20.0, 0.0 },	/* v2 */
    },
    .fill = {
        .fill_color = { 0.6, 0.1, 0.1 },
        .Kd = 0.8, .Ks = 0.0, .T = 0.0, .R=0.5,
        .index_of_refraction = 0.0, .phong_power = 5.0,
    },
};



static
rectangle rectangle3 = {
    .vertices = {
        { 0.0, 0.0, 0.0 },	/* v0 */
        { 0.0, 20.0, 0.0 },	/* v1 */
        { 0.0, 20.0, 20.0 },	/* v3 */
        { 0.0, 0.0, 20.0 },	/* v2 */
    },
    .fill = {
        .fill_color = { 0.1, 0.1, 0.6 },
        .Kd = 0.8, .Ks = 0.0, .T = 0.0, .R=0.5,
        .index_of_refraction = 0.0, .phong_power = 5.0,
    },
};

/* The following will be preprocessed by Makefile, they are "not only" comments */
/*
LOAD_OBJ_SCENE models/monkey.obj {0,0,0} {0,0,0} {1,1,1}
*/