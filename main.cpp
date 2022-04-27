#include "main.h"

// Cat image from: http://www.iacuc.arizona.edu/training/cats/index.html
int main(int argc, char** argv)
{
	//vector<vector<float>> res_by_res = get_data(marching_squares_resolution);

	//size_t target_res = 2;

	//for (size_t i = 0; i < res_by_res.size(); i++)
	//	res_by_res[i] = opencv_downsize(res_by_res[i], marching_squares_resolution, target_res);

	//res_by_res = get_data(res_by_res, target_res);


	srand(static_cast<unsigned int>(123));

	vector<vector<float>> res_by_res;
	
	for (size_t n = 0; n < 50; n++)
	{
		get_vertices();
		res_by_res = get_data(marching_squares_resolution);

		for (size_t i = 0; i < res_by_res.size(); i++)
			res_by_res[i] = opencv_blur(res_by_res[i], 1);


		add_images.push_back(res_by_res[0]);

		float_grayscale luma;

		luma.px = marching_squares_resolution;
		luma.py = marching_squares_resolution;
		luma.pixel_data = res_by_res[0];

		ostringstream oss;
		oss << "image" << n << ".tga";
		write_float_grayscale_to_tga(oss.str().c_str(), luma);


		vector<contour> contours;

		for (size_t i = 0; i < line_segments[0].size(); i++)
		{
			contour c;
			line_segment ls = line_segments[0][i];

			c.d.push_back(ls);
			contours.push_back(c);
		}

		vector<contour> temp_contours;

		while (contours.size() > 0)
			merge_contours(contours, temp_contours);

		add_contours.push_back(temp_contours);
	}

	// Get average image
	
	vector<float> avg_res_by_res;

	avg_res_by_res = get_average(add_images, marching_squares_resolution);

	float_grayscale luma;

	luma.px = marching_squares_resolution;
	luma.py = marching_squares_resolution;
	luma.pixel_data = avg_res_by_res;

	ostringstream oss;
	oss << "image_avg.tga";
	write_float_grayscale_to_tga(oss.str().c_str(), luma);


	vector<vector<float>> avg_res_by_res_array;
	
	for (size_t i = 0; i < type_count; i++)
		avg_res_by_res_array.push_back(avg_res_by_res);

	avg_res_by_res_array = get_data(avg_res_by_res_array, marching_squares_resolution);

	vector<contour> contours;

	for (size_t i = 0; i < line_segments[0].size(); i++)
	{
		contour c;
		line_segment ls = line_segments[0][i];

		c.d.push_back(ls);
		contours.push_back(c);
	}

	vector<contour> temp_contours;

	while (contours.size() > 0)
		merge_contours(contours, temp_contours);

	avg_contours = temp_contours;



	render_image(argc, argv);

	return 0;
}

void render_image(int &argc, char ** &argv)
{
	// Initialize OpenGL.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win_x, win_y);
	win_id = glutCreateWindow("Marching Squares");
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);

	glShadeModel(GL_FLAT);
	glClearColor(background_colour, background_colour, background_colour, 1);

	// Begin rendering.
	glutMainLoop();

	// Cleanup OpenGL.
	glutDestroyWindow(win_id);
}

void idle_func(void)
{
	glutPostRedisplay();
}

// Resize window.
void reshape_func(int width, int height)
{
	if(width < 1)
		width = 1;

	if(height < 1)
		height = 1;

	win_x = width;
	win_y = height;

	// Viewport setup.
	glutSetWindow(win_id);
	glutReshapeWindow(win_x, win_y);
	glViewport(0, 0, win_x, win_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, static_cast<GLfloat>(win_x)/static_cast<GLfloat>(win_y), 0.1, 10);
	gluLookAt(0, 0, camera_z, 0, 0, 0, 0, 1, 0);
}

void keyboard_func(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	{
		test_point.y += 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 's':
	{
		test_point.y -= 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 'a':
	{
		test_point.x -= 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	case 'd':
	{
		test_point.x += 0.01f;

		if (false == get_index(test_point_index))
			test_point_index = get_closest_index(test_point);

		break;
	}
	default:
		break;
	}
}

// Visualization.
void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Scale all geometric primitives so that template width == 1.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(inverse_width, inverse_width, inverse_width);

	// Render a dark background.
	glColor3f(202.0f/255.0f, 170.0f/255.0f, 170.0f/255.0f);
	glBegin(GL_QUADS);
		glVertex2f(-template_width*0.5f, template_height * 0.5f);
		glVertex2f(-template_width * 0.5f, -template_height * 0.5f);
		glVertex2f(template_width * 0.5f, -template_height * 0.5f);
		glVertex2f(template_width * 0.5f, template_height * 0.5f);
	glEnd();

	// Render image area.





	glBegin(GL_TRIANGLES);

	for (size_t i = 0; i < triangles.size(); i++)
	{
		glColor3f(
			(205.0f / 255.0f),
			(205.0f / 255.0f),
			(226.0f / 255.0f));

		for (size_t j = 0; j < triangles[i].size(); j++)
		{
			glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);
			glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);
			glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);
		}
	}
	glEnd();



	////// Draw triangle outlines
	//glBegin(GL_LINES);

	//for (size_t i = 0; i < triangles.size(); i++)
	//{
	//	glColor3f(
	//		(205.0f / 255.0f + 2.0f) / 3.0f,
	//		(205.0f / 255.0f + 2.0f) / 3.0f,
	//		(226.0f / 255.0f + 2.0f) / 3.0f);

	//	for (size_t j = 0; j < triangles[i].size(); j++)
	//	{
	//		glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);
	//		glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);

	//		glVertex2f(triangles[i][j].vertex[1].x, triangles[i][j].vertex[1].y);
	//		glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);

	//		glVertex2f(triangles[i][j].vertex[2].x, triangles[i][j].vertex[2].y);
	//		glVertex2f(triangles[i][j].vertex[0].x, triangles[i][j].vertex[0].y);

	//	}
	//}
	//glEnd();












	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(1);
	glBegin(GL_LINES);



	for (size_t n = 0; n < add_contours.size(); n++)
	{
		for (size_t i = 0; i < add_contours[n].size(); i++)
		{
			glColor4f(0.25f, 0.25f, 0.25f, 0.25f);// static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));// colours[i].r, colours[i].g, colours[i].b);

			for (size_t j = 0; j < add_contours[n][i].d.size(); j++)
			{
				glVertex2f(add_contours[n][i].d[j].vertex[0].x, add_contours[n][i].d[j].vertex[0].y);
				glVertex2f(add_contours[n][i].d[j].vertex[1].x, add_contours[n][i].d[j].vertex[1].y);
			}
		}
	}



	glEnd();
	glLineWidth(1);

	glDisable(GL_BLEND);




	glLineWidth(2);
	glBegin(GL_LINES);


	for (size_t i = 0; i < avg_contours.size(); i++)
	{
		glColor3f(0, 0, 0);// static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));// colours[i].r, colours[i].g, colours[i].b);

		for (size_t j = 0; j < avg_contours[i].d.size(); j++)
		{
			glVertex2f(avg_contours[i].d[j].vertex[0].x, avg_contours[i].d[j].vertex[0].y);
			glVertex2f(avg_contours[i].d[j].vertex[1].x, avg_contours[i].d[j].vertex[1].y);
		}
	}

	glEnd();
	glLineWidth(1);






//	glLineWidth(1);
//	glBegin(GL_LINES);

	//srand(1234);


	//// Draw contours
	//for (size_t i = 0; i < final_contours.size(); i++)
	//{
	//	glColor3f(1,1, 1);// static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));// colours[i].r, colours[i].g, colours[i].b);

	//	for (size_t j = 0; j < final_contours[i].d.size(); j++)
	//	{
	//		glVertex2f(final_contours[i].d[j].vertex[0].x, final_contours[i].d[j].vertex[0].y);
	//		glVertex2f(final_contours[i].d[j].vertex[1].x, final_contours[i].d[j].vertex[1].y);

	//		// Draw normal
	//		//vertex_2 v((final_contours[i].d[j].vertex[0].x + final_contours[i].d[j].vertex[1].x) * 0.5f,
	//		//	(final_contours[i].d[j].vertex[0].y + final_contours[i].d[j].vertex[1].y) * 0.5f);

	//		//glVertex2f(v.x, v.y);
	//		//glVertex2f(v.x + normals[i][j].x / 50, v.y + normals[i][j].y / 50);
	//	}
	//}

	//glEnd();
	//glLineWidth(1);



	// Render image outline edges.
	//glLineWidth(2);
	//glBegin(GL_LINES);

	//for(size_t i = 0; i < line_segments.size(); i++)
	//{
	//	glColor3f(colours[i].r, colours[i].g, colours[i].b);

	//	for (size_t j = 0; j < line_segments[i].size(); j++)
	//	{
	//		glVertex2f(line_segments[i][j].vertex[0].x, line_segments[i][j].vertex[0].y);
	//		glVertex2f(line_segments[i][j].vertex[1].x, line_segments[i][j].vertex[1].y);
	//	}
	//}

	//glEnd();
	//glLineWidth(1);


	//glPointSize(12);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < train_points.size(); i++)
	//{
	//	glColor3f(0, 0, 0);

	//	for (size_t j = 0; j < train_points[i].size(); j++)
	//	{
	//		glVertex2f(train_points[i][j].x, train_points[i][j].y);
	//	}
	//}
	//glEnd();

	//glPointSize(8);

	//glBegin(GL_POINTS);

	//for (size_t i = 0; i < train_points.size(); i++)
	//{
	//	glColor3f(colours[i].r, colours[i].g, colours[i].b);

	//	for (size_t j = 0; j < train_points[i].size(); j++)
	//	{
	//		glVertex2f(train_points[i][j].x, train_points[i][j].y);
	//	}
	//}

	////glColor3f(colours[test_point_index].r, colours[test_point_index].g, colours[test_point_index].b);
	////glVertex2f(test_point.x, test_point.y);

	//glEnd();

	glFlush();
}
