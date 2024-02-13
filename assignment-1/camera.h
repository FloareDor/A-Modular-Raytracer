#ifndef CAMERA_H
#define CAMERA_H

#include<cmath>
#include "vec3.h"
#include "shader.h"
#include "objects.h"
#include "ray.h"

// Generate ray direction based on pixel coordinates with fisheye effect
Vec3 fishRayDirection(int x, int y, double width, double height, double fov) {
    float aspectRatio = float(width) / float(height);
    float fovRadians = fov * 3.147 / 180.0;
    float halfWidth = tan(fovRadians / 2.0);
    float halfHeight = halfWidth / aspectRatio;

    // Map x, y to [-1, 1]
    float scaleX = (2.0 * x - width) / width;
    float scaleY = (2.0 * y - height) / height;

    // Apply fisheye effect
    float r = sqrt(scaleX * scaleX + scaleY * scaleY);
    float theta = atan2(scaleY, scaleX);
    float phi = r * halfWidth * 3.147;

    float dirX = cos(phi) * cos(theta);
    float dirY = cos(phi) * sin(theta);
    float dirZ = sin(phi);

    return Vec3(dirX, dirY, dirZ).unit_vector();
}


// My camera system is heavily influenced by "Ray Tracing in One Weekend E-Book"
// I really enjoyed the explanation by Dr. Peter Shirley
// https://raytracing.github.io/
unsigned char* CameraAndScene(bool orthogonal, int width, int height, Vec3 cameraPosition, Vec3 camera_up, Vec3 look_at, Vec3 light_pos, Vec3 sphere1_centre, Vec3 sphere2_centre, Vec3 s3_centre){

	// camera parameters
	// Vec3 cameraPosition = Vec3(0,-1,10);
	// cameraPosition = Vec3(4, -1, 5);
	// Vec3 camera_up = Vec3(0, -1, 0);
	// Vec3 look_at = Vec3(0, 0, -1);
	// cameraPosition = Vec3(0, -1, 1);

	auto viewplane_height = 6.0; // 4.2
	auto viewplane_width = viewplane_height * (static_cast<double>(width)/height);

	// basis vectors for camera
	Vec3 W = (cameraPosition - look_at).unit_vector();
	Vec3 U = camera_up.cross(look_at);
	Vec3 V = W.cross(U);

	// The vectors along the axis(es) of the viewplane
	// Vec3 viewplane_u = Vec3(viewplane_width, 0, 0);
	// Vec3 viewplane_v = Vec3(0, -viewplane_height, 0);

	Vec3 viewplane_u = U * viewplane_width;
	Vec3 viewplane_v = V*-1 * viewplane_height;


	// The horizontal and vertical delta vectors from pixel to pixel.
	Vec3 pixel_delta_u = viewplane_u / width;
	Vec3 pixel_delta_v = viewplane_v / height;

	double focalLength = (cameraPosition - look_at).length();
	if(orthogonal){
		focalLength = 0.0;
	}
	// Calculate the location of the upper left pixel.
	// Vec3 viewplane_upper_left = (cameraPosition - Vec3(0, 0, focalLength)) - viewplane_u/2 - viewplane_v/2;
	Vec3 viewplane_upper_left = (cameraPosition - (W*focalLength)) - viewplane_u/2 - viewplane_v/2;

	Vec3 initial_pixel = viewplane_upper_left + (pixel_delta_u + pixel_delta_v) * 0.5;

	// std::cout << "inital_pixel:" << std::endl;
	// initial_pixel.print();

	unsigned char* image = new unsigned char[width*height*3]; // to avoid stack overflow

	Vec3 lightsource_pos = Vec3(2,-2,0.5);

	Objects world;

	// Sphere s1(Vec3(0, 0, -1.1),0.8,Color(1,2,1));
	// Sphere s2(Vec3(0.9, 0.5, -0.2), 0.3, Color(0.5, 1.68, 1.52));
	// Plane  p1(Vec3(0, -1, 0), 0.8, Color(1, 2, 1));

	Shader shader;
	std::shared_ptr<Sphere> sphere_ptr1 = std::make_shared<Sphere>(sphere1_centre, 0.8, Color(0,0,0)*0.75, shader, true); // Vec3(1, 0, -2), Color(2.44,1.94,1.94)*0.75
	std::shared_ptr<Sphere> sphere_ptr2 = std::make_shared<Sphere>(sphere2_centre, 0.2, Color(0.70, 1.50, 1.84), shader, true);// Color(0.70, 1.50, 1.84)
	std::shared_ptr<Sphere> sphere_ptr3 = std::make_shared<Sphere>(s3_centre, 0.4, Color(0.3, 0.4, 0.7), shader, true);// Color(0.3, 0.4, 0.7)
	std::shared_ptr<Plane>  plane_ptr   = std::make_shared<Plane>(Vec3(0, -1, 0), 0.8, Color(0.8,0.1,0.4), shader, true); // 10.32,10.32,10.33 0.8,0.1,0.4 // Color(0.8,0.1,0.4)
	// std::shared_ptr<Triangle>  triangle1_ptr   = std::make_shared<Triangle>(Vec3(0, -0.4, -0.5), Vec3(-200, -200, -12), Vec3(200, -200, -12), Color(1.33, 2.00, 0.69), shader, false);
	std::shared_ptr<Tetrahedron>  tetrahedron   = std::make_shared<Tetrahedron>(Vec3(-1, 0.6, -0.2), Vec3(0, 0.6, -0.8), Vec3(-2, 0.6, -0.8), Vec3(-1, -0.35, -0.5), Color(0.70, 1.84, 1.61), shader, true); // Color(1.33, 2.00, 0.69) // Color(0.70, 1.84, 1.61)


	// Sunlight sunlight1(light_pos, 12); // Vec3(-3, -2, 1) // 8 with 3
	// Sunlight sunlight2(Vec3(3, -2, 0), 8);
	Sunlight sunlight3(Vec3(0, -2, 3), 4);
	Sunlight sunlight4(Vec3(0, -2, -3), 4);
	Sunlight sunlight5(Vec3(-2, -2, 1), 8);

	world.addObject(sphere_ptr1);
	world.addObject(sphere_ptr2);
	world.addObject(sphere_ptr3);
	world.addObject(plane_ptr);
	// world.addObject(triangle1_ptr);
	world.addObject(tetrahedron);
	// world.lightsource = sunlight1;
	// world.addLight(sunlight1);
	// world.addLight(sunlight2);
	world.addLight(sunlight3);
	world.addLight(sunlight4);
	world.addLight(sunlight5);

	double fisheye_radius = std::min(width, height) / 2.0;

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			
			Vec3 rayOrigin = cameraPosition;
			auto viewplane_pixel_loc = initial_pixel + (pixel_delta_u * x) + (pixel_delta_v * y);
			Vec3 rayDirection = (viewplane_pixel_loc - cameraPosition).unit_vector();
			// rayDirection = fishRayDirection(x, y, width, height, 2);

			if(orthogonal){
				rayOrigin = initial_pixel + (pixel_delta_u * x) + (pixel_delta_v * y);
				rayDirection = W*-1;
			}

			// rayDirection = Vec3(0.2, 0, -1);

			Ray ray(rayOrigin, rayDirection);
			int idx = (y * width + x) * 3;
			// double distance_to_center = std::sqrt((x - width / 2.0) * (x - width / 2.0) + (y - height / 2.0) * (y - height / 2.0));
			// if(distance_to_center > fisheye_radius) {
			// 	// Ray falls outside the fisheye circle, color it black
			// 	image[idx] = 0;
			// 	image[idx+1] = 0;
			// 	image[idx+2] = 0;
			// }else{

			// Color color = traceRay(ray, lightsource_pos, false);
			Color color = world.castRay(ray, world, lightsource_pos, false);

			// int idx = (y * width + x) * 3;
			image[idx] = color.x;
			image[idx+1] = color.y;
			image[idx+2] = color.z;
		}
	}
	// // unsigned char *data = &image[0];
	// std::vector<unsigned char> image_copy(image, image + width * height * 3); // Using copy constructor

	// saveImage("render.png", image_copy, width, height);
	return image;
}

#endif
