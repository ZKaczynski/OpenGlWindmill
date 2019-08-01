void create_cylinder_indieces(GLuint *table, int number_of_slices) {

	int j = 1;
	for (int i = 0; i < number_of_slices; i++) {
		table[3 * i] = 0;
		table[3 * i + 1] = j % (number_of_slices + 1);
		if ((j + 1) % (number_of_slices + 1) == 0) {
			table[3 * i + 2] = 1;
		}
		else {
			table[3 * i + 2] = (j + 1) % (number_of_slices + 1);
		}
		j += 1;
	}
	j = number_of_slices + 2;
	for (int i = 0; i < number_of_slices; i++) {
		table[3 * (number_of_slices + i)] = number_of_slices + 1;
		table[3 * (number_of_slices + i) + 1] = j % (2 * (number_of_slices + 1));


		if (((j + 1) % (2 * (number_of_slices + 1)) == 0)) {
			table[3 * (number_of_slices + i) + 2] = number_of_slices + 2;
		}
		else {
			table[3 * (number_of_slices + i) + 2] = (j + 1) % (2 * (number_of_slices + 1));
		}
		j += 1;
	}
	j = 1;

	for (int i = 0; i < number_of_slices; i++) {
		table[3 * (2 * number_of_slices + i)] = j;
		if ((j + 1) % (number_of_slices + 1) == 0) {
			table[3 * (2 * number_of_slices + i) + 1] = 1;
		}
		else {
			table[3 * (2 * number_of_slices + i) + 1] = (j + 1) % (number_of_slices + 1);
		}
		table[3 * (2 * number_of_slices + i) + 2] = j + number_of_slices + 1;
		j += 1;
	}


	j = 1;

	for (int i = 0; i < number_of_slices; i++) {

		if ((j) % (number_of_slices) == 0) {
			table[3 * (3 * number_of_slices + i)] = 1;
		}
		else {


			table[3 * (3 * number_of_slices + i)] = j + 1;
		}


		table[3 * (3 * number_of_slices + i) + 1] = j + number_of_slices + 1;

		if (j + number_of_slices == 2 * (number_of_slices)) {
			table[3 * (3 * number_of_slices + i) + 2] = number_of_slices + 2;
		}
		else {
			table[3 * (3 * number_of_slices + i) + 2] = (j + number_of_slices + 2);

		}
		j += 1;
	}

}

void create_cylider_vertices(GLfloat *table, int number_of_slices) {
	table[0] = 0.0f;
	table[1] = 0.0f;
	table[2] = 1.0f;
	table[3] = 0.51f;
	table[4] = 0.32f;
	table[5] = 0.0f;
	table[6] = 0.5f;
	table[7] = 1.0f;

	for (int i = 0; i < number_of_slices; i++) {
		table[8 * (i + 1)] = cos(glm::radians((float)(i * 360 / number_of_slices)));
		table[(8 * (i + 1)) + 1] = sin(glm::radians((float)(i * 360 / number_of_slices)));
		table[(8 * (i + 1)) + 2] = 1.0f;
		table[(8 * (i + 1)) + 3] = 0.71f;
		table[(8 * (i + 1)) + 4] = 0.61f;
		table[(8 * (i + 1)) + 5] = 0.3f;

		if (i % 2 == 0) {
			table[(8 * (i + 1)) + 6] = 1.0;

			table[(8 * (i + 1)) + 7] = 0.0f;
		}

		else {
			table[(8 * (i + 1)) + 6] = 0.0f;
			table[(8 * (i + 1)) + 7] = 0.0f;
		}
	}

	table[8 * (number_of_slices + 1)] = 0.0f;
	table[8 * (number_of_slices + 1) + 1] = 0.0f;
	table[8 * (number_of_slices + 1) + 2] = -1.0f;
	table[8 * (number_of_slices + 1) + 3] = 0.71f;
	table[8 * (number_of_slices + 1) + 4] = 0.61f;
	table[8 * (number_of_slices + 1) + 5] = 0.3f;
	table[8 * (number_of_slices + 1) + 6] = 0.0f;
	table[8 * (number_of_slices + 1) + 7] = 0.5f;

	for (int i = 0; i < number_of_slices; i++) {
		table[8 * (2 + number_of_slices) + 8 * i] = cos(glm::radians((float)(i * 360 / number_of_slices)));
		table[(8 * (i + 2 + number_of_slices)) + 1] = sin(glm::radians((float)(i * 360 / number_of_slices)));
		table[(8 * (i + 2 + number_of_slices)) + 2] = -1.0f;
		table[(8 * (i + 2 + number_of_slices)) + 3] = 0.71f;
		table[(8 * (i + 2 + number_of_slices)) + 4] = 0.61f;
		table[(8 * (i + 2 + number_of_slices)) + 5] = 0.32f;

		if (i % 2 == 0) {
			table[(8 * (i + 2 + number_of_slices)) + 6] = 1.0;
			table[(8 * (i + 2 + number_of_slices)) + 7] = 0.0f;
		}
		else {
			table[(8 * (i + 2 + number_of_slices)) + 6] = 0.0f;
			table[(8 * (i + 2 + number_of_slices)) + 7] = 0.0f;
		}
	}

}