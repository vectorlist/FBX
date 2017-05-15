#include <fbxmesh.h>

void FBXMesh::buildBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(FBXVertex), vertices.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
		(GLvoid*)offsetof(FBXVertex, FBXVertex::position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
		(GLvoid*)offsetof(FBXVertex, FBXVertex::normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
		(GLvoid*)offsetof(FBXVertex, FBXVertex::st));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
		GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indices_num = indices.size();
}

void FBXMesh::render(GLuint shader)
{
	if (vao == NULL) return;
	glUseProgram(shader);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}

void FBXMesh::renderBoundingBox()
{
	glRenderMode(GL_LINE);
	glBegin(GL_LINE);
	glColor3f(1, 1, 0);
	glVertex3f(10, 0, 0);
	glVertex3f(-10, 0, 0);

	glEnd();
}
